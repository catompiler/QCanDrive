#include "reglisteditorwin.h"
#include "regdelegate.h"
#include "ui_reglisteditorwin.h"
#include "regentrydlg.h"
#include "regselectdlg.h"
#include "reglistmodel.h"
//#include "regvarmodel.h"
#include "regentry.h"
#include "regobject.h"
#include "regvar.h"
#include "regarray.h"
#include "regrecord.h"
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

    //m_regVarModel = new RegVarModel();
    m_regListDelegate = new RegDelegate();

    m_regsListModel = new RegListModel();
    ui->tvRegList->setModel(m_regsListModel);
    ui->tvRegList->setItemDelegate(m_regListDelegate);

    m_regSelectDlg->setRegListModel(m_regsListModel);

    ui->tvRegList->setSelectionMode(QAbstractItemView::SingleSelection);
    QItemSelectionModel* sel_model = ui->tvRegList->selectionModel();
    if(sel_model == nullptr){
        sel_model = new QItemSelectionModel();
        ui->tvRegList->setSelectionModel(sel_model);
    }
    connect(sel_model, &QItemSelectionModel::selectionChanged, this, &RegListEditorWin::tvRegList_selection_changed);
}

RegListEditorWin::~RegListEditorWin()
{
    delete ui;
    delete m_regSelectDlg;
    delete m_regEntryDlg;
    delete m_regsListModel;
    //delete m_regVarModel;
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

    m_regEntryDlg->setIndexEditable(true);
    m_regEntryDlg->setObjectTypeEditable(true);
    m_regEntryDlg->setName(QString("newObject"));
    m_regEntryDlg->setDescription(QString());

    if(m_regEntryDlg->exec()){

        if(m_regsListModel->hasEntryByIndex(m_regEntryDlg->index())){
            QMessageBox::critical(this, tr("Ошибка добавления."), tr("Элемент с данным индексом уже существует!"));
            return;
        }

        RegEntry* re = new RegEntry(m_regEntryDlg->index(), m_regEntryDlg->objectType());

        RegObject* ro = re->object();

        if(ro == nullptr){
            delete re;
            qDebug() << "RegObject is null";
            return;
        }

        ro->setName(m_regEntryDlg->name());
        ro->setDescription(m_regEntryDlg->description());

        if(!m_regsListModel->addEntry(re)){
            qDebug() << "m_regsListModel->addEntry(...)";
            delete re;
        }
    }
}

void RegListEditorWin::on_actAddSubItem_triggered(bool checked)
{
    Q_UNUSED(checked);

    //qDebug() << "on_pbAddSub_clicked";

    QModelIndex entry_index = m_regsListModel->entryIndex(ui->tvRegList->currentIndex());
    RegEntry* re = m_regsListModel->entryByIndex(entry_index);

    if(re == nullptr) return;
    if(re->objectType() != ObjectType::REC && re->objectType() != ObjectType::ARR) return;

    m_regEntryDlg->setIndexEditable(false);
    m_regEntryDlg->setIndex(re->index());
    m_regEntryDlg->setObjectTypeEditable(false);
    m_regEntryDlg->setObjectType(ObjectType::VAR);
    m_regEntryDlg->setName(QString("newSubObject"));
    m_regEntryDlg->setDescription(QString());

    if(m_regEntryDlg->exec()){

        RegObject* ro = RegObject::newByType(m_regEntryDlg->objectType());

        ro->setName(m_regEntryDlg->name());
        ro->setDescription(m_regEntryDlg->description());

        if(!m_regsListModel->addSubObject(ro, entry_index)){
            qDebug() << "m_regsListModel->addEntry(...)";
            delete ro;
        }
    }
}

void RegListEditorWin::on_actDelItem_triggered(bool checked)
{
    Q_UNUSED(checked);

    //qDebug() << "on_pbDel_clicked";

    QModelIndex index = ui->tvRegList->currentIndex();

    if(!index.isValid()) return;

    m_regsListModel->removeRow(index.row(), index.parent());
}

void RegListEditorWin::on_actDebugExec_triggered(bool checked)
{
    Q_UNUSED(checked);

    m_regSelectDlg->exec();
}

void RegListEditorWin::on_tvRegList_activated(const QModelIndex& index)
{
    //qDebug() << "on_tvRegList_activated";

    if(!index.isValid()) return;

    // entry.
    if(!index.parent().isValid()){

        RegEntry* re = m_regsListModel->entryByIndex(index);

        if(re == nullptr) return;

        RegObject* ro = re->object();

        if(ro == nullptr) return;

        m_regEntryDlg->setIndexEditable(false);
        m_regEntryDlg->setIndex(re->index());
        m_regEntryDlg->setObjectTypeEditable(false);
        m_regEntryDlg->setObjectType(re->objectType());
        m_regEntryDlg->setName(ro->name());
        m_regEntryDlg->setDescription(ro->description());

        if(m_regEntryDlg->exec()){

            re->setObjectType(m_regEntryDlg->objectType());
            ro->setName(m_regEntryDlg->name());
            ro->setDescription(m_regEntryDlg->description());

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

