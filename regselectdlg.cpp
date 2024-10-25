#include "regselectdlg.h"
#include "ui_regselectdlg.h"
#include "regselectmodel.h"
#include "reglistmodel.h"
#include "regentry.h"
#include "regvar.h"
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

bool RegSelectDlg::hasSelectedReg() const
{
    return !ui->tvRegList->selectionModel()->selectedIndexes().isEmpty();
}

QPair<uint, uint> RegSelectDlg::selectedRegIndex() const
{
    auto model = qobject_cast<RegListModel*>(m_regSelMdl->sourceModel());
    if(model == nullptr) return qMakePair(0U, 0U);

    QModelIndexList sel_index_list = ui->tvRegList->selectionModel()->selectedIndexes();
    if(sel_index_list.isEmpty()) return qMakePair(0U, 0U);

    QModelIndex index = m_regSelMdl->mapToSource(sel_index_list.first());

    RegVar* rv = model->varByModelIndex(index);
    if(rv == nullptr) return qMakePair(0U, 0U);

    RegEntry* re = rv->parent();
    if(re == nullptr) return qMakePair(0U, 0U);

    return qMakePair(static_cast<uint>(re->index()), static_cast<uint>(rv->subIndex()));
}

void RegSelectDlg::selectReg(uint index, uint subIndex)
{
    auto model = qobject_cast<RegListModel*>(m_regSelMdl->sourceModel());
    if(model == nullptr) return;

    QModelIndex src_index = model->objectModelIndexByRegIndex(index, subIndex);
    QModelIndex sel_index = m_regSelMdl->mapFromSource(src_index);

    ui->tvRegList->selectionModel()->setCurrentIndex(sel_index, QItemSelectionModel::ClearAndSelect);
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
