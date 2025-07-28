#include "regsviewwgt.h"
#include "regselectdlg.h"
#include "regsviewmodel.h"
#include "regentry.h"
#include "regobject.h"
#include "regvar.h"
#include "regtypes.h"
#include "regutils.h"
#include "settings.h"
#include <QMessageBox>
#include <QApplication>
#include <QItemSelectionModel>
#include <QIODevice>
#include <QByteArray>
#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QScopedPointer>
#include <QDebug>


#define COL_WIDTH_INDEX 75
#define COL_WIDTH_NAME 150
#define COL_WIDTH_TYPE 75
#define COL_WIDTH_COUNT 75
#define COL_WIDTH_MEM_ADDR 150
#define COL_WIDTH_MIN_VAL 75
#define COL_WIDTH_MAX_VAL 75
#define COL_WIDTH_DEF_VAL 100
#define COL_WIDTH_BASE 100
#define COL_WIDTH_FLAGS 100
#define COL_WIDTH_EXTFLAGS 150
#define COL_WIDTH_DESCR 100
//#define COL_WIDTH_ 50


static const int col_width[] = {
    COL_WIDTH_INDEX,
    COL_WIDTH_NAME,
    COL_WIDTH_TYPE,
    COL_WIDTH_COUNT,
    COL_WIDTH_MEM_ADDR,
    COL_WIDTH_MIN_VAL,
    COL_WIDTH_MAX_VAL,
    COL_WIDTH_DEF_VAL,
    COL_WIDTH_BASE,
    COL_WIDTH_FLAGS,
    COL_WIDTH_EXTFLAGS,
    COL_WIDTH_DESCR
};
static const int col_width_len = ((sizeof(col_width))/(sizeof(col_width[0])));


RegsViewWgt::RegsViewWgt(QWidget *parent)
    : QTreeView(parent)
{
    m_regsViewModel = new RegsViewModel();
    getTreeView()->setModel(m_regsViewModel);
    //getTreeView()->setItemDelegate(nullptr);

    /*getTreeView()->setSelectionMode(QAbstractItemView::SingleSelection);
    QItemSelectionModel* sel_model = getTreeView()->selectionModel();
    if(sel_model == nullptr){
        sel_model = new QItemSelectionModel();
        sel_model->setParent(getTreeView());
        getTreeView()->setSelectionModel(sel_model);
    }
    connect(sel_model, &QItemSelectionModel::selectionChanged, this, &RegsViewWgt::m_tvRegList_selection_changed);
    */
    connect(this, &RegsViewWgt::activated, this, &RegsViewWgt::m_tvRegList_activated);

    /*for(int i = 0; i < col_width_len; i ++){
        getTreeView()->setColumnWidth(i, col_width[i]);
    }*/

    restoreSettings();
}

RegsViewWgt::~RegsViewWgt()
{
    storeSettings();

    delete m_regsViewModel;
}

const RegListModel* RegsViewWgt::regListModel() const
{
    return qobject_cast<RegListModel*>(m_regsViewModel->sourceModel());
}

RegListModel* RegsViewWgt::regListModel()
{
    return qobject_cast<RegListModel*>(m_regsViewModel->sourceModel());
}

void RegsViewWgt::setRegListModel(RegListModel* newRegListModel)
{
    m_regsViewModel->setSourceModel(newRegListModel);
}

void RegsViewWgt::expandTree()
{
    getTreeView()->expandAll();
}

void RegsViewWgt::collapseTree()
{
    getTreeView()->collapseAll();
}

void RegsViewWgt::m_tvRegList_activated(const QModelIndex& index)
{
    //qDebug() << "on_tvRegList_activated";

    if(!index.isValid()) return;

    // entry.
    if(!index.parent().isValid()){

        //RegEntry* re = m_regsViewModel->entryByModelIndex(index);
        //if(re == nullptr) return;

    }
}

void RegsViewWgt::m_tvRegList_selection_changed(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    //qDebug() << "on_tvRegList_activated";

    if(selected.empty()){
    }else{
    }
}


void RegsViewWgt::restoreSettings()
{
}

void RegsViewWgt::storeSettings()
{
}

