#include "regselectdlg.h"
#include "ui_regselectdlg.h"
#include "regselectmodel.h"
#include "reglistmodel.h"
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QPushButton>
#include <QShowEvent>
#include <QDebug>



RegSelectDlg::RegSelectDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegSelectDlg)
{
    ui->setupUi(this);

    m_regSelMdl = new RegSelectModel();
    ui->tvRegList->setModel(m_regSelMdl);

    QItemSelectionModel* selMdl = ui->tvRegList->selectionModel();
    if(selMdl == nullptr){
        selMdl = new QItemSelectionModel();
        ui->tvRegList->setSelectionModel(selMdl);
    }
    connect(selMdl, &QItemSelectionModel::selectionChanged, this, &RegSelectDlg::regList_selectionChanged);
    ui->tvRegList->setSelectionMode(QAbstractItemView::SingleSelection);
}

RegSelectDlg::~RegSelectDlg()
{
    delete ui;
    delete m_regSelMdl;
}

RegListModel* RegSelectDlg::regListModel() const
{
    return qobject_cast<RegListModel*>(m_regSelMdl->sourceModel());
}

void RegSelectDlg::setRegListModel(RegListModel* newRegListModel)
{
    /*QAbstractItemModel* oldModel = m_regSelMdl->sourceModel();
    if(oldModel){
    }
    if(newRegListModel){
    }*/
    m_regSelMdl->setSourceModel(newRegListModel);
}

void RegSelectDlg::regList_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    //qDebug() << "RegSelectDlg::regList_selectionChanged()";// << selected << ", " << deselected << ")";
    //qDebug() << ui->tvRegList->selectionModel()->selectedIndexes().isEmpty();

    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    QPushButton* pb = ui->bbOkCancel->button(QDialogButtonBox::Ok);
    if(pb){
        pb->setEnabled(!ui->tvRegList->selectionModel()->selectedIndexes().isEmpty());
    }
}

void RegSelectDlg::showEvent(QShowEvent* event)
{
    QPushButton* pb = ui->bbOkCancel->button(QDialogButtonBox::Ok);
    if(pb){
        pb->setEnabled(!ui->tvRegList->selectionModel()->selectedIndexes().isEmpty());
    }
    event->ignore();
}
