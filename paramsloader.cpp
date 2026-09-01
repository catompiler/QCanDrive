#include "paramsloader.h"
#include "slcanopennode.h"
#include "sdocomm.h"
#include "regentry.h"
#include "regvar.h"



ParamsLoader::ParamsLoader(QObject *parent)
    : QObject{parent}
{
    m_slcon = nullptr;
    m_nodeId = 0;

    m_state = STATE_NONE;
    m_lastOp = OP_NONE;

    m_valuesList = new RegValuesList();
    m_regVarList = new RegVarList();

    m_valuesProcessed = 0;
    m_curVarIndex = 0;

    m_comm = new SDOComm();
    connect(m_comm, &SDOComm::finished, this, &ParamsLoader::sdoFinished);
}

ParamsLoader::~ParamsLoader()
{
    disconnect(m_comm, &SDOComm::finished, this, &ParamsLoader::sdoFinished);
    if(!m_comm->running()){
        delete m_comm;
    }else{
        m_comm->deleteLater();
    }

    delete m_valuesList;
    delete m_regVarList;
}

SLCanOpenNode* ParamsLoader::getSLCanOpenNode()
{
    return m_slcon;
}

void ParamsLoader::setSLCanOpenNode(SLCanOpenNode* slcon)
{
    m_slcon = slcon;
}

CO::NodeId ParamsLoader::nodeId() const
{
    return m_nodeId;
}

void ParamsLoader::setNodeId(CO::NodeId newNodeId)
{
    m_nodeId = newNodeId;
}

RegVarList ParamsLoader::varList()
{
    return *m_regVarList;
}

const RegVarList& ParamsLoader::varList() const
{
    return *m_regVarList;
}

void ParamsLoader::setVarList(RegVarList newVarList)
{
    m_regVarList->swap(newVarList);
}

ParamsLoader::RegValuesList ParamsLoader::valuesList()
{
    return *m_valuesList;
}

const ParamsLoader::RegValuesList& ParamsLoader::valuesList() const
{
    return *m_valuesList;
}

void ParamsLoader::setValuesList(RegValuesList newValuesList)
{
    m_valuesList->swap(newValuesList);
}

ParamsLoader::Operation ParamsLoader::lastOperation() const
{
    return m_lastOp;
}

bool ParamsLoader::busy() const
{
    return m_state != STATE_NONE;
}

bool ParamsLoader::cancelled() const
{
    return m_comm->cancelled();
}

void ParamsLoader::clear()
{
    m_regVarList->clear();
    m_valuesList->clear();
}

bool ParamsLoader::uploadFromDrive()
{
    if(m_slcon == nullptr) return false;
    if(!m_slcon->isConnected()) return false;
    if(m_state != STATE_NONE) return false;
    if(m_comm->running()) return false;

    m_valuesList->clear();

    m_comm->setState(SDOComm::IDLE);
    m_comm->setError(SDOComm::ERROR_NONE);
    m_comm->setCancel(false);

    m_valuesProcessed = 0;
    m_curVarIndex = 0;
    m_state = STATE_UPLOAD;
    m_lastOp = OP_UPLOAD_FROM_DRIVE;

    return processUpload();
}

bool ParamsLoader::downloadToDrive()
{
    if(m_slcon == nullptr) return false;
    if(!m_slcon->isConnected()) return false;
    if(m_state != STATE_NONE) return false;
    if(m_comm->running()) return false;

    if(m_valuesList->isEmpty()) return false;

    m_comm->setState(SDOComm::IDLE);
    m_comm->setError(SDOComm::ERROR_NONE);
    m_comm->setCancel(false);

    m_valuesProcessed = 0;
    m_curVarIndex = 0;
    m_state = STATE_DOWNLOAD;
    m_lastOp = OP_DOWNLOAD_TO_DRIVE;

    return processDownload();
}

bool ParamsLoader::downloadToDrive(RegValuesList newValuesList)
{
    setValuesList(qMove(newValuesList));
    return downloadToDrive();
}

int ParamsLoader::valuesToProcess() const
{
    return m_regVarList->count();
}

void ParamsLoader::sdoFinished()
{
    switch(m_state){
    default:
    case STATE_NONE:
        break;
    case STATE_UPLOAD:
        processUpload();
        break;
    case STATE_DOWNLOAD:
        processDownload();
        break;
    }
}

int ParamsLoader::valuesProcessed() const
{
    return m_valuesProcessed;
}

void ParamsLoader::cancel()
{
    m_comm->cancel();
}

bool ParamsLoader::processUpload()
{
    // Ждём завершения операции.
    if(m_comm->running()){
        return false;
    }

    if(m_comm->cancelled()){
        m_state = STATE_NONE;
        emit finished();
        return false;
    }

    // Есть ошибки.
    if(m_comm->hasError()){
        RegVar* rv = m_regVarList->at(m_curVarIndex);
        RegEntry* re = rv->parent();

        reg_index_t index = (re) ? re->index() : REG_INDEX_INVALID;
        reg_subindex_t sub_index = rv->subIndex();

        // Сообщить об ошибке.
        emit errorOccured(tr("Ошибка чтения регистра параметра %1.%2")
                              .arg(index, 0, 16)
                              .arg(sub_index, 0, 10)
                          );
        // следующая переменная.
        m_curVarIndex ++;
    }else{
        // Обмен данными был начат и успешно завершён.
        if(m_comm->isFinished()){
            RegVar* rv = m_regVarList->at(m_curVarIndex);
            RegEntry* re = rv->parent();

            if(re){
                reg_fullindex_t fullindex = RegUtils::makeFullIndex(
                    re->index(), rv->subIndex()
                    );

                if(RegTypes::unpackTo32(&m_value, &m_value, rv->dataType())){
                    m_valuesList->insert(fullindex, m_value);
                    m_valuesProcessed ++;
                }
            }

            // следующая переменная.
            m_curVarIndex ++;
        }
    }

    for(; m_curVarIndex < m_regVarList->count(); m_curVarIndex ++ ){

        RegVar* rv = m_regVarList->at(m_curVarIndex);
        RegEntry* re = rv->parent();

        if(!re){
            // Сообщить об ошибке.
            emit errorOccured(tr("Отсутствует родитель у параметра \"%1\"!").arg(rv->memAddr()));

            // Следующий параметр.
            continue;
        }

        SDOComm* comm = m_slcon->read(m_nodeId, re->index(), rv->subIndex(), &m_value, RegTypes::sizeBytes(rv->dataType()), m_comm);
        if(!comm){
            // Сообщить об ошибке.
            emit errorOccured(tr("Невозможно начать чтение параметра!"));

            // При ошибке соединения дальнейшее чтение невозможно.
            m_state = STATE_NONE;
            emit finished();
            return false;
        }

        // Выходим и ждём завершения обмена данными.
        break;
    }

    // Обновим прогресс.
    emit progressChanged(m_curVarIndex);

    if(m_curVarIndex == m_regVarList->size()){
        m_state = STATE_NONE;
        emit done();
        emit finished();
    }

    return true;
}

bool ParamsLoader::processDownload()
{
    // Ждём завершения операции.
    if(m_comm->running()){
        return false;
    }

    // Есть ошибки.
    if(m_comm->hasError()){
        RegVar* rv = m_regVarList->at(m_curVarIndex);
        RegEntry* re = rv->parent();

        reg_index_t index = (re) ? re->index() : REG_INDEX_INVALID;
        reg_subindex_t sub_index = rv->subIndex();

        // Сообщить об ошибке.
        emit errorOccured(tr("Ошибка записи параметра %1.%2")
                              .arg(index, 0, 16)
                              .arg(sub_index, 0, 10)
                          );
        // следующая переменная.
        m_curVarIndex ++;
    }else{
        // Обмен данными был начат и успешно завершён.
        if(m_comm->isFinished()){
            m_valuesProcessed ++;

            // следующая переменная.
            m_curVarIndex ++;
        }
    }

    for(; m_curVarIndex < m_regVarList->count(); m_curVarIndex ++ ){
        RegVar* rv = m_regVarList->at(m_curVarIndex);
        RegEntry* re = rv->parent();

        if(!re){
            // Сообщить об ошибке.
            emit errorOccured(tr("Отсутствует родитель у параметра \"%1\"!").arg(rv->memAddr()));

            // Следующий параметр.
            continue;
        }

        reg_fullindex_t fullindex = RegUtils::makeFullIndex(
            re->index(), rv->subIndex()
            );

        if(!m_valuesList->contains(fullindex)){
            // Сообщить об ошибке.
            emit errorOccured(tr("Отсутствует значение для параметра %1.%2!")
                                  .arg(re->index(), 0, 16)
                                  .arg(rv->subIndex(), 0, 10));

            // Следующий параметр.
            continue;
        }

        m_value = m_valuesList->value(fullindex);

        if(!RegTypes::packFrom32(&m_value, &m_value, rv->dataType())){
            // Сообщить об ошибке.
            emit errorOccured(tr("Не числовой тип у параметра %1.%2!")
                                  .arg(re->index(), 0, 16)
                                  .arg(rv->subIndex(), 0, 10));

            // Следующий параметр.
            continue;
        }

        SDOComm* comm = m_slcon->write(m_nodeId, re->index(), rv->subIndex(), &m_value, RegTypes::sizeBytes(rv->dataType()), m_comm);
        if(!comm){
            // Сообщить об ошибке.
            emit errorOccured(tr("Невозможно начать запись параметра!"));

            // При ошибке соединения дальнейшее чтение невозможно.
            m_state = STATE_NONE;
            emit finished();
            return false;
        }

        // Выходим и ждём завершения обмена данными.
        break;
    }

    // Обновим прогресс.
    emit progressChanged(m_curVarIndex);

    if(m_curVarIndex == m_regVarList->size()){
        m_state = STATE_NONE;
        emit done();
        emit finished();
    }

    return true;
}
