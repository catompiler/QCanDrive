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

    m_regEntries = new RegEntryList();
    m_regEntries->append(new RegEntry(0x1000, ObjectType::VAR));

    m_regsListModel = new RegListModel(m_regEntries);
    ui->tvRegList->setModel(m_regsListModel);
}

RegListEditorWin::~RegListEditorWin()
{
    delete ui;
    delete m_regsListModel;

    for(auto entry: qAsConst(*m_regEntries)){
        delete entry;
    }
    delete m_regEntries;
}

void RegListEditorWin::on_pbAdd_clicked()
{
    qDebug() << "on_pbAdd_clicked";

    m_regsListModel->layoutAboutToBeChanged();
    m_regEntries->append(new RegEntry(0x2000, ObjectType::VAR));
    m_regsListModel->layoutChanged();
}

