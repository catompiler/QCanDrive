#include "sdoscope.h"
#include "slcanopennode.h"
#include "sdovalue.h"
#include "regtypes.h"
#include "regutils.h"
#include "reglistmodel.h"
#include "regvar.h"
#include <assert.h>
#include <QDebug>




SDOScope::SDOScope(QObject *parent)
    : QObject(parent)
{
    m_channels = nullptr;

    m_regListModel = nullptr;

    m_slcon = nullptr;

    m_comm = new SDOComm();
    connect(m_comm, &SDOComm::finished, this, &SDOScope::sdoFinished);

    m_nodeId = 1;
    m_entryIndex = 0x2800;
    m_versionSubIndex = VERSION_SUBINDEX;

    m_control = CONTROL_NONE;
    m_status = STATUS_NONE;

    m_error = ERROR_NONE;
    m_state = STATE_NONE;

    m_init_state = INIT_NONE;
    m_init_cur_task = 0;
    populateInitTasks();

    m_update_state = UPDATE_NONE;
    m_update_cur_task = 0;
    m_update_base_ch = 0;
    m_update_base_read = false;
    m_update_base_dataType = DataType::I32;
    m_update_base_value = 1;
    // Только после инициализации (нужны каналы).
    //populateUpdateTasks();

    m_apply_state = APPLY_NONE;
    m_error = ERROR_NONE;
    m_apply_cur_task = 0;
    m_apply_status_read = false;
    // Только после инициализации (нужны каналы).
    //populateApplyTasks();

    m_run_state = RUN_NONE;
    m_run_status_read = false;

    m_read_state = READ_NONE;
    m_read_read = false;
    m_read_ch = 0;

    m_version = 0;
    m_max_channels = 0;
    m_max_samples = 0;
    m_max_sample_rate = 0;

    m_samples = 0;
    m_prescaler = 0;
    m_hist_samples = 0;
    m_mode = 0;

    m_trig_enabled = 0;
    m_trig_ch_n = 0;
    m_trig_type = 0;
    m_trig_value = 0;

    m_start_index = 0;
    m_ring_samples = nullptr;
}

SDOScope::~SDOScope()
{
    if(m_ring_samples) delete[] m_ring_samples;

    disconnect(m_comm, &SDOComm::finished, this, &SDOScope::sdoFinished);
    if(!m_comm->running()){
        delete m_comm;
    }else{
        m_comm->deleteLater();
    }

    if(m_channels) delete[] m_channels;
}

const RegListModel* SDOScope::regListModel() const
{
    return m_regListModel;
}

RegListModel* SDOScope::regListModel()
{
    return m_regListModel;
}

void SDOScope::setRegListModel(RegListModel* newRegListModel)
{
    m_regListModel = newRegListModel;
}

SLCanOpenNode* SDOScope::getSLCanOpenNode()
{
    return m_slcon;
}

void SDOScope::setSLCanOpenNode(SLCanOpenNode* slcon)
{
    m_slcon = slcon;
}

CO::NodeId SDOScope::nodeId() const
{
       return m_nodeId;
}

void SDOScope::setNodeId(CO::NodeId newNodeId)
{
       m_nodeId = newNodeId;
}

CO::Index SDOScope::entryIndex() const
{
    return m_entryIndex;
}

void SDOScope::setEntryIndex(CO::Index newEntryIndex)
{
    m_entryIndex = newEntryIndex;
}

CO::SubIndex SDOScope::versionSubIndex() const
{
    return m_versionSubIndex;
}

void SDOScope::setVersionSubIndex(CO::SubIndex newVersionSubIndex)
{
    m_versionSubIndex = newVersionSubIndex;
}

bool SDOScope::init()
{
    if(!m_slcon) return false;
    if(m_state != STATE_NONE) return false;

    m_init_state = INIT_BEGIN;
    m_state = STATE_INIT;
    m_error = ERROR_NONE;
    m_init_cur_task = 0;

    return processInit() != PROCESSING_ERROR;
}

bool SDOScope::deinit()
{
    m_init_state = INIT_NONE;
    m_state = STATE_NONE;
    m_error = ERROR_NONE;

    return true;
}

bool SDOScope::update()
{
    if(!m_slcon) return false;
    if(!isInitialized()) return false;
    if(m_state != STATE_NONE) return false;

    m_update_state = UPDATE_BEGIN;
    m_state = STATE_UPDATE;
    m_error = ERROR_NONE;
    m_update_cur_task = 0;
    m_update_base_ch = 0;
    m_update_base_read = false;

    return processUpdate() != PROCESSING_ERROR;
}

bool SDOScope::updateCommon()
{
    if(!m_slcon) return false;
    if(!isInitialized()) return false;
    if(m_state != STATE_NONE) return false;

    m_update_state = UPDATE_BEGIN;
    m_state = STATE_UPDATE_COMMON;
    m_error = ERROR_NONE;
    m_update_cur_task = 0;
    m_update_base_ch = 0;
    m_update_base_read = false;

    return processUpdateCommon() != PROCESSING_ERROR;
}

bool SDOScope::updateTrig()
{
    if(!m_slcon) return false;
    if(!isInitialized()) return false;
    if(m_state != STATE_NONE) return false;

    m_update_state = UPDATE_BEGIN;
    m_state = STATE_UPDATE_TRIG;
    m_error = ERROR_NONE;
    m_update_cur_task = 0;
    m_update_base_ch = 0;
    m_update_base_read = false;

    return processUpdateTrig() != PROCESSING_ERROR;
}

bool SDOScope::updateChannels()
{
    if(!m_slcon) return false;
    if(!isInitialized()) return false;
    if(m_state != STATE_NONE) return false;

    m_update_state = UPDATE_BEGIN;
    m_state = STATE_UPDATE_CHANNELS;
    m_error = ERROR_NONE;
    m_update_cur_task = 0;
    m_update_base_ch = 0;
    m_update_base_read = false;

    return processUpdateChannels() != PROCESSING_ERROR;
}

bool SDOScope::apply()
{
    if(!m_slcon) return false;
    if(!isInitialized()) return false;
    if(m_state != STATE_NONE) return false;

    m_apply_state = APPLY_BEGIN;
    m_state = STATE_APPLY;
    m_error = ERROR_NONE;
    m_apply_cur_task = 0;
    m_apply_status_read = false;

    return processApply() != PROCESSING_ERROR;
}

bool SDOScope::applyCommon()
{
    if(!m_slcon) return false;
    if(!isInitialized()) return false;
    if(m_state != STATE_NONE) return false;

    m_apply_state = APPLY_BEGIN;
    m_state = STATE_APPLY_COMMON;
    m_error = ERROR_NONE;
    m_apply_cur_task = 0;
    m_apply_status_read = false;

    return processApplyCommon() != PROCESSING_ERROR;
}

bool SDOScope::applyTrig()
{
    if(!m_slcon) return false;
    if(!isInitialized()) return false;
    if(m_state != STATE_NONE) return false;

    m_apply_state = APPLY_BEGIN;
    m_state = STATE_APPLY_TRIG;
    m_error = ERROR_NONE;
    m_apply_cur_task = 0;
    m_apply_status_read = false;

    return processApplyTrig() != PROCESSING_ERROR;
}

bool SDOScope::applyChannels()
{
    if(!m_slcon) return false;
    if(!isInitialized()) return false;
    if(m_state != STATE_NONE) return false;

    m_apply_state = APPLY_BEGIN;
    m_state = STATE_APPLY_CHANNELS;
    m_error = ERROR_NONE;
    m_apply_cur_task = 0;
    m_apply_status_read = false;

    return processApplyChannels() != PROCESSING_ERROR;
}

bool SDOScope::run()
{
    if(!m_slcon) return false;
    if(!isInitialized()) return false;
    if(m_state != STATE_NONE) return false;

    m_run_state = RUN_BEGIN;
    m_state = STATE_RUN;
    m_error = ERROR_NONE;
    m_run_status_read = false;

    return processRun() != PROCESSING_ERROR;
}

bool SDOScope::read()
{
    if(!m_slcon) return false;
    if(!isInitialized()) return false;
    if(m_state != STATE_NONE) return false;

    m_read_state = READ_BEGIN;
    m_state = STATE_READ;
    m_error = ERROR_NONE;
    m_read_ch = 0;
    m_read_read = false;

    return processRead() != PROCESSING_ERROR;
}

void SDOScope::sdoFinished()
{
    switch(m_state){
    default:
        break;
    case STATE_NONE:
        break;
    case STATE_INIT:
        processInit();
        break;
    case STATE_UPDATE:
        processUpdate();
        break;
    case STATE_UPDATE_COMMON:
        processUpdateCommon();
        break;
    case STATE_UPDATE_TRIG:
        processUpdateTrig();
        break;
    case STATE_UPDATE_CHANNELS:
        processUpdateChannels();
        break;
    case STATE_APPLY:
        processApply();
        break;
    case STATE_APPLY_COMMON:
        processApplyCommon();
        break;
    case STATE_APPLY_TRIG:
        processApplyTrig();
        break;
    case STATE_APPLY_CHANNELS:
        processApplyChannels();
        break;
    case STATE_RUN:
        processRun();
        break;
    case STATE_READ:
        processRead();
        break;
    }
}

uint32_t SDOScope::maxSampleRate() const
{
    return m_max_sample_rate;
}

SDOScope::Error SDOScope::error() const
{
    return m_error;
}

bool SDOScope::isInitialized() const
{
    return m_init_state == INIT_DONE;
}

bool SDOScope::isUpdated() const
{
    return m_update_state == UPDATE_DONE;
}

uint SDOScope::channelsCount() const
{
    return m_max_channels;
}

uint SDOScope::samplesCount() const
{
    return m_samples;
}

uint SDOScope::sampleRate() const
{
    return m_max_sample_rate / (m_prescaler + 1);
}

uint SDOScope::histSamplesCount() const
{
    return m_hist_samples;
}

uint SDOScope::mode() const
{
    return m_mode;
}

SDOScope::Channel* SDOScope::channel(uint i)
{
    if(i >= m_max_channels) return nullptr;

    return &m_channels[i];
}

const SDOScope::Channel* SDOScope::channel(uint i) const
{
    if(i >= m_max_channels) return nullptr;

    return &m_channels[i];
}

uint SDOScope::maxSamplesCount() const
{
    return m_max_samples;
}

uint SDOScope::maxChannelsCount() const
{
    return m_max_channels;
}

uint SDOScope::version() const
{
    return m_version;
}

SDOScope::ProcessingState SDOScope::processInit()
{
    assert(m_slcon != nullptr);

    Error proc_err = ERROR_NONE;
    ProcessingState proc_state = PROCESSING_IN_PROGRES;


    // Обработка состояния.
    switch(m_init_state){
    default:
    case INIT_NONE:
        m_state = STATE_NONE;
        return PROCESSING_DONE;

    case INIT_BEGIN:{
        // Прочитать версию.
        SDOComm* comm = m_slcon->read(m_nodeId, m_entryIndex, m_versionSubIndex, &m_version, sizeof(m_version), m_comm);
        if(comm == nullptr){
            proc_err = ERROR_COMM;
            proc_state = PROCESSING_ERROR;
            break;
        }
        m_init_state = INIT_VERSION;
    }break;

    case INIT_VERSION:{
        // Ожидание завершения предыдущего этапа.
        if(!m_comm->isFinished()){
            break;
        }
        if(m_comm->hasError()){
            proc_err = ERROR_COMM;
            proc_state = PROCESSING_ERROR;
            break;
        }

        // Проверим версию.
        if(m_version != OSCOPE_VERSION){
            proc_err = ERROR_VERSION;
            proc_state = PROCESSING_ERROR;
            break;
        }

        m_init_state = INIT_TASKS;
    }

    __attribute__((fallthrough));
    case INIT_TASKS:{
        // Ожидание завершения предыдущего этапа.
        if(!m_comm->isFinished()){
            break;
        }
        if(m_comm->hasError()){
            proc_err = ERROR_COMM;
            proc_state = PROCESSING_ERROR;
            break;
        }

        if(m_init_cur_task < static_cast<uint>(m_init_tasks.count())){
            CommTask* ct = &m_init_tasks[m_init_cur_task];
            // Следующие данные.
            if(!runCommTask(ct)){
                proc_err = ERROR_COMM;
                proc_state = PROCESSING_ERROR;
                break;
            }

            m_init_cur_task ++;
            break;
        }else{

            if(m_max_channels > MAX_CHANNELS){
                proc_err = ERROR_CONSTRAINTS;
                proc_state = PROCESSING_ERROR;
                break;
            }

            if(m_max_samples > MAX_SAMPLES){
                proc_err = ERROR_CONSTRAINTS;
                proc_state = PROCESSING_ERROR;
                break;
            }

            // Изменим число каналов.
            if(m_channels) delete[] m_channels;
            m_channels = new Channel[m_max_channels];

            // Изменим число семплов в каналах.
            for(uint i = 0; i < m_max_channels; i ++){
                m_channels[i].resize(m_max_samples);
                // remove after debug
                //memset(m_channels[i].m_samples, 0xDEADBEEF, m_max_samples*sizeof(Channel::sample_t));
            }

            // Выделим буфер для чтения данных каналов.
            if(m_ring_samples) delete[] m_ring_samples;
            m_ring_samples = new Channel::sample_t[m_max_samples];

            // Каналы теперь созданы -
            // обновим задания для обновления.
            populateUpdateTasks();
            // и применения настроек.
            populateApplyTasks();

            m_init_state = INIT_DONE;
        }
    }

    __attribute__((fallthrough));
    case INIT_DONE:{
        proc_err = ERROR_NONE;
        proc_state = PROCESSING_DONE;
    }break;
    }


    // Обработка результата.
    switch(proc_state){
    default:
    case PROCESSING_DONE:
        m_state = STATE_NONE;
        m_error = proc_err;

        emit initialized();
        handleFinished();
        break;

    case PROCESSING_ERROR:
        m_state = STATE_NONE;
        m_error = proc_err;

        handleError();
        break;

    case PROCESSING_IN_PROGRES:
        break;
    }

    return proc_state;
}

void SDOScope::populateInitTasks()
{
    m_init_tasks.clear();
    m_init_tasks.reserve(3);

    m_init_tasks.append({CommTask::READ, MAX_CHANNELS_SUBINDEX, &m_max_channels, sizeof(m_max_channels)});
    m_init_tasks.append({CommTask::READ, MAX_SAMPLES_SUBINDEX, &m_max_samples, sizeof(m_max_samples)});
    m_init_tasks.append({CommTask::READ, MAX_SAMPLE_RATE_SUBINDEX, &m_max_sample_rate, sizeof(m_max_sample_rate)});
}

SDOScope::ProcessingState SDOScope::processUpdate()
{
    assert(m_slcon != nullptr);

    Error proc_err = ERROR_NONE;
    ProcessingState proc_state = PROCESSING_IN_PROGRES;


    auto res_pair = processUpdateImpl(true, true, true);
    proc_state = res_pair.first; proc_err = res_pair.second;


    // Обработка результата.
    switch(proc_state){
    default:
    case PROCESSING_DONE:
        m_state = STATE_NONE;
        m_error = proc_err;

        emit updated();
        handleFinished();
        break;

    case PROCESSING_ERROR:
        m_state = STATE_NONE;
        m_error = proc_err;

        handleError();
        break;

    case PROCESSING_IN_PROGRES:
        break;
    }

    return proc_state;
}

SDOScope::ProcessingState SDOScope::processUpdateCommon()
{
    assert(m_slcon != nullptr);

    Error proc_err = ERROR_NONE;
    ProcessingState proc_state = PROCESSING_IN_PROGRES;


    auto res_pair = processUpdateImpl(true, false, false);
    proc_state = res_pair.first; proc_err = res_pair.second;


    // Обработка результата.
    switch(proc_state){
    default:
    case PROCESSING_DONE:
        m_state = STATE_NONE;
        m_error = proc_err;

        emit updatedCommon();
        handleFinished();
        break;

    case PROCESSING_ERROR:
        m_state = STATE_NONE;
        m_error = proc_err;

        handleError();
        break;

    case PROCESSING_IN_PROGRES:
        break;
    }

    return proc_state;
}

SDOScope::ProcessingState SDOScope::processUpdateTrig()
{
    assert(m_slcon != nullptr);

    Error proc_err = ERROR_NONE;
    ProcessingState proc_state = PROCESSING_IN_PROGRES;


    auto res_pair = processUpdateImpl(false, true, false);
    proc_state = res_pair.first; proc_err = res_pair.second;


    // Обработка результата.
    switch(proc_state){
    default:
    case PROCESSING_DONE:
        m_state = STATE_NONE;
        m_error = proc_err;

        emit updatedTrig();
        handleFinished();
        break;

    case PROCESSING_ERROR:
        m_state = STATE_NONE;
        m_error = proc_err;

        handleError();
        break;

    case PROCESSING_IN_PROGRES:
        break;
    }

    return proc_state;
}

SDOScope::ProcessingState SDOScope::processUpdateChannels()
{
    assert(m_slcon != nullptr);

    Error proc_err = ERROR_NONE;
    ProcessingState proc_state = PROCESSING_IN_PROGRES;


    auto res_pair = processUpdateImpl(false, false, true);
    proc_state = res_pair.first; proc_err = res_pair.second;


    // Обработка результата.
    switch(proc_state){
    default:
    case PROCESSING_DONE:
        m_state = STATE_NONE;
        m_error = proc_err;

        emit updatedChannels();
        handleFinished();
        break;

    case PROCESSING_ERROR:
        m_state = STATE_NONE;
        m_error = proc_err;

        handleError();
        break;

    case PROCESSING_IN_PROGRES:
        break;
    }

    return proc_state;
}

QPair<SDOScope::ProcessingState, SDOScope::Error> SDOScope::processUpdateImpl(bool updCommon, bool updTrig, bool updChannels)
{
    assert(m_slcon != nullptr);

    Error proc_err = ERROR_NONE;
    ProcessingState proc_state = PROCESSING_IN_PROGRES;


    // Обработка состояния.
    switch(m_update_state){
    default:
    case UPDATE_NONE:
        m_state = STATE_NONE;
        break;

    case UPDATE_BEGIN:
        m_update_state = UPDATE_COMMON;

    __attribute__((fallthrough));
    case UPDATE_COMMON:{
        // Если нужно обновить общие настройки.
        if(updCommon){
            // Ожидание завершения предыдущего этапа.
            if(!m_comm->isFinished()){
                break;
            }
            if(m_comm->hasError()){
                proc_err = ERROR_COMM;
                proc_state = PROCESSING_ERROR;
                break;
            }

            if(m_update_cur_task < static_cast<uint>(m_update_common_tasks.count())){
                CommTask* ct = &m_update_common_tasks[m_update_cur_task];
                // Следующие данные.
                if(!runCommTask(ct)){
                    proc_err = ERROR_COMM;
                    proc_state = PROCESSING_ERROR;
                    break;
                }

                m_update_cur_task ++;
                break;
            }
            m_update_cur_task = 0;
        }

        m_update_state = UPDATE_TRIG;
    }

    __attribute__((fallthrough));
    case UPDATE_TRIG:{
        // Если нужно обновить настройки триггера.
        if(updTrig){
            // Ожидание завершения предыдущего этапа.
            if(!m_comm->isFinished()){
                break;
            }
            if(m_comm->hasError()){
                proc_err = ERROR_COMM;
                proc_state = PROCESSING_ERROR;
                break;
            }

            if(m_update_cur_task < static_cast<uint>(m_update_trig_tasks.count())){
                CommTask* ct = &m_update_trig_tasks[m_update_cur_task];
                // Следующие данные.
                if(!runCommTask(ct)){
                    proc_err = ERROR_COMM;
                    proc_state = PROCESSING_ERROR;
                    break;
                }

                m_update_cur_task ++;
                break;
            }
            m_update_cur_task = 0;
        }

        m_update_state = UPDATE_CHANNELS;
    }

    __attribute__((fallthrough));
    case UPDATE_CHANNELS:{
        // Если нужно обновить настройки каналов.
        if(updChannels){
            // Ожидание завершения предыдущего этапа.
            if(!m_comm->isFinished()){
                break;
            }
            if(m_comm->hasError()){
                proc_err = ERROR_COMM;
                proc_state = PROCESSING_ERROR;
                break;
            }

            if(m_update_cur_task < static_cast<uint>(m_update_channels_tasks.count())){
                CommTask* ct = &m_update_channels_tasks[m_update_cur_task];
                // Следующие данные.
                if(!runCommTask(ct)){
                    proc_err = ERROR_COMM;
                    proc_state = PROCESSING_ERROR;
                    break;
                }

                m_update_cur_task ++;
                break;
            }

            // Теперь, когда все данные обновлены, обновим типы данных каналов.
            for (uint i = 0; i < m_max_channels; i ++) {
                Channel* ch = &m_channels[i];

                RegVar* rv = findRegVar(ch->regIndex(), ch->regSubIndex());
                if(rv == nullptr) continue;

                ch->setDataType(rv->dataType());
            }
        }

        m_update_state = UPDATE_BASE;
    }

    __attribute__((fallthrough));
    case UPDATE_BASE:{
        // Если нужно обновлять настройки каналов.
        if(updChannels){
            // Ожидание завершения предыдущего этапа.
            if(!m_comm->isFinished()){
                break;
            }
            if(m_comm->hasError()){
                proc_err = ERROR_COMM;
                proc_state = PROCESSING_ERROR;
                break;
            }

            // Если чтение данных уже было запущено.
            if(m_update_base_read){
                // Канал.
                Channel* ch_readed = &m_channels[m_update_base_ch];
                // Установим базовое значение канала.
                ch_readed->setBaseValue(COValue::valueFrom<qreal>(&m_update_base_value, m_update_base_dataType, 1.0));

                // Следующий канал.
                m_update_base_ch ++;
            }

            // Канал.
            Channel* ch = nullptr;
            // Регистр значения канала.
            RegVar* rv = nullptr;
            // Регистр базового значения.
            RegVar* rv_base = nullptr;

            // По всем каналам.
            while(m_update_base_ch < m_max_channels){
                // Получим текущий канал.
                ch = &m_channels[m_update_base_ch];

                // Найдём регистр.
                rv = findRegVar(ch->regIndex(), ch->regSubIndex());
                // Если не найден - пропустим
                if(rv == nullptr){
                    m_update_base_ch ++;
                    continue;
                }

                // Найдём регистр базового значения.
                rv_base = findRegVar(rv->baseIndex(), rv->baseSubIndex());
                // Если не найден, тоже пропустим - читать просто нечего.
                if(rv_base == nullptr){
                    m_update_base_ch ++;
                    continue;
                }

                break;
            }

            // Если есть откуда обновлять.
            // Если список не окончен и регистр найден - запустим чтение его базового значения.
            if(ch != nullptr && rv != nullptr && rv_base != nullptr){

                m_update_base_dataType = rv_base->dataType();

                SDOComm* comm = m_slcon->read(m_nodeId, rv->baseIndex(), rv->baseSubIndex(), &m_update_base_value, sizeof(m_update_base_value), m_comm);
                if(comm == nullptr){
                    proc_err = ERROR_COMM;
                    proc_state = PROCESSING_ERROR;
                    break;
                }

                // Установим флаг чтения.
                m_update_base_read = true;

                // Выходим и ожидаем завершения чтения.
                break;
            }
        }

        m_update_state = UPDATE_DONE;
    }

    __attribute__((fallthrough));
    case UPDATE_DONE:{
        proc_err = ERROR_NONE;
        proc_state = PROCESSING_DONE;
    }break;
    }


    return qMakePair(proc_state, proc_err);
}

void SDOScope::populateUpdateTasks()
{
    // Применение общих настроек.
    m_update_common_tasks.clear();
    m_update_common_tasks.reserve(4);
    m_update_common_tasks.append({CommTask::READ, SAMPLES_SUBINDEX, &m_samples, sizeof(m_samples)});
    m_update_common_tasks.append({CommTask::READ, PRESCALER_SUBINDEX, &m_prescaler, sizeof(m_prescaler)});
    m_update_common_tasks.append({CommTask::READ, HIST_SAMPLES_SUBINDEX, &m_hist_samples, sizeof(m_hist_samples)});
    m_update_common_tasks.append({CommTask::READ, MODE_SUBINDEX, &m_mode, sizeof(m_mode)});

    // Применение настроек триггера.
    m_update_trig_tasks.clear();
    m_update_trig_tasks.reserve(4);
    m_update_trig_tasks.append({CommTask::READ, TRIG_ENABLED_SUBINDEX, &m_trig_enabled, sizeof(m_trig_enabled)});
    m_update_trig_tasks.append({CommTask::READ, TRIG_CH_N_SUBINDEX, &m_trig_ch_n, sizeof(m_trig_ch_n)});
    m_update_trig_tasks.append({CommTask::READ, TRIG_TYPE_SUBINDEX, &m_trig_type, sizeof(m_trig_type)});
    m_update_trig_tasks.append({CommTask::READ, TRIG_VALUE_SUBINDEX, &m_trig_value, sizeof(m_trig_value)});

    // Применение настроек каналов.
    m_update_channels_tasks.clear();
    m_update_channels_tasks.reserve(m_max_channels * 2);
    for(uint i = 0; i < m_max_channels; i ++){
        m_update_channels_tasks.append({CommTask::READ, static_cast<CO::SubIndex>(CH0_ENABLED_SUBINDEX + i * (CH1_ENABLED_SUBINDEX - CH0_ENABLED_SUBINDEX)), m_channels[i].enabledPtr(), sizeof(decltype(*m_channels[i].enabledPtr()))});
        m_update_channels_tasks.append({CommTask::READ, static_cast<CO::SubIndex>(CH0_REG_ID_SUBINDEX + i * (CH1_REG_ID_SUBINDEX - CH0_REG_ID_SUBINDEX)), m_channels[i].regIdPtr(), sizeof(decltype(*m_channels[i].regIdPtr()))});
    }
//    m_update_tasks.append({CommTask::READ, , &m_, sizeof(m_)});
}

SDOScope::ProcessingState SDOScope::processApply()
{
    assert(m_slcon != nullptr);

    Error proc_err = ERROR_NONE;
    ProcessingState proc_state = PROCESSING_IN_PROGRES;


    auto res_pair = processApplyImpl(true, true, true);
    proc_state = res_pair.first; proc_err = res_pair.second;


    // Обработка результата.
    switch(proc_state){
    default:
    case PROCESSING_DONE:
        m_state = STATE_NONE;
        m_error = proc_err;

        emit applied();
        handleFinished();
        break;

    case PROCESSING_ERROR:
        m_state = STATE_NONE;
        m_error = proc_err;

        handleError();
        break;

    case PROCESSING_IN_PROGRES:
        break;
    }

    return proc_state;
}

SDOScope::ProcessingState SDOScope::processApplyCommon()
{
    assert(m_slcon != nullptr);

    Error proc_err = ERROR_NONE;
    ProcessingState proc_state = PROCESSING_IN_PROGRES;


    auto res_pair = processApplyImpl(true, false, false);
    proc_state = res_pair.first; proc_err = res_pair.second;


    // Обработка результата.
    switch(proc_state){
    default:
    case PROCESSING_DONE:
        m_state = STATE_NONE;
        m_error = proc_err;

        emit appliedCommon();
        handleFinished();
        break;

    case PROCESSING_ERROR:
        m_state = STATE_NONE;
        m_error = proc_err;

        handleError();
        break;

    case PROCESSING_IN_PROGRES:
        break;
    }

    return proc_state;
}

SDOScope::ProcessingState SDOScope::processApplyTrig()
{
    assert(m_slcon != nullptr);

    Error proc_err = ERROR_NONE;
    ProcessingState proc_state = PROCESSING_IN_PROGRES;


    auto res_pair = processApplyImpl(false, true, false);
    proc_state = res_pair.first; proc_err = res_pair.second;


    // Обработка результата.
    switch(proc_state){
    default:
    case PROCESSING_DONE:
        m_state = STATE_NONE;
        m_error = proc_err;

        emit appliedTrig();
        handleFinished();
        break;

    case PROCESSING_ERROR:
        m_state = STATE_NONE;
        m_error = proc_err;

        handleError();
        break;

    case PROCESSING_IN_PROGRES:
        break;
    }

    return proc_state;
}

SDOScope::ProcessingState SDOScope::processApplyChannels()
{
    assert(m_slcon != nullptr);

    Error proc_err = ERROR_NONE;
    ProcessingState proc_state = PROCESSING_IN_PROGRES;


    auto res_pair = processApplyImpl(false, false, true);
    proc_state = res_pair.first; proc_err = res_pair.second;


    // Обработка результата.
    switch(proc_state){
    default:
    case PROCESSING_DONE:
        m_state = STATE_NONE;
        m_error = proc_err;

        emit appliedChannels();
        handleFinished();
        break;

    case PROCESSING_ERROR:
        m_state = STATE_NONE;
        m_error = proc_err;

        handleError();
        break;

    case PROCESSING_IN_PROGRES:
        break;
    }

    return proc_state;
}

QPair<SDOScope::ProcessingState, SDOScope::Error> SDOScope::processApplyImpl(bool applCommon, bool applTrig, bool applChannels)
{
    assert(m_slcon != nullptr);

    Error proc_err = ERROR_NONE;
    ProcessingState proc_state = PROCESSING_IN_PROGRES;


    // Обработка состояния.
    switch(m_apply_state){
    default:
    case APPLY_NONE:
        m_state = STATE_NONE;
        break;

    case APPLY_BEGIN:{
        m_apply_state = APPLY_COMMON;
    }

    __attribute__((fallthrough));
    case APPLY_COMMON:{
        // Если нужно применить общие настройки.
        if(applCommon){
            // Ожидание завершения предыдущего этапа.
            if(!m_comm->isFinished()){
                break;
            }
            if(m_comm->hasError()){
                proc_err = ERROR_COMM;
                proc_state = PROCESSING_ERROR;
                break;
            }

            if(m_apply_cur_task < static_cast<uint>(m_apply_common_tasks.count())){
                CommTask* ct = &m_apply_common_tasks[m_apply_cur_task];
                // Следующие данные.
                if(!runCommTask(ct)){
                    proc_err = ERROR_COMM;
                    proc_state = PROCESSING_ERROR;
                    break;
                }

                m_apply_cur_task ++;
                break;
            }
            m_apply_cur_task = 0;
        }

        m_apply_state = APPLY_TRIG;
    }

    __attribute__((fallthrough));
    case APPLY_TRIG:{
        // Если нужно применить настройки триггера.
        if(applTrig){
            // Ожидание завершения предыдущего этапа.
            if(!m_comm->isFinished()){
                break;
            }
            if(m_comm->hasError()){
                proc_err = ERROR_COMM;
                proc_state = PROCESSING_ERROR;
                break;
            }

            if(m_apply_cur_task < static_cast<uint>(m_apply_trig_tasks.count())){
                CommTask* ct = &m_apply_trig_tasks[m_apply_cur_task];
                // Следующие данные.
                if(!runCommTask(ct)){
                    proc_err = ERROR_COMM;
                    proc_state = PROCESSING_ERROR;
                    break;
                }

                m_apply_cur_task ++;
                break;
            }
            m_apply_cur_task = 0;
        }

        m_apply_state = APPLY_CHANNELS;
    }

    __attribute__((fallthrough));
    case APPLY_CHANNELS:{
        // Если нужно применить настройки каналов.
        if(applChannels){
            // Ожидание завершения предыдущего этапа.
            if(!m_comm->isFinished()){
                break;
            }
            if(m_comm->hasError()){
                proc_err = ERROR_COMM;
                proc_state = PROCESSING_ERROR;
                break;
            }

            if(m_apply_cur_task < static_cast<uint>(m_apply_channels_tasks.count())){
                CommTask* ct = &m_apply_channels_tasks[m_apply_cur_task];
                // Следующие данные.
                if(!runCommTask(ct)){
                    proc_err = ERROR_COMM;
                    proc_state = PROCESSING_ERROR;
                    break;
                }

                m_apply_cur_task ++;
                break;
            }

            // Сброс статуса.
            m_status = STATUS_NONE;
            // Записать.
            SDOComm* comm = m_slcon->write(m_nodeId, m_entryIndex, STATUS_SUBINDEX, &m_status, sizeof(m_status), m_comm);
            if(comm == nullptr){
                proc_err = ERROR_COMM;
                proc_state = PROCESSING_ERROR;
                break;
            }
        }

        m_apply_state = APPLY_WAIT;
    }

    __attribute__((fallthrough));
    case APPLY_WAIT:{
        // Если нужно применить настройки каналов.
        if(applChannels){
            // Ожидание завершения предыдущего этапа.
            if(!m_comm->isFinished()){
                break;
            }
            if(m_comm->hasError()){
                proc_err = ERROR_COMM;
                proc_state = PROCESSING_ERROR;
                break;
            }

            //qDebug() << m_control << m_status;

            if(!m_apply_status_read || !(m_status & STATUS_READY)){
                // Если статус не читался или осциллограф ещё не применил настройки - продолжим ждать.
                // Прочитаем статус.
                SDOComm* comm = m_slcon->read(m_nodeId, m_entryIndex, STATUS_SUBINDEX, &m_status, sizeof(m_status), m_comm);
                if(comm == nullptr){
                    proc_err = ERROR_COMM;
                    proc_state = PROCESSING_ERROR;
                    break;
                }

                // Установим флаг чтения статуса.
                m_apply_status_read = true;
                break;
            }
        }

        m_apply_state = APPLY_DONE;
    }

    __attribute__((fallthrough));
    case APPLY_DONE:{
        proc_err = ERROR_NONE;
        proc_state = PROCESSING_DONE;
    }break;
    }


    return qMakePair(proc_state, proc_err);
}

void SDOScope::populateApplyTasks()
{
    // Применение общих настроек.
    m_apply_common_tasks.clear();
    m_apply_common_tasks.reserve(4);
    m_apply_common_tasks.append({CommTask::WRITE, SAMPLES_SUBINDEX, &m_samples, sizeof(m_samples)});
    m_apply_common_tasks.append({CommTask::WRITE, PRESCALER_SUBINDEX, &m_prescaler, sizeof(m_prescaler)});
    m_apply_common_tasks.append({CommTask::WRITE, HIST_SAMPLES_SUBINDEX, &m_hist_samples, sizeof(m_hist_samples)});
    m_apply_common_tasks.append({CommTask::WRITE, MODE_SUBINDEX, &m_mode, sizeof(m_mode)});

    // Применение настроек триггера.
    m_apply_trig_tasks.clear();
    m_apply_trig_tasks.reserve(4);
    m_apply_trig_tasks.append({CommTask::WRITE, TRIG_ENABLED_SUBINDEX, &m_trig_enabled, sizeof(m_trig_enabled)});
    m_apply_trig_tasks.append({CommTask::WRITE, TRIG_CH_N_SUBINDEX, &m_trig_ch_n, sizeof(m_trig_ch_n)});
    m_apply_trig_tasks.append({CommTask::WRITE, TRIG_TYPE_SUBINDEX, &m_trig_type, sizeof(m_trig_type)});
    m_apply_trig_tasks.append({CommTask::WRITE, TRIG_VALUE_SUBINDEX, &m_trig_value, sizeof(m_trig_value)});

    // Применение настроек каналов.
    m_apply_channels_tasks.clear();
    m_apply_channels_tasks.reserve(m_max_channels * 2);
    for(uint i = 0; i < m_max_channels; i ++){
        m_apply_channels_tasks.append({CommTask::WRITE, static_cast<CO::SubIndex>(CH0_ENABLED_SUBINDEX + i * (CH1_ENABLED_SUBINDEX - CH0_ENABLED_SUBINDEX)), m_channels[i].enabledPtr(), sizeof(decltype(*m_channels[i].enabledPtr()))});
        m_apply_channels_tasks.append({CommTask::WRITE, static_cast<CO::SubIndex>(CH0_REG_ID_SUBINDEX + i * (CH1_REG_ID_SUBINDEX - CH0_REG_ID_SUBINDEX)), m_channels[i].regIdPtr(), sizeof(decltype(*m_channels[i].regIdPtr()))});
    }
    //    m_apply_tasks.append({CommTask::READ, , &m_, sizeof(m_)});
}

SDOScope::ProcessingState SDOScope::processRun()
{
    assert(m_slcon != nullptr);

    Error proc_err = ERROR_NONE;
    ProcessingState proc_state = PROCESSING_IN_PROGRES;


    // Обработка состояния.
    switch(m_run_state){
    default:
    case RUN_NONE:
        m_state = STATE_NONE;
        return PROCESSING_DONE;

    case RUN_BEGIN:{
        // Сбросить состояние.
        m_status = STATUS_NONE;
        // Записать.
        SDOComm* comm = m_slcon->write(m_nodeId, m_entryIndex, STATUS_SUBINDEX, &m_status, sizeof(m_status), m_comm);
        if(comm == nullptr){
            proc_err = ERROR_COMM;
            proc_state = PROCESSING_ERROR;
            break;
        }
        m_run_state = RUN_START;
    }break;

    case RUN_START:{
        // Ожидание завершения предыдущего этапа.
        if(!m_comm->isFinished()){
            break;
        }
        if(m_comm->hasError()){
            proc_err = ERROR_COMM;
            proc_state = PROCESSING_ERROR;
            break;
        }

        // Установить управление.
        m_control = CONTROL_ENABLE | CONTROL_START;
        // Записать.
        SDOComm* comm = m_slcon->write(m_nodeId, m_entryIndex, CONTROL_SUBINDEX, &m_control, sizeof(m_control), m_comm);
        if(comm == nullptr){
            proc_err = ERROR_COMM;
            proc_state = PROCESSING_ERROR;
            break;
        }

        m_run_state = RUN_WAIT;
    }

    __attribute__((fallthrough));
    case RUN_WAIT:{
        // Ожидание завершения предыдущего этапа.
        if(!m_comm->isFinished()){
            break;
        }
        if(m_comm->hasError()){
            proc_err = ERROR_COMM;
            proc_state = PROCESSING_ERROR;
            break;
        }

        //qDebug() << m_control << m_status;

        if(!m_run_status_read || (m_status & STATUS_RUN)){
            // Если статус не читался или осциллограф всё ещё работает - продолжим ждать.
            // Прочитаем статус.
            SDOComm* comm = m_slcon->read(m_nodeId, m_entryIndex, STATUS_SUBINDEX, &m_status, sizeof(m_status), m_comm);
            if(comm == nullptr){
                proc_err = ERROR_COMM;
                proc_state = PROCESSING_ERROR;
                break;
            }

            // Установим флаг чтения статуса.
            m_run_status_read = true;
            break;
        }

        m_run_state = RUN_DONE;
    }

    __attribute__((fallthrough));
    case RUN_DONE:{
        proc_err = ERROR_NONE;
        proc_state = PROCESSING_DONE;
    }break;
    }


    // Обработка результата.
    switch(proc_state){
    default:
    case PROCESSING_DONE:
        m_state = STATE_NONE;
        m_error = proc_err;

        emit done();
        handleFinished();
        break;

    case PROCESSING_ERROR:
        m_state = STATE_NONE;
        m_error = proc_err;

        handleError();
        break;

    case PROCESSING_IN_PROGRES:
        break;
    }

    return proc_state;
}

// TODO: Добавить чтение индексов осцлиллографа, как следствие, добавить чтение только нужного числа семплов.
SDOScope::ProcessingState SDOScope::processRead()
{
    assert(m_slcon != nullptr);

    Error proc_err = ERROR_NONE;
    ProcessingState proc_state = PROCESSING_IN_PROGRES;


    // Обработка состояния.
    switch(m_read_state){
    default:
    case READ_NONE:
        m_state = STATE_NONE;
        return PROCESSING_DONE;

    case READ_BEGIN:{
        // Прпочитать индекс начала данных.
        SDOComm* comm = m_slcon->read(m_nodeId, m_entryIndex, CUR_INDEX_SUBINDEX, &m_start_index, sizeof(m_start_index), m_comm);
        if(comm == nullptr){
            proc_err = ERROR_COMM;
            proc_state = PROCESSING_ERROR;
            break;
        }
        m_read_state = READ_DATA;
    }

    __attribute__((fallthrough));
    case READ_DATA:{
        // Ожидание завершения предыдущего этапа.
        if(!m_comm->isFinished()){
            break;
        }
        if(m_comm->hasError()){
            proc_err = ERROR_COMM;
            proc_state = PROCESSING_ERROR;
            break;
        }


        // Если чтение уже было запущено,
        // а при достижении данного участка кода
        // ещё и завершено.
        // Не смотря на чтение из предыдущего этапа -
        // в этом случае условие не выполнится.
        if(m_read_read){
            // Скопируем данные в текущий канал с
            // преобразованием кольцевого порядка
            // следования данных в линейный.
            Channel* ch = &m_channels[m_read_ch];
            Channel::sample_t* ch_samples = ch->samplesPtr();

            // Часть буфера с началом данных.
            size_t start_part_index = m_start_index;
            size_t start_part_count = m_samples - m_start_index;
            // Часть буфера с концом данных.
            size_t end_part_index = 0;
            size_t end_part_count = m_start_index;
            // Индексы в целевом буфере.
            size_t dst_start_part_index = 0;
            size_t dst_end_part_index   = start_part_count;

            // Копирование частей данных, если это необходимо (размер не равен нулю).
            if(start_part_count != 0) memcpy(&ch_samples[dst_start_part_index], &m_ring_samples[start_part_index], sizeof(Channel::sample_t) * start_part_count);
            if(end_part_count   != 0) memcpy(&ch_samples[dst_end_part_index  ], &m_ring_samples[end_part_index  ], sizeof(Channel::sample_t) * end_part_count  );

            // Перейдём к следующему каналу.
            m_read_ch ++;
        }

        Channel* ch = nullptr;

        while(m_read_ch < m_max_channels){
            // Канал.
            ch = &m_channels[m_read_ch];

            // Если канал включен - выйдем из цикла и прочитаем его данные.
            if(ch->enabled()){
                break;
            }

            // Следующий канал.
            m_read_ch ++;
        }

        // Если есть канал для чтения.
        if(ch != nullptr && m_read_ch < m_max_channels){
            // Прочитаем данные.
            SDOComm* comm = m_slcon->read(m_nodeId, m_entryIndex, CH_DATA0_SAMPLES_SUBINDEX + m_read_ch, m_ring_samples, sizeof(Channel::sample_t) * m_samples, m_comm);
            if(comm == nullptr){
                proc_err = ERROR_COMM;
                proc_state = PROCESSING_ERROR;
                break;
            }

            // Установим флаг чтения.
            m_read_read = true;
            break;
        }

        m_read_state = READ_DONE;
    }

    __attribute__((fallthrough));
    case READ_DONE:{
        proc_err = ERROR_NONE;
        proc_state = PROCESSING_DONE;
    }break;
    }


    // Обработка результата.
    switch(proc_state){
    default:
    case PROCESSING_DONE:
        m_state = STATE_NONE;
        m_error = proc_err;

        emit readed();
        handleFinished();
        break;

    case PROCESSING_ERROR:
        m_state = STATE_NONE;
        m_error = proc_err;

        handleError();
        break;

    case PROCESSING_IN_PROGRES:
        break;
    }

    return proc_state;
}

void SDOScope::handleError()
{
    // if(m_error == ERROR_COMM){
    //     qDebug() << "SDOScope communication error:" << m_comm->error() << "transfered:" << m_comm->transferedDataSize() << "buffered:" << m_comm->bufferedDataSize();
    // }

    emit errorOccured();
    emit finished();
}

void SDOScope::handleFinished()
{
     emit finished();
}

bool SDOScope::runCommTask(CommTask* ct)
{
    SDOComm* comm = (ct->dir == CommTask::READ)
        ? (m_slcon->read(m_nodeId, m_entryIndex, ct->index, ct->ptr, ct->size, m_comm))
        : (m_slcon->write(m_nodeId, m_entryIndex, ct->index, ct->ptr, ct->size, m_comm));

    if(comm == nullptr){
        return false;
    }

    return true;
}

RegVar* SDOScope::findRegVar(CO::Index rvIndex, CO::SubIndex rvSubIndex)
{
    if(m_regListModel == nullptr) return nullptr;

    return m_regListModel->varByRegIndex(rvIndex, rvSubIndex);
}


SDOScope::Channel::Channel()
{
    m_enabled = 0;
    m_reg_id = 0;
    m_samples = nullptr;
    m_samples_count = 0;
    m_dataType = DataType::IQ24;
    m_base_value = 1.0;
}

SDOScope::Channel::Channel(Channel&& ch)
{
    m_enabled = ch.m_enabled;
    m_reg_id = ch.m_reg_id;
    m_dataType = ch.m_dataType;
    m_base_value = ch.m_base_value;

    // move data.
    m_samples = ch.m_samples;
    m_samples_count = ch.m_samples_count;
    ch.m_samples = nullptr;
    ch.m_samples_count = 0;
}

SDOScope::Channel::~Channel()
{
    if(m_samples) delete[] m_samples;
}

bool SDOScope::Channel::enabled() const
{
    return m_enabled;
}

void SDOScope::Channel::setEnabled(bool newEnabled)
{
    m_enabled = newEnabled;
}

CO::Index SDOScope::Channel::regIndex() const
{
    return RegUtils::getIndex(m_reg_id);
}

void SDOScope::Channel::setRegIndex(CO::Index newIndex)
{
    m_reg_id = RegUtils::makeFullIndex(newIndex, regSubIndex());
}

CO::SubIndex SDOScope::Channel::regSubIndex() const
{
    return RegUtils::getSubIndex(m_reg_id);
}

void SDOScope::Channel::setRegSubIndex(CO::SubIndex newSubIndex)
{
    m_reg_id = RegUtils::makeFullIndex(regIndex(), newSubIndex);
}

uint SDOScope::Channel::samplesCount() const
{
    return m_samples_count;
}

SDOScope::Channel::sample_t SDOScope::Channel::sample(uint i) const
{
    if(m_samples == nullptr) return 0;
    if(i >= m_samples_count) return 0;

    return m_samples[i];
}

bool SDOScope::Channel::setSample(uint i, sample_t value)
{
    if(m_samples == nullptr) return false;
    if(i >= m_samples_count) return false;

    m_samples[i] = value;

    return true;
}

DataType SDOScope::Channel::dataType() const
{
    return m_dataType;
}

void SDOScope::Channel::setDataType(DataType newDataType)
{
    m_dataType = newDataType;
}

qreal SDOScope::Channel::baseValue() const
{
    return m_base_value;
}

void SDOScope::Channel::setBaseValue(qreal newBaseValue)
{
    m_base_value = newBaseValue;
}

qreal SDOScope::Channel::rawValue(uint i) const
{
    if(m_samples == nullptr) return 0.0;
    if(i >= m_samples_count) return 0.0;

    return COValue::valueFrom<qreal>(&m_samples[i], m_dataType);
}

qreal SDOScope::Channel::value(uint i) const
{
    return rawValue(i) * m_base_value;
}

uint32_t* SDOScope::Channel::enabledPtr()
{
    return &m_enabled;
}

const uint32_t* SDOScope::Channel::enabledPtr() const
{
    return &m_enabled;
}

uint32_t* SDOScope::Channel::regIdPtr()
{
    return &m_reg_id;
}

const uint32_t* SDOScope::Channel::regIdPtr() const
{
    return &m_reg_id;
}

SDOScope::Channel::sample_t* SDOScope::Channel::samplesPtr()
{
    return m_samples;
}

const SDOScope::Channel::sample_t* SDOScope::Channel::samplesPtr() const
{
    return m_samples;
}

void SDOScope::Channel::resize(uint newSize)
{
    assert(newSize > 0);

    if(m_samples) delete[] m_samples;

    m_samples = new sample_t[newSize];
    m_samples_count = newSize;
}
