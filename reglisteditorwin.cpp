#include "reglisteditorwin.h"
#include "ui_reglisteditorwin.h"
#include "regentrydlg.h"
#include "reglistmodel.h"
#include "regentry.h"
#include <QDebug>


RegListEditorWin::RegListEditorWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RegListEditorWin)
{
    ui->setupUi(this);
    m_regEntryDlg = new RegEntryDlg();

    m_regEntries = new RegEntryList();
    //m_regEntries->append(new RegEntry(0x1000, ObjectType::VAR));

    m_regsListModel = new RegListModel(m_regEntries);
    ui->tvRegList->setModel(m_regsListModel);
}

RegListEditorWin::~RegListEditorWin()
{
    delete ui;
    delete m_regEntryDlg;
    delete m_regsListModel;

    for(auto entry: qAsConst(*m_regEntries)){
        delete entry;
    }
    delete m_regEntries;
}

void RegListEditorWin::on_pbAdd_clicked()
{
    qDebug() << "on_pbAdd_clicked";

    m_regEntryDlg->setName(QString("newObject"));
    m_regEntryDlg->setDescription(QString());

    if(m_regEntryDlg->exec()){

        RegEntry* re = new RegEntry(m_regEntryDlg->index(), m_regEntryDlg->objectType());

        re->setName(m_regEntryDlg->name());
        re->setDescription(m_regEntryDlg->description());

        m_regsListModel->layoutAboutToBeChanged();
        m_regEntries->append(re);
        m_regsListModel->layoutChanged();
    }
}

