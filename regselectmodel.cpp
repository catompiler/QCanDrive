#include "regselectmodel.h"
#include "reglistmodel.h"
#include <QDebug>



RegSelectModel::RegSelectModel(QObject* parent)
    :QAbstractProxyModel(parent)
{

}

RegSelectModel::~RegSelectModel()
{

}


QModelIndex RegSelectModel::index(int row, int column, const QModelIndex& parent) const
{
    //qDebug() << "RegSelectModel::index(" << row << ", " << column << parent << ")";

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QModelIndex();

    QModelIndex source_index = model->index(row, column, parent);

    return createIndex(row, column, source_index.internalPointer());
}

QModelIndex RegSelectModel::parent(const QModelIndex& child) const
{
    //qDebug() << "RegSelectModel::parent(" << child << ")";

    if(!child.isValid()) return QModelIndex();

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QModelIndex();

    QModelIndex source_parent = model->parent(child);

    return createIndex(source_parent.row(), source_parent.column(), source_parent.internalPointer());
}

int RegSelectModel::rowCount(const QModelIndex& parent) const
{    
    //qDebug() << "RegSelectModel::rowCount(" << parent << ")";

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return 0;

    return model->rowCount(parent);
}

int RegSelectModel::columnCount(const QModelIndex& parent) const
{
    //qDebug() << "RegSelectModel::columnCount(" << parent << ")";

    Q_UNUSED(parent);

    /*QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return 0;*/

    return ColumnsCount;
}

QModelIndex RegSelectModel::mapToSource(const QModelIndex& proxyIndex) const
{
    //qDebug() << "RegSelectModel::mapToSource(" << proxyIndex << ")";

    if(!proxyIndex.isValid()) return QModelIndex();

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QModelIndex();

    QModelIndex parent = proxyIndex.parent();

    switch(proxyIndex.column()){
    default:
        break;
    case COL_INDEX:
        return model->index(proxyIndex.row(), RegListModel::COL_INDEX, parent);
    case COL_NAME:
        return model->index(proxyIndex.row(), RegListModel::COL_NAME, parent);
    case COL_DESCR:
        return model->index(proxyIndex.row(), RegListModel::COL_DESCR, parent);
    }

    return QModelIndex();
}

QModelIndex RegSelectModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    //qDebug() << "RegSelectModel::mapFromSource(" << sourceIndex << ")";

    if(!sourceIndex.isValid()) return QModelIndex();

    QModelIndex parent = sourceIndex.parent();

    switch(sourceIndex.column()){
    default:
        break;
    case RegListModel::COL_INDEX:
        return index(sourceIndex.row(), COL_INDEX, parent);
    case RegListModel::COL_NAME:
        return index(sourceIndex.row(), COL_NAME, parent);
    case RegListModel::COL_DESCR:
        return index(sourceIndex.row(), COL_DESCR, parent);
    }

    return QModelIndex();
}

QVariant RegSelectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    //qDebug() << "RegSelectModel::headerData(" << section << ", " << orientation << static_cast<Qt::ItemDataRole>(role) << ")";

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QVariant();

    if(orientation == Qt::Horizontal){
        switch(section){
        default:
            break;
        case COL_INDEX:
            section = RegListModel::COL_INDEX;
            break;
        case COL_NAME:
            section = RegListModel::COL_NAME;
            break;
        case COL_DESCR:
            section = RegListModel::COL_DESCR;
            break;
        }
    }

    return model->headerData(section, orientation, role);
}


QVariant RegSelectModel::data(const QModelIndex& index, int role) const
{
    //qDebug() << "RegSelectModel::data(" << index << ", " << static_cast<Qt::ItemDataRole>(role) << ")";

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QVariant();

    QModelIndex source_index = mapToSource(index);

    return model->data(source_index, role);
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

    RegObject* ro = reglist_model->objectByIndex(source_index);
    if(ro == nullptr) return flags;

    if(ro->type() != ObjectType::VAR) flags &= ~Qt::ItemIsSelectable;

    return flags;
}
