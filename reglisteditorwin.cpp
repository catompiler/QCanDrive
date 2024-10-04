#include "reglisteditorwin.h"
#include "ui_reglisteditorwin.h"
#include "reglistmodel.h"
#include "regentry.h"
#include <QDebug>


RegListEditorWin::RegListEditorWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RegListEditorWin)
{
    ui->setupUi(this);

    m_reg_entries = new QList<RegEntry*>();
    m_reg_entries->append(new RegEntry(0x1000, ObjectType::VAR));

    m_regsListModel = new RegListModel(m_reg_entries);
    ui->tvRegList->setModel(m_regsListModel);
}

RegListEditorWin::~RegListEditorWin()
{
    delete ui;
    delete m_regsListModel;

    for(auto entry: qAsConst(*m_reg_entries)){
        delete entry;
    }
    delete m_reg_entries;
}

void RegListEditorWin::on_pbAdd_clicked()
{
    m_reg_entries->append(new RegEntry(0x2000, ObjectType::VAR));

    qDebug() << "on_pbAdd_clicked"
             << m_regsListModel->insertRow(m_regsListModel->rowCount(), QModelIndex());
}

