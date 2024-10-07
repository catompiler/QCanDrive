#include "reglisteditorwin.h"
#include "ui_reglisteditorwin.h"
#include "regentrydlg.h"
#include "reglistmodel.h"
#include "regentry.h"
#include <QMessageBox>
#include <QItemSelectionModel>
#include <QDebug>


RegListEditorWin::RegListEditorWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RegListEditorWin)
{
    ui->setupUi(this);
    m_regEntryDlg = new RegEntryDlg();

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
}

void RegListEditorWin::on_pbAdd_clicked()
{
    qDebug() << "on_pbAdd_clicked";

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

void RegListEditorWin::tvRegList_selection_changed(const QItemSelection &selected, const QItemSelection &deselected)
{
    qDebug() << "on_tvRegList_activated";

    if(selected.empty()){
        return;
    }

    QModelIndex index = selected.indexes().first();

    if(!index.isValid()){
        return;
    }

    RegObject* ro = m_regsListModel->objectByIndex(index);

    if(ro == nullptr){
        return;
    }

    qDebug() << RegTypes::typeStr(ro->type());
}

