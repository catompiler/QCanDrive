#include "reglisteditorwin.h"
#include "ui_reglisteditorwin.h"
#include "regentrydlg.h"
#include "reglistmodel.h"
#include "regentry.h"
#include <QMessageBox>
#include <QDebug>


RegListEditorWin::RegListEditorWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RegListEditorWin)
{
    ui->setupUi(this);
    m_regEntryDlg = new RegEntryDlg();

    m_regsListModel = new RegListModel();
    ui->tvRegList->setModel(m_regsListModel);
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

void RegListEditorWin::on_pbDel_clicked()
{
    QModelIndex index = ui->tvRegList->currentIndex();

    if(index.isValid()){
        m_regsListModel->removeRow(index.row(), index.parent());
    }
}

