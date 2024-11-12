#include "reglisteditorwin.h"
#include "regdelegate.h"
#include "ui_reglisteditorwin.h"
#include "regentrydlg.h"
#include "regselectdlg.h"
#include "flagseditdlg.h"
#include "exportdlg.h"
#include "reglistmodel.h"
#include "regentry.h"
#include "regobject.h"
#include "regvar.h"
#include "regtypes.h"
#include "regutils.h"
#include "reglistxmlserializer.h"
#include "reglistregsexporter.h"
#include "reglistdataexporter.h"
#include "reglistcoexporter.h"
#include "reglistedsexporter.h"
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
#include <QDebug>


#define COL_WIDTH_INDEX 75
#define COL_WIDTH_NAME 100
#define COL_WIDTH_TYPE 50
#define COL_WIDTH_COUNT 50
#define COL_WIDTH_MEM_ADDR 100
#define COL_WIDTH_MIN_VAL 75
#define COL_WIDTH_MAX_VAL 75
#define COL_WIDTH_DEF_VAL 100
#define COL_WIDTH_BASE 75
#define COL_WIDTH_FLAGS 75
#define COL_WIDTH_EXTFLAGS 150
#define COL_WIDTH_DESCR 100
//#define COL_WIDTH_ 50


static const int col_width[] = {
    COL_WIDTH_INDEX,
    COL_WIDTH_NAME,
    COL_WIDTH_TYPE,
    COL_WIDTH_COUNT,
    COL_WIDTH_MEM_ADDR,
    COL_WIDTH_MIN_VAL,
    COL_WIDTH_MAX_VAL,
    COL_WIDTH_DEF_VAL,
    COL_WIDTH_BASE,
    COL_WIDTH_FLAGS,
    COL_WIDTH_EXTFLAGS,
    COL_WIDTH_DESCR
};
static const int col_width_len = ((sizeof(col_width))/(sizeof(col_width[0])));


RegListEditorWin::RegListEditorWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RegListEditorWin)
{
    ui->setupUi(this);
    m_regEntryDlg = new RegEntryDlg();
    m_regSelectDlg = new RegSelectDlg();
    m_flagsEditDlg = new FlagsEditDlg();
    m_exportDlg = new ExportDlg();

    m_regListDelegate = new RegDelegate();
    m_regListDelegate->setRegSelectDialog(m_regSelectDlg);
    m_regListDelegate->setFlagsEditDialog(m_flagsEditDlg);

    m_regsListModel = new RegListModel();
    ui->tvRegList->setModel(m_regsListModel);
    ui->tvRegList->setItemDelegate(m_regListDelegate);

    m_regSelectDlg->setRegListModel(m_regsListModel);

    ui->tvRegList->setSelectionMode(QAbstractItemView::SingleSelection);
    QItemSelectionModel* sel_model = ui->tvRegList->selectionModel();
    if(sel_model == nullptr){
        sel_model = new QItemSelectionModel();
        sel_model->setParent(ui->tvRegList);
        ui->tvRegList->setSelectionModel(sel_model);
    }
    connect(sel_model, &QItemSelectionModel::selectionChanged, this, &RegListEditorWin::tvRegList_selection_changed);

    for(int i = 0; i < col_width_len; i ++){
        ui->tvRegList->setColumnWidth(i, col_width[i]);
    }
}

RegListEditorWin::~RegListEditorWin()
{
    delete ui;
    delete m_exportDlg;
    delete m_flagsEditDlg;
    delete m_regSelectDlg;
    delete m_regEntryDlg;
    delete m_regsListModel;
    delete m_regListDelegate;
}

void RegListEditorWin::on_actOpen_triggered(bool checked)
{
    Q_UNUSED(checked);

    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Открыть файлы"), m_curDir, tr("Файлы списка регистров (*.regxml)"));

    if(filenames.isEmpty()) return;

    m_curDir = QDir(filenames.front()).path();

    m_regsListModel->setRegList(RegEntryList());

    for(auto& filename: filenames){
        appendFile(filename);
    }
}

void RegListEditorWin::on_actOpenAppend_triggered(bool checked)
{
    Q_UNUSED(checked);

    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Добавить файлы"), m_curDir, tr("Файлы списка регистров (*.regxml)"));

    if(filenames.isEmpty()) return;

    m_curDir = QDir(filenames.front()).path();

    for(auto& filename: filenames){
        appendFile(filename);
    }
}

void RegListEditorWin::on_actSaveAs_triggered(bool checked)
{
    Q_UNUSED(checked);

    QString filename = QFileDialog::getSaveFileName(this, tr("Сохранить файл"), m_curDir, tr("Файлы списка регистров (*.regxml)"));

    if(filename.isEmpty()) return;

    m_curDir = QDir(filename).path();

    QFile file(filename);

    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::critical(this, tr("Ошибка!"), tr("Невозможно сохранить файл!"));
        return;
    }

    RegListXmlSerializer ser;

    if(!ser.serialize(&file, m_regsListModel->regEntryList())){
        QMessageBox::critical(this, tr("Ошибка!"), tr("Невозможно записать данные в файл!"));
    }

    file.close();
}

void RegListEditorWin::on_actExport_triggered(bool checked)
{
    Q_UNUSED(checked);

    if(m_exportDlg->exec()){
        if(m_exportDlg->exportRegs()) doDlgExportRegs();
        if(m_exportDlg->exportData()) doDlgExportData();
        if(m_exportDlg->exportCO()) doDlgExportCo();
        if(m_exportDlg->exportEds()) doDlgExportEds();
        QMessageBox::information(this, tr("Экспорт"), tr("Завершено!"));
    }
}

void RegListEditorWin::on_actQuit_triggered(bool checked)
{
    Q_UNUSED(checked);

    qApp->quit();
}

void RegListEditorWin::on_actExpandTree_triggered(bool checked)
{
    Q_UNUSED(checked);

    ui->tvRegList->expandAll();
}

void RegListEditorWin::on_actCollapseTree_triggered(bool checked)
{
    Q_UNUSED(checked);

    ui->tvRegList->collapseAll();
}

void RegListEditorWin::on_actMoveUp_triggered(bool checked)
{
    Q_UNUSED(checked);

    QModelIndex index = ui->tvRegList->currentIndex();

    if(!index.isValid()) return;

    if(m_regsListModel->moveRow(index.parent(), index.row(), index.parent(), index.row() - 1)){
        ui->tvRegList->selectionModel()->setCurrentIndex(m_regsListModel->index(index.row() - 1, index.column(), index.parent()), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
}

void RegListEditorWin::on_actMoveDown_triggered(bool checked)
{
    Q_UNUSED(checked);

    QModelIndex index = ui->tvRegList->currentIndex();

    if(!index.isValid()) return;

    if(m_regsListModel->moveRow(index.parent(), index.row(), index.parent(), index.row() + 1)){
        ui->tvRegList->selectionModel()->setCurrentIndex(m_regsListModel->index(index.row() + 1, index.column(), index.parent()), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
}

void RegListEditorWin::on_actAddItem_triggered(bool checked)
{
    Q_UNUSED(checked);

    //qDebug() << "on_pbAdd_clicked";

    QModelIndex entry_index = m_regsListModel->entryModelIndexByModelIndex(ui->tvRegList->currentIndex());
    RegEntry* re = m_regsListModel->entryByModelIndex(entry_index);
    if(re != nullptr){
        m_regEntryDlg->setIndex(re->index() + 1);
        m_regEntryDlg->setObjectType(re->type());
    }

    m_regEntryDlg->setIndexEditable(true);
    m_regEntryDlg->setObjectTypeEditable(true);
    m_regEntryDlg->setName(QString("newObject"));
    m_regEntryDlg->setDescription(QString());

    if(m_regEntryDlg->exec()){

        if(m_regsListModel->hasEntryByRegIndex(m_regEntryDlg->index())){
            QMessageBox::critical(this, tr("Ошибка добавления."), tr("Элемент с данным индексом уже существует!"));
            return;
        }

        RegEntry* re = new RegEntry(m_regEntryDlg->index(), m_regEntryDlg->objectType());

        re->setName(m_regEntryDlg->name());
        re->setDescription(m_regEntryDlg->description());

        if(!m_regsListModel->addEntry(re)){
            qDebug() << "m_regsListModel->addEntry(...)";
            delete re;
            return;
        }

        QModelIndex entry_index = m_regsListModel->entryModelIndex(re);

        if(!entry_index.isValid()){
            qDebug() << "Invalid added entry model index";
            return;
        }

        if((re->type() == ObjectType::REC || re->type() == ObjectType::ARR) /* && add count var */){
            RegVar* count_var = new RegVar();

            count_var->setSubIndex(0);
            count_var->setDataType(DataType::U8);
            count_var->setMinValue(0);
            count_var->setMaxValue(254);
            if(re->type() == ObjectType::ARR){
                count_var->setDefaultValue(1);
            }else{
                count_var->setDefaultValue(0);
            }
            count_var->setName("count");
            count_var->setDescription("Number of sub-entries");
            count_var->setEFlags(RegEFlag::RL_HIDE | RegEFlag::CO_COUNT);

            if(!m_regsListModel->addSubObject(count_var, entry_index)){
                qDebug() << "m_regsListModel->addSubObject(count_var)";
                delete count_var;
            }

            ui->tvRegList->expand(entry_index);
        }

        if((re->type() == ObjectType::VAR || re->type() == ObjectType::ARR) /* && add count var */){
            RegVar* var = new RegVar();

            if(re->type() == ObjectType::VAR){
                var->setSubIndex(0);
                var->setName("value");
            }else if(re->type() == ObjectType::ARR){
                var->setSubIndex(1);
                var->setName("data");
                var->setCount(0);
            }

            if(!m_regsListModel->addSubObject(var, entry_index)){
                qDebug() << "m_regsListModel->addSubObject(var)";
                delete var;
            }

            ui->tvRegList->expand(entry_index);
        }
    }
}

void RegListEditorWin::on_actDuplicateItem_triggered(bool checked)
{
    Q_UNUSED(checked);

    //qDebug() << "on_pbAdd_clicked";

    QModelIndex entry_index = m_regsListModel->entryModelIndexByModelIndex(ui->tvRegList->currentIndex());
    if(!entry_index.isValid()) return;

    RegEntry* orig_re = m_regsListModel->entryByModelIndex(entry_index);
    if(orig_re == nullptr) return;

    m_regEntryDlg->setIndex(orig_re->index() + 1);
    m_regEntryDlg->setObjectType(orig_re->type());
    m_regEntryDlg->setName(orig_re->name());
    m_regEntryDlg->setDescription(orig_re->description());

    m_regEntryDlg->setIndexEditable(true);
    m_regEntryDlg->setObjectTypeEditable(true);

    if(m_regEntryDlg->exec()){

        if(m_regsListModel->hasEntryByRegIndex(m_regEntryDlg->index())){
            QMessageBox::critical(this, tr("Ошибка добавления."), tr("Элемент с данным индексом уже существует!"));
            return;
        }

        RegEntry* re = new RegEntry(*orig_re);

        re->setIndex(m_regEntryDlg->index());
        re->setType(m_regEntryDlg->objectType());
        re->setName(m_regEntryDlg->name());
        re->setDescription(m_regEntryDlg->description());

        if(!m_regsListModel->addEntry(re)){
            qDebug() << "m_regsListModel->addEntry(...)";
            delete re;
            return;
        }

        QModelIndex entry_index = m_regsListModel->entryModelIndex(re);

        if(!entry_index.isValid()){
            qDebug() << "Invalid added entry model index";
            return;
        }
        ui->tvRegList->expand(entry_index);
    }
}


void RegListEditorWin::on_actAddSubItem_triggered(bool checked)
{
    Q_UNUSED(checked);

    //qDebug() << "on_pbAddSub_clicked";

    QModelIndex entry_index = m_regsListModel->entryModelIndexByModelIndex(ui->tvRegList->currentIndex());
    RegEntry* re = m_regsListModel->entryByModelIndex(entry_index);
    if(re == nullptr) return;

    switch(re->type()){
    case ObjectType::VAR:
        if(re->count() != 0){
            QMessageBox::critical(this, tr("Ошибка"), tr("Внутри уже есть переменная!"));
            return;
        }
    case ObjectType::ARR:
    case ObjectType::REC:
        break;
    }

    m_regEntryDlg->setIndexEditable(true);
    m_regEntryDlg->setIndex(re->countAll());
    m_regEntryDlg->setObjectTypeEditable(false);
    m_regEntryDlg->setObjectType(ObjectType::VAR);

    RegVar* orig_rv = m_regsListModel->varByModelIndex(ui->tvRegList->currentIndex());

    if(orig_rv){
        m_regEntryDlg->setName(orig_rv->name());
        m_regEntryDlg->setDescription(orig_rv->description());
    }else{
        m_regEntryDlg->setName(QString("newSubObject"));
        m_regEntryDlg->setDescription(QString());
    }

    if(m_regEntryDlg->exec()){

        if(re->hasVarBySubIndex(m_regEntryDlg->index())){
            QMessageBox::critical(this, tr("Ошибка добавления."), tr("Элемент с данным индексом уже существует!"));
            return;
        }

        RegVar* rv = nullptr;
        if(orig_rv){
            rv = new RegVar(*orig_rv);
        }else{
            rv = new RegVar();
        }

        rv->setSubIndex(m_regEntryDlg->index());
        rv->setName(m_regEntryDlg->name());
        rv->setDescription(m_regEntryDlg->description());

        if(!m_regsListModel->addSubObject(rv, entry_index)){
            qDebug() << "m_regsListModel->addSubObject(...)";
            delete rv;
        }
    }
}

void RegListEditorWin::on_actDelItem_triggered(bool checked)
{
    Q_UNUSED(checked);

    //qDebug() << "on_pbDel_clicked";

    QModelIndex index = ui->tvRegList->currentIndex();

    if(!index.isValid()) return;

    ui->tvRegList->selectionModel()->clear();
    m_regsListModel->removeRow(index.row(), index.parent());
}

void RegListEditorWin::on_actDelAll_triggered(bool checked)
{
    Q_UNUSED(checked);

    m_regsListModel->setRegList(RegEntryList());
}

void RegListEditorWin::on_actDebugExec_triggered(bool checked)
{
    Q_UNUSED(checked);

    doDlgExportEds();
}

void RegListEditorWin::on_tvRegList_activated(const QModelIndex& index)
{
    //qDebug() << "on_tvRegList_activated";

    if(!index.isValid()) return;

    // entry.
    if(!index.parent().isValid()){

        RegEntry* re = m_regsListModel->entryByModelIndex(index);

        if(re == nullptr) return;

        m_regEntryDlg->setIndexEditable(true);
        m_regEntryDlg->setIndex(re->index());
        m_regEntryDlg->setObjectTypeEditable(true);
        m_regEntryDlg->setObjectType(re->type());
        m_regEntryDlg->setName(re->name());
        m_regEntryDlg->setDescription(re->description());

        if(m_regEntryDlg->exec()){

            re->setIndex(m_regEntryDlg->index());
            re->setType(m_regEntryDlg->objectType());
            re->setName(m_regEntryDlg->name());
            re->setDescription(m_regEntryDlg->description());

            m_regsListModel->entryAtIndexModified(index);
        }
    }
}

void RegListEditorWin::tvRegList_selection_changed(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    //qDebug() << "on_tvRegList_activated";

    if(selected.empty()){
    }else{
    }
}

void RegListEditorWin::appendFile(const QString& fileName)
{
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(this, tr("Ошибка!"), tr("Невозможно открыть файл: %1").arg(QFileInfo(fileName).fileName()));
        return;
    }

    RegListXmlSerializer ser;
    RegEntryList reglist;

    if(!ser.deserialize(&file, &reglist)){
        QMessageBox::critical(this, tr("Ошибка!"), tr("Невозможно прочитать данные из файла: %1").arg(QFileInfo(fileName).fileName()));
    }else{
        m_regsListModel->addRegList(reglist);
        if(!reglist.isEmpty()){
            QMessageBox::warning(this, tr("Предупреждение!"), tr("Часть данных не добавлена из-за совпадения индексов в файле: %1").arg(QFileInfo(fileName).fileName()));
            qDeleteAll(reglist);
        }
    }

    file.close();
}

void RegListEditorWin::doDlgExportRegs()
{
    auto reglist = m_regsListModel->regEntryList();
    auto entymapping = RegUtils::genRegDataEntryNameMapping(reglist);
    auto varmapping = RegUtils::genRegDataVarsNameMapping(reglist);

    RegListRegsExporter exporter;

    exporter.setListFileName(m_exportDlg->regListFileName())
            .setIdsFileName(m_exportDlg->regIdsFileName())
            .setDataFileName(m_exportDlg->regDataDeclFileName())
            .setUserCodeIds(m_exportDlg->userCodeRegIds())
            .setUserCodeList(m_exportDlg->userCodeRegList())
            .setDataName(m_exportDlg->dataName())
            .setSyntaxType(RegUtils::SyntaxType::camelCase)
            .setEntryNameMap(&entymapping)
            .setVarNameMap(&varmapping);

    if(!exporter.doExport(m_exportDlg->path(), m_regsListModel->regEntryList())){
        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка экспорта!"));
    }
}

void RegListEditorWin::doDlgExportData()
{
    auto reglist = m_regsListModel->regEntryList();
    auto entymapping = RegUtils::genRegDataEntryNameMapping(reglist);
    auto varmapping = RegUtils::genRegDataVarsNameMapping(reglist);

    RegListDataExporter exporter;

    exporter.setDeclFileName(m_exportDlg->regDataDeclFileName())
            .setImplFileName(m_exportDlg->regDataImplFileName())
            .setUserCodeDecl(m_exportDlg->userCodeDataDecl())
            .setUserCodeImpl(m_exportDlg->userCodeDataImpl())
            .setDataName(m_exportDlg->dataName())
            .setSyntaxType(RegUtils::SyntaxType::camelCase)
            .setEntryNameMap(&entymapping)
            .setVarNameMap(&varmapping);

    if(!exporter.doExport(m_exportDlg->path(), m_regsListModel->regEntryList())){
        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка экспорта!"));
    }
}

void RegListEditorWin::doDlgExportCo()
{
    auto reglist = m_regsListModel->regEntryList();
    auto entymapping = RegUtils::genRegDataEntryNameMapping(reglist);
    auto varmapping = RegUtils::genRegDataVarsNameMapping(reglist);

    RegListCoExporter exporter;

    exporter.setCOhFileName(m_exportDlg->cohFileName())
            .setCOcFileName(m_exportDlg->cocFileName())
            .setDataFileName(m_exportDlg->regDataDeclFileName())
            .setUserCodeCOh(m_exportDlg->userCodeCOh())
            .setUserCodeCOc(m_exportDlg->userCodeCOc())
            .setDataName(m_exportDlg->dataName())
            .setSyntaxType(RegUtils::SyntaxType::camelCase)
            .setEntryNameMap(&entymapping)
            .setVarNameMap(&varmapping);

    if(!exporter.doExport(m_exportDlg->path(), m_regsListModel->regEntryList())){
        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка экспорта!"));
    }
}

void RegListEditorWin::doDlgExportEds()
{
    auto reglist = m_regsListModel->regEntryList();
    auto entymapping = RegUtils::genRegDataEntryNameMapping(reglist);
    auto varmapping = RegUtils::genRegDataVarsNameMapping(reglist);

    RegListEdsExporter exporter;

    exporter.setEdsFileName(m_exportDlg->edsFileName())
            .setFileVersion(1, 0)
            .setFileAuthor("")
            .setOrderCode("")
            .setVendorName("TTS Corp")
            .setProductName("Super Drive")
            .setGranularity(8)
            .setDataName(m_exportDlg->dataName())
            .setSyntaxType(RegUtils::SyntaxType::camelCase)
            .setEntryNameMap(&entymapping)
            .setVarNameMap(&varmapping);

    if(!exporter.doExport(m_exportDlg->path(), m_regsListModel->regEntryList())){
        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка экспорта!"));
    }
}

