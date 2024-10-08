#include "reglisteditorwin.h"
#include "regvardelegate.h"
#include "ui_reglisteditorwin.h"
#include "regentrydlg.h"
#include "reglistmodel.h"
#include "regvarmodel.h"
#include "regentry.h"
#include "regobject.h"
#include "regvar.h"
#include "regarray.h"
#include "regrecord.h"
#include <QMessageBox>
#include <QItemSelectionModel>
#include <QDebug>


RegListEditorWin::RegListEditorWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RegListEditorWin)
{
    ui->setupUi(this);
    m_regEntryDlg = new RegEntryDlg();

    m_regVarModel = new RegVarModel();
    m_regVarDelegate = new RegVarDelegate();

    m_regsListModel = new RegListModel();
    ui->tvRegList->setModel(m_regsListModel);

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
    delete m_regEntryDlg;
    delete m_regsListModel;
    delete m_regVarModel;
    delete m_regVarDelegate;
}

void RegListEditorWin::on_pbAdd_clicked()
{
    qDebug() << "on_pbAdd_clicked";

    m_regEntryDlg->setIndexEditable(true);
    m_regEntryDlg->setName(QString("newObject"));
    m_regEntryDlg->setDescription(QString());

    if(m_regEntryDlg->exec()){

        if(m_regsListModel->hasEntryByIndex(m_regEntryDlg->index())){
            QMessageBox::critical(this, tr("Ошибка добавления."), tr("Элемент с данным индексом уже существует!"));
            return;
        }

        RegEntry* re = new RegEntry(m_regEntryDlg->index(), m_regEntryDlg->objectType());

        re->setName(m_regEntryDlg->name());
        re->setDescription(m_regEntryDlg->description());

        if(!m_regsListModel->addEntry(re)){
            qDebug() << "m_regsListModel->addEntry(...)";
            delete re;
        }
    }
}

void RegListEditorWin::on_pbAddSub_clicked()
{
    qDebug() << "on_pbAddSub_clicked";

    QModelIndex index = ui->tvRegList->currentIndex();

    //RegEntry* re =
}

void RegListEditorWin::on_pbDel_clicked()
{
    qDebug() << "on_pbDel_clicked";

    QModelIndex index = ui->tvRegList->currentIndex();

    if(index.isValid()){
        m_regsListModel->removeRow(index.row(), index.parent());
    }
}

void RegListEditorWin::on_tvRegList_activated(const QModelIndex& index)
{
    qDebug() << "on_tvRegList_activated";

    if(!index.isValid()) return;

    // entry.
    if(!index.parent().isValid()){

        RegEntry* re = m_regsListModel->entryByIndex(index);

        if(re == nullptr) return;

        m_regEntryDlg->setIndexEditable(false);
        m_regEntryDlg->setIndex(re->index());
        m_regEntryDlg->setObjectType(re->objectType());
        m_regEntryDlg->setName(re->name());
        m_regEntryDlg->setDescription(re->description());

        if(m_regEntryDlg->exec()){

            re->setObjectType(m_regEntryDlg->objectType());
            re->setName(m_regEntryDlg->name());
            re->setDescription(m_regEntryDlg->description());

            m_regsListModel->entryAtIndexModified(index);

            updateRegViewModel(index);
        }
    }
}

void RegListEditorWin::tvRegList_selection_changed(const QItemSelection &selected, const QItemSelection &deselected)
{
    qDebug() << "on_tvRegList_activated";

    if(selected.empty()){
        updateRegViewModel(QModelIndex());
    }else{
        updateRegViewModel(selected.indexes().first());
    }
}

void RegListEditorWin::updateRegViewModel(const QModelIndex& index)
{
    qDebug() << "updateRegViewModel";

    if(!index.isValid()){
        ui->tvRegInfo->setModel(nullptr);
        return;
    }

    RegObject* ro = m_regsListModel->objectByIndex(index);

    if(ro == nullptr){
        ui->tvRegInfo->setModel(nullptr);
        return;
    }

    qDebug() << RegTypes::typeStr(ro->type());

    switch(ro->type()){
    default:
        break;
    case ObjectType::VAR:
        m_regVarModel->setRegVar(static_cast<RegVar*>(ro));
        ui->tvRegInfo->setModel(m_regVarModel);
        ui->tvRegInfo->setItemDelegate(m_regVarDelegate);
        break;
    case ObjectType::ARR:
        ui->tvRegInfo->setItemDelegate(nullptr);
        ui->tvRegInfo->setModel(nullptr);
        break;
    case ObjectType::REC:
        ui->tvRegInfo->setItemDelegate(nullptr);
        ui->tvRegInfo->setModel(nullptr);
        break;
    }
}

