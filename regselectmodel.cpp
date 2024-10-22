#include "regselectmodel.h"
#include "reglistmodel.h"
#include "regobject.h"
#include <QDebug>



RegSelectModel::RegSelectModel(QObject* parent)
    :QSortFilterProxyModel(parent)
{
}

RegSelectModel::~RegSelectModel()
{
}


Qt::ItemFlags RegSelectModel::flags(const QModelIndex& index) const
{
    //qDebug() << "RegSelectModel::flags(" << index << ")";

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return Qt::NoItemFlags;

    QModelIndex source_index = mapToSource(index);
    Qt::ItemFlags flags = model->flags(source_index);

    flags &= ~Qt::ItemIsEditable;

    RegListModel* reglist_model = qobject_cast<RegListModel*>(model);
    if(reglist_model == nullptr) return flags;

    RegObject* ro = reglist_model->objectByModelIndex(source_index);
    if(ro == nullptr) return flags;

    if(ro->parent() == nullptr) flags &= ~Qt::ItemIsSelectable;

    return flags;
}


bool RegSelectModel::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const
{
    Q_UNUSED(source_parent);

    switch(source_column){
    default:
        break;
    case RegListModel::COL_INDEX:
    case RegListModel::COL_NAME:
    case RegListModel::COL_DESCR:
        return  true;
    }

    return false;
}
