#include "reglisteditorwin.h"
#include "regdelegate.h"
#include "ui_reglisteditorwin.h"
#include "regentrydlg.h"
#include "regselectdlg.h"
#include "flagseditdlg.h"
#include "reglistmodel.h"
#include "regentry.h"
#include "regobject.h"
#include "regvar.h"
#include <QMessageBox>
#include <QApplication>
#include <QItemSelectionModel>
#include <QDebug>


RegListEditorWin::RegListEditorWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RegListEditorWin)
{
    ui->setupUi(this);
    m_regEntryDlg = new RegEntryDlg();
    m_regSelectDlg = new RegSelectDlg();
    m_flagsEditDlg = new FlagsEditDlg();

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
}

RegListEditorWin::~RegListEditorWin()
{
    delete ui;
    delete m_flagsEditDlg;
    delete m_regSelectDlg;
    delete m_regEntryDlg;
    delete m_regsListModel;
    delete m_regListDelegate;
}

void RegListEditorWin::on_actQuit_triggered(bool checked)
{
    Q_UNUSED(checked);

    qApp->quit();
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
            count_var->setDataType(DataType::UNSIGNED8);
            count_var->setMinValue(0);
            count_var->setMaxValue(254);
            count_var->setDefaultValue(0);
            count_var->setName("count");
            count_var->setDescription("Number of sub-entries");

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

void RegListEditorWin::on_actAddSubItem_triggered(bool checked)
{
    Q_UNUSED(checked);

    //qDebug() << "on_pbAddSub_clicked";

    QModelIndex entry_index = m_regsListModel->entryModelIndexByModelIndex(ui->tvRegList->currentIndex());
    RegEntry* re = m_regsListModel->entryByModelIndex(entry_index);
    if(re == nullptr) return;

    m_regEntryDlg->setIndexEditable(true);
    m_regEntryDlg->setIndex(re->count());
    m_regEntryDlg->setObjectTypeEditable(false);
    m_regEntryDlg->setObjectType(ObjectType::VAR);
    m_regEntryDlg->setName(QString("newSubObject"));
    m_regEntryDlg->setDescription(QString());

    if(m_regEntryDlg->exec()){

        if(re->hasVarBySubIndex(m_regEntryDlg->index())){
            QMessageBox::critical(this, tr("Ошибка добавления."), tr("Элемент с данным индексом уже существует!"));
            return;
        }

        RegVar* rv = new RegVar();

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

void RegListEditorWin::on_actDebugExec_triggered(bool checked)
{
    Q_UNUSED(checked);

    m_flagsEditDlg->setFlagsNames(RegTypes::flagsNames().mid(1));
    m_flagsEditDlg->exec();
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

