#include "regsviewwgt.h"
#include "regdelegate.h"
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
#include "reglistxml2serializer.h"
#include "reglistregsexporter.h"
#include "reglistdataexporter.h"
#include "reglistcoexporter.h"
#include "reglistedsexporter.h"
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
#include <QDebug>


#define COL_WIDTH_INDEX 75
#define COL_WIDTH_NAME 150
#define COL_WIDTH_TYPE 75
#define COL_WIDTH_COUNT 75
#define COL_WIDTH_MEM_ADDR 150
#define COL_WIDTH_MIN_VAL 75
#define COL_WIDTH_MAX_VAL 75
#define COL_WIDTH_DEF_VAL 100
#define COL_WIDTH_BASE 100
#define COL_WIDTH_FLAGS 100
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


RegsViewWgt::RegsViewWgt(QWidget *parent)
    : QTreeView(parent)
{
    m_regEntryDlg = new RegEntryDlg();
    m_regSelectDlg = new RegSelectDlg();
    m_flagsEditDlg = new FlagsEditDlg();
    m_exportDlg = new ExportDlg();

    m_regListDelegate = new RegDelegate();
    m_regListDelegate->setRegSelectDialog(m_regSelectDlg);
    m_regListDelegate->setFlagsEditDialog(m_flagsEditDlg);

    m_regsViewModel = new RegListModel();
    getTreeView()->setModel(m_regsViewModel);
    getTreeView()->setItemDelegate(m_regListDelegate);

    m_regSelectDlg->setRegListModel(m_regsViewModel);

    getTreeView()->setSelectionMode(QAbstractItemView::SingleSelection);
    QItemSelectionModel* sel_model = getTreeView()->selectionModel();
    if(sel_model == nullptr){
        sel_model = new QItemSelectionModel();
        sel_model->setParent(getTreeView());
        getTreeView()->setSelectionModel(sel_model);
    }
    connect(sel_model, &QItemSelectionModel::selectionChanged, this, &RegsViewWgt::m_tvRegList_selection_changed);
    connect(this, &RegsViewWgt::activated, this, &RegsViewWgt::m_tvRegList_activated);

    for(int i = 0; i < col_width_len; i ++){
        getTreeView()->setColumnWidth(i, col_width[i]);
    }

    restoreSettings();
}

RegsViewWgt::~RegsViewWgt()
{
    storeSettings();

    delete m_exportDlg;
    delete m_flagsEditDlg;
    delete m_regSelectDlg;
    delete m_regEntryDlg;
    delete m_regsViewModel;
    delete m_regListDelegate;
}

void RegsViewWgt::openRegList()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Открыть файлы"), m_curDir,
                                                          tr("Файлы списка регистров (*.regxml2);;Файлы списка регистров - старая версия (*.regxml)"));

    if(filenames.isEmpty()) return;

    m_curDir = QDir(filenames.front()).path();

    m_regsViewModel->setRegList(RegEntryList());

    for(auto& filename: filenames){
        appendFile(filename);
    }
}

void RegsViewWgt::appendRegList()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Добавить файлы"), m_curDir,
                                                          tr("Файлы списка регистров (*.regxml2);;Файлы списка регистров - старая версия (*.regxml)"));

    if(filenames.isEmpty()) return;

    m_curDir = QDir(filenames.front()).path();

    for(auto& filename: filenames){
        appendFile(filename);
    }
}

void RegsViewWgt::saveAsRegList()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Сохранить файл"), m_curDir,
                                                    tr("Файлы списка регистров (*.regxml2);;Файлы списка регистров - старая версия (*.regxml)"));

    if(filename.isEmpty()) return;

    m_curDir = QDir(filename).path();

    QFile file(filename);

    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::critical(this, tr("Ошибка!"), tr("Невозможно сохранить файл!"));
        return;
    }

    QScopedPointer<RegListSerializer> ser;

    if(filename.endsWith(".regxml2")){
        ser.reset(new RegListXml2Serializer());
    }else{
        ser.reset(new RegListXmlSerializer());
    }

    if(!ser->serialize(&file, m_regsViewModel->regEntryList())){
        QMessageBox::critical(this, tr("Ошибка!"), tr("Невозможно записать данные в файл!"));
    }

    file.close();
}

void RegsViewWgt::expandTree()
{
    getTreeView()->expandAll();
}

void RegsViewWgt::collapseTree()
{
    getTreeView()->collapseAll();
}

void RegsViewWgt::moveUp()
{
    QModelIndex index = getTreeView()->currentIndex();

    if(!index.isValid()) return;

    if(m_regsViewModel->moveRow(index.parent(), index.row(), index.parent(), index.row() - 1)){
        getTreeView()->selectionModel()->setCurrentIndex(m_regsViewModel->index(index.row() - 1, index.column(), index.parent()), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
}

void RegsViewWgt::moveDown()
{
    QModelIndex index = getTreeView()->currentIndex();

    if(!index.isValid()) return;

    if(m_regsViewModel->moveRow(index.parent(), index.row(), index.parent(), index.row() + 1)){
        getTreeView()->selectionModel()->setCurrentIndex(m_regsViewModel->index(index.row() + 1, index.column(), index.parent()), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
}

void RegsViewWgt::addItem()
{
    //qDebug() << "on_pbAdd_clicked";

    QModelIndex entry_index = m_regsViewModel->entryModelIndexByModelIndex(getTreeView()->currentIndex());
    RegEntry* re = m_regsViewModel->entryByModelIndex(entry_index);
    if(re != nullptr){
        m_regEntryDlg->setIndex(re->index() + 1);
        m_regEntryDlg->setObjectType(re->type());
    }

    m_regEntryDlg->setIndexEditable(true);
    m_regEntryDlg->setObjectTypeEditable(true);
    m_regEntryDlg->setName(QString("newObject"));
    m_regEntryDlg->setDescription(QString());

    if(m_regEntryDlg->exec()){

        if(m_regsViewModel->hasEntryByRegIndex(m_regEntryDlg->index())){
            QMessageBox::critical(this, tr("Ошибка добавления."), tr("Элемент с данным индексом уже существует!"));
            return;
        }

        RegEntry* re = new RegEntry(m_regEntryDlg->index(), m_regEntryDlg->objectType());

        re->setName(m_regEntryDlg->name());
        re->setDescription(m_regEntryDlg->description());

        if(!m_regsViewModel->addEntry(re)){
            qDebug() << "m_regsListModel->addEntry(...)";
            delete re;
            return;
        }

        QModelIndex entry_index = m_regsViewModel->entryModelIndex(re);

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

            if(!m_regsViewModel->addSubObject(count_var, entry_index)){
                qDebug() << "m_regsListModel->addSubObject(count_var)";
                delete count_var;
            }

            getTreeView()->expand(entry_index);
        }

        if((re->type() == ObjectType::VAR || re->type() == ObjectType::ARR) /* && add count var */){
            RegVar* var = new RegVar();

            if(re->type() == ObjectType::VAR){
                var->setSubIndex(0);
                var->setName("value");
            }else if(re->type() == ObjectType::ARR){
                var->setSubIndex(1);
                var->setName("data");
                //var->setCount(0);
            }

            if(!m_regsViewModel->addSubObject(var, entry_index)){
                qDebug() << "m_regsListModel->addSubObject(var)";
                delete var;
            }

            getTreeView()->expand(entry_index);
        }
    }
}

void RegsViewWgt::duplicateItem()
{
    //qDebug() << "on_pbAdd_clicked";

    QModelIndex entry_index = m_regsViewModel->entryModelIndexByModelIndex(getTreeView()->currentIndex());
    if(!entry_index.isValid()) return;

    RegEntry* orig_re = m_regsViewModel->entryByModelIndex(entry_index);
    if(orig_re == nullptr) return;

    m_regEntryDlg->setIndex(orig_re->index() + 1);
    m_regEntryDlg->setObjectType(orig_re->type());
    m_regEntryDlg->setName(orig_re->name());
    m_regEntryDlg->setDescription(orig_re->description());

    m_regEntryDlg->setIndexEditable(true);
    m_regEntryDlg->setObjectTypeEditable(true);

    if(m_regEntryDlg->exec()){

        if(m_regsViewModel->hasEntryByRegIndex(m_regEntryDlg->index())){
            QMessageBox::critical(this, tr("Ошибка добавления."), tr("Элемент с данным индексом уже существует!"));
            return;
        }

        RegEntry* re = new RegEntry(*orig_re);

        re->setIndex(m_regEntryDlg->index());
        re->setType(m_regEntryDlg->objectType());
        re->setName(m_regEntryDlg->name());
        re->setDescription(m_regEntryDlg->description());

        if(!m_regsViewModel->addEntry(re)){
            qDebug() << "m_regsListModel->addEntry(...)";
            delete re;
            return;
        }

        QModelIndex entry_index = m_regsViewModel->entryModelIndex(re);

        if(!entry_index.isValid()){
            qDebug() << "Invalid added entry model index";
            return;
        }
        getTreeView()->expand(entry_index);
    }
}


void RegsViewWgt::addSubItem()
{
    //qDebug() << "on_pbAddSub_clicked";

    QModelIndex entry_index = m_regsViewModel->entryModelIndexByModelIndex(getTreeView()->currentIndex());
    RegEntry* re = m_regsViewModel->entryByModelIndex(entry_index);
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
    reg_index_t newIndex = re->countAll();
    if(newIndex != 0 && re->type() == ObjectType::ARR){
        RegVar* rv = re->lastVar();
        newIndex = rv->subIndex() + qMax(1U, rv->count());
    }
    m_regEntryDlg->setIndex(newIndex);
    m_regEntryDlg->setObjectTypeEditable(false);
    m_regEntryDlg->setObjectType(ObjectType::VAR);

    RegVar* orig_rv = m_regsViewModel->varByModelIndex(getTreeView()->currentIndex());

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

        if(re->type() == ObjectType::ARR && re->count() > 1){
            RegVar* v = re->at(re->count() - 1);
            rv->setDataType(v->dataType());
        }

        rv->setSubIndex(m_regEntryDlg->index());
        rv->setName(m_regEntryDlg->name());
        rv->setDescription(m_regEntryDlg->description());

        if(!m_regsViewModel->addSubObject(rv, entry_index)){
            qDebug() << "m_regsListModel->addSubObject(...)";
            delete rv;
        }
    }
}

void RegsViewWgt::delItem()
{
    //qDebug() << "on_pbDel_clicked";

    QModelIndex index = getTreeView()->currentIndex();

    if(!index.isValid()) return;

    getTreeView()->selectionModel()->clear();
    m_regsViewModel->removeRow(index.row(), index.parent());
}

void RegsViewWgt::delAll()
{
    m_regsViewModel->setRegList(RegEntryList());
}

void RegsViewWgt::m_tvRegList_activated(const QModelIndex& index)
{
    //qDebug() << "on_tvRegList_activated";

    if(!index.isValid()) return;

    // entry.
    if(!index.parent().isValid()){

        RegEntry* re = m_regsViewModel->entryByModelIndex(index);

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

            m_regsViewModel->entryAtIndexModified(index);
        }
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

void RegsViewWgt::appendFile(const QString& fileName)
{
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(this, tr("Ошибка!"), tr("Невозможно открыть файл: %1").arg(QFileInfo(fileName).fileName()));
        return;
    }

    QScopedPointer<RegListSerializer> ser;

    if(fileName.endsWith(".regxml2")){
        ser.reset(new RegListXml2Serializer());
    }else{
        ser.reset(new RegListXmlSerializer());
    }

    RegEntryList reglist;

    if(!ser->deserialize(&file, &reglist)){
        QMessageBox::critical(this, tr("Ошибка!"), tr("Невозможно прочитать данные из файла: %1").arg(QFileInfo(fileName).fileName()));
    }else{
        m_regsViewModel->addRegList(reglist);
        if(!reglist.isEmpty()){
            QMessageBox::warning(this, tr("Предупреждение!"), tr("Часть данных не добавлена из-за совпадения индексов в файле: %1").arg(QFileInfo(fileName).fileName()));
            qDeleteAll(reglist);
        }
    }

    file.close();
}


void RegsViewWgt::restoreSettings()
{
}

void RegsViewWgt::storeSettings()
{
}

