#include "regselectdlg.h"
#include "ui_regselectdlg.h"
#include "regselectmodel.h"
#include "reglistmodel.h"
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QPushButton>
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
    //connect(selMdl, &QItemSelectionModel::selectionChanged, this, &RegSelectDlg::regList_selectionChanged);
    //connect(selMdl, &QItemSelectionModel::currentChanged, this, &RegSelectDlg::regList_currentChanged);
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
        disconnect(oldModel, &QAbstractItemModel::dataChanged, m_regSelMdl, &QAbstractItemModel::dataChanged);
        disconnect(oldModel, &QAbstractItemModel::layoutAboutToBeChanged, m_regSelMdl, &QAbstractItemModel::layoutAboutToBeChanged);
        disconnect(oldModel, &QAbstractItemModel::layoutChanged, m_regSelMdl, &QAbstractItemModel::layoutChanged);
        disconnect(oldModel, &QAbstractItemModel::modelAboutToBeReset, m_regSelMdl, &QAbstractItemModel::modelAboutToBeReset);
        disconnect(oldModel, &QAbstractItemModel::modelReset, m_regSelMdl, &QAbstractItemModel::modelReset);
        disconnect(oldModel, &QAbstractItemModel::rowsAboutToBeInserted, m_regSelMdl, &QAbstractItemModel::rowsAboutToBeInserted);
        disconnect(oldModel, &QAbstractItemModel::rowsInserted, m_regSelMdl, &QAbstractItemModel::rowsInserted);
        disconnect(oldModel, &QAbstractItemModel::rowsAboutToBeRemoved, m_regSelMdl, &QAbstractItemModel::rowsAboutToBeRemoved);
        disconnect(oldModel, &QAbstractItemModel::rowsRemoved, m_regSelMdl, &QAbstractItemModel::rowsRemoved);
    }
    if(newRegListModel){
        connect(newRegListModel, &QAbstractItemModel::dataChanged, m_regSelMdl, &QAbstractItemModel::dataChanged);
        connect(newRegListModel, &QAbstractItemModel::layoutAboutToBeChanged, m_regSelMdl, &QAbstractItemModel::layoutAboutToBeChanged);
        connect(newRegListModel, &QAbstractItemModel::layoutChanged, m_regSelMdl, &QAbstractItemModel::layoutChanged);
        connect(newRegListModel, &QAbstractItemModel::modelAboutToBeReset, m_regSelMdl, &QAbstractItemModel::modelAboutToBeReset);
        connect(newRegListModel, &QAbstractItemModel::modelReset, m_regSelMdl, &QAbstractItemModel::modelReset);
        connect(newRegListModel, &QAbstractItemModel::rowsAboutToBeInserted, m_regSelMdl, &QAbstractItemModel::rowsAboutToBeInserted);
        connect(newRegListModel, &QAbstractItemModel::rowsInserted, m_regSelMdl, &QAbstractItemModel::rowsInserted);
        connect(newRegListModel, &QAbstractItemModel::rowsAboutToBeRemoved, m_regSelMdl, &QAbstractItemModel::rowsAboutToBeRemoved);
        connect(newRegListModel, &QAbstractItemModel::rowsRemoved, m_regSelMdl, &QAbstractItemModel::rowsRemoved);
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

void RegSelectDlg::regList_currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    //qDebug() << "RegSelectDlg::regList_currentChanged()";// << current << ", " << previous << ")";
    //qDebug() << ui->tvRegList->selectionModel()->selectedIndexes().isEmpty();

    Q_UNUSED(current);
    Q_UNUSED(previous);

    QPushButton* pb = ui->bbOkCancel->button(QDialogButtonBox::Ok);
    if(pb){
        pb->setEnabled(!ui->tvRegList->selectionModel()->selectedIndexes().isEmpty());
    }
}
