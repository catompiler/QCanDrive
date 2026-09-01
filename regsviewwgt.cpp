#include "regsviewwgt.h"
#include "regselectdlg.h"
#include "regsviewmodel.h"
#include "regsviewdelegate.h"
#include "regentry.h"
#include "regobject.h"
#include "regvar.h"
#include "regtypes.h"
#include "regutils.h"
#include "settings.h"
#include <QMessageBox>
#include <QApplication>
#include <QItemSelectionModel>
#include <QIODevice>
#include <QByteArray>
#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QScopedPointer>
#include <QTimer>
#include "paramsloader.h"
#include "paramsreader.h"
#include <QProgressDialog>
#include <QDebug>


#define COL_WIDTH_INDEX 75
#define COL_WIDTH_NAME 100
#define COL_WIDTH_DESCR 175
#define COL_WIDTH_TYPE 75
#define COL_WIDTH_MIN_VAL 75
#define COL_WIDTH_MAX_VAL 75
#define COL_WIDTH_DEF_VAL 100
#define COL_WIDTH_VALUE 100
//#define COL_WIDTH_ 50


static const int col_width[] = {
    COL_WIDTH_INDEX,
    COL_WIDTH_NAME,
    COL_WIDTH_DESCR,
    COL_WIDTH_TYPE,
    COL_WIDTH_MIN_VAL,
    COL_WIDTH_MAX_VAL,
    COL_WIDTH_DEF_VAL,
    COL_WIDTH_VALUE,
};
static const int col_width_len = ((sizeof(col_width))/(sizeof(col_width[0])));


RegsViewWgt::RegsViewWgt(QWidget *parent)
    : QTreeView(parent)
{
    m_regsViewModel = new RegsViewModel();
    getTreeView()->setModel(m_regsViewModel);
    //getTreeView()->setItemDelegate(nullptr);

    m_regsViewDelegate = new RegsViewDelegate();
    getTreeView()->setItemDelegate(m_regsViewDelegate);

    m_refreshTimer = new QTimer();
    connect(m_refreshTimer, &QTimer::timeout, this, &RegsViewWgt::refreshRegs);

    m_progressDlg = new QProgressDialog();
    m_progressDlg->setAutoClose(false);
    m_progressDlg->setAutoReset(false);
    m_progressDlg->setMinimumDuration(1000);
    m_progressDlg->reset();
    connect(m_progressDlg, &QProgressDialog::canceled, this, &RegsViewWgt::m_progressDialog_canceled);

    m_paramsLoader = new ParamsLoader();
    m_paramsReader = new ParamsReader();
    connect(m_paramsLoader, &ParamsLoader::errorOccured, this, &RegsViewWgt::m_paramsLoader_errorOccured);
    connect(m_paramsLoader, &ParamsLoader::done, this, &RegsViewWgt::m_paramsLoader_done);
    connect(m_paramsLoader, &ParamsLoader::finished, this, &RegsViewWgt::m_paramsLoader_finished);
    connect(m_paramsLoader, &ParamsLoader::progressChanged, this, &RegsViewWgt::m_paramsLoader_progressChanged);

    /*getTreeView()->setSelectionMode(QAbstractItemView::SingleSelection);
    QItemSelectionModel* sel_model = getTreeView()->selectionModel();
    if(sel_model == nullptr){
        sel_model = new QItemSelectionModel();
        sel_model->setParent(getTreeView());
        getTreeView()->setSelectionModel(sel_model);
    }
    connect(sel_model, &QItemSelectionModel::selectionChanged, this, &RegsViewWgt::m_tvRegList_selection_changed);
    */
    connect(this, &RegsViewWgt::activated, this, &RegsViewWgt::m_tvRegList_activated);

    for(int i = 0; i < col_width_len && i < getTreeView()->model()->columnCount(); i ++){
        getTreeView()->setColumnWidth(i, col_width[i]);
    }

    restoreSettings();
}

RegsViewWgt::~RegsViewWgt()
{
    storeSettings();

    delete m_paramsReader;
    delete m_paramsLoader;

    delete m_progressDlg;

    delete m_refreshTimer;
    delete m_regsViewDelegate;
    delete m_regsViewModel;
}

const RegListModel* RegsViewWgt::regListModel() const
{
    return qobject_cast<RegListModel*>(m_regsViewModel->sourceModel());
}

RegListModel* RegsViewWgt::regListModel()
{
    return qobject_cast<RegListModel*>(m_regsViewModel->sourceModel());
}

void RegsViewWgt::setRegListModel(RegListModel* newRegListModel)
{
    m_regsViewModel->setSourceModel(newRegListModel);
}

SLCanOpenNode* RegsViewWgt::getSLCanOpenNode()
{
    return m_regsViewModel->getSLCanOpenNode();
}

const SLCanOpenNode* RegsViewWgt::getSLCanOpenNode() const
{
    return m_regsViewModel->getSLCanOpenNode();
}

void RegsViewWgt::setSLCanOpenNode(SLCanOpenNode* slcon)
{
    m_regsViewModel->setSLCanOpenNode(slcon);
    m_paramsLoader->setSLCanOpenNode(slcon);
}

CO::NodeId RegsViewWgt::nodeId() const
{
    return m_regsViewModel->nodeId();
}

void RegsViewWgt::setNodeId(CO::NodeId newNodeId)
{
    m_regsViewModel->setNodeId(newNodeId);
    m_paramsLoader->setNodeId(newNodeId);
}

uint RegsViewWgt::regsRefreshPeriod() const
{
    return static_cast<uint>(m_refreshTimer->interval());
}

void RegsViewWgt::setRegsRefreshPeriod(uint newRegsRefreshPeriod)
{
    bool running = m_refreshTimer->isActive();

    m_refreshTimer->stop();
    m_refreshTimer->setInterval(static_cast<int>(newRegsRefreshPeriod));

    if(running) m_refreshTimer->start();
}

void RegsViewWgt::expandTree()
{
    getTreeView()->expandAll();
}

void RegsViewWgt::collapseTree()
{
    getTreeView()->collapseAll();
}

void RegsViewWgt::refreshRegs()
{
    if(state() != QAbstractItemView::EditingState) m_regsViewModel->refreshRegs();
}

void RegsViewWgt::setRefreshingRegs(bool newRefreshing)
{
    if(newRefreshing){
        m_refreshTimer->start();
    }else{
        m_refreshTimer->stop();
        m_regsViewModel->stopRefreshingRegs();
    }
}

void RegsViewWgt::uploadFromDrive()
{
    qDebug() << "RegsViewWgt::uploadFromDrive()";

    RegListModel* regListModel = this->regListModel();
    if(!regListModel){
        qDebug() << "regListModel == NULL";
        return;
    }

    m_paramsLoader->setVarList(regListModel->getRegRapams());
    if(m_paramsLoader->valuesToProcess() == 0){
        QMessageBox::critical(this, tr("Ошибка"), tr("Нет параметров для выгрузки!"));
        return;
    }

    m_progressDlg->reset();
    m_progressDlg->setLabelText(tr("Выгрузка параметров из устройства..."));
    m_progressDlg->setMinimum(0);
    m_progressDlg->setMaximum(m_paramsLoader->valuesToProcess());
    m_progressDlg->setValue(0);

    if(m_paramsLoader->uploadFromDrive()){
        m_progressDlg->show();
    }else{
        qDebug() << "uploadFromDrive failed";
        m_progressDlg->reset();
    }
}

void RegsViewWgt::downloadToDrive()
{
    qDebug() << "RegsViewWgt::downloadToDrive()";

    RegListModel* regListModel = this->regListModel();
    if(!regListModel){
        qDebug() << "regListModel == NULL";
        return;
    }

    m_paramsLoader->setVarList(regListModel->getRegRapams());
    if(m_paramsLoader->valuesToProcess() == 0){
        QMessageBox::critical(this, tr("Ошибка"), tr("Нет параметров для загрузки!"));
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Загрузить параметры"), QString(), tr("Параметры (*.ini)"));
    if(fileName.isEmpty()) return;

    {
        ParamsReader reader;
        ParamsReader::RegValuesList valuesList = reader.read(fileName, m_paramsLoader->varList());
        if(valuesList.isEmpty()){
            QMessageBox::critical(this, tr("Ошибка"), tr("Невозможно прочитать параметры из файла или файл пуст!"));
            return;
        }
        m_paramsLoader->setValuesList(valuesList);
    }

    m_progressDlg->reset();
    m_progressDlg->setLabelText(tr("Загрука параметров в устройство..."));
    m_progressDlg->setMinimum(0);
    m_progressDlg->setMaximum(m_paramsLoader->valuesToProcess());
    m_progressDlg->setValue(0);

    if(m_paramsLoader->downloadToDrive()){
        m_progressDlg->show();
    }else{
        qDebug() << "downloadToDrive failed";
        m_progressDlg->reset();
    }
}

void RegsViewWgt::m_tvRegList_activated(const QModelIndex& index)
{
    //qDebug() << "on_tvRegList_activated";

    if(!index.isValid()) return;

    // entry.
    if(!index.parent().isValid()){

        //RegEntry* re = m_regsViewModel->entryByModelIndex(index);
        //if(re == nullptr) return;

    }
}

void RegsViewWgt::m_tvRegList_selection_changed(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    //qDebug() << "on_tvRegList_activated";

    if(selected.empty()){
    }else{
    }
}

void RegsViewWgt::m_paramsLoader_errorOccured(const QString& errStr)
{
    qDebug() << "RegsViewWgt::m_paramsLoader_errorOccured";

    QMessageBox::critical(this, tr("Ошибка"), errStr);
}

void RegsViewWgt::m_paramsLoader_done()
{
    qDebug() << "RegsViewWgt::m_paramsLoader_done";

    if(m_paramsLoader->lastOperation() == ParamsLoader::OP_UPLOAD_FROM_DRIVE){

        QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить параметры"), QString(), tr("Параметры (*.ini)"));

        if(fileName.isEmpty()) return;

        ParamsReader reader;

        if(!reader.write(fileName, m_paramsLoader->varList(), m_paramsLoader->valuesList())){
            QMessageBox::critical(this, tr("Ошибка"), tr("Невозможно записать параметры в файл!"));
        }
    }else if(m_paramsLoader->lastOperation() == ParamsLoader::OP_DOWNLOAD_TO_DRIVE){
        QMessageBox::information(this, tr("Готово"), tr("Параметры успешно записаны!"));
    }
}

void RegsViewWgt::m_paramsLoader_finished()
{
    qDebug() << "RegsViewWgt::m_paramsLoader_finished";

    m_progressDlg->reset();
    m_progressDlg->hide();
}

void RegsViewWgt::m_paramsLoader_progressChanged(int progress)
{
    qDebug() << "RegsViewWgt::m_paramsLoader_progressChanged";

    m_progressDlg->setValue(progress);
}

void RegsViewWgt::m_progressDialog_canceled()
{
    qDebug() << "RegsViewWgt::m_progressDialog_canceled";

    m_paramsLoader->cancel();
}


void RegsViewWgt::restoreSettings()
{
}

void RegsViewWgt::storeSettings()
{
}

