#include "regsviewmodel.h"
#include "reglistmodel.h"
#include "regobject.h"
#include <QDebug>


typedef struct _Col_Mapping {
    const char* name;
    int sourceCol;
} col_mapping_t;


static const col_mapping_t cols[] = {
    {"Индекс", RegListModel::COL_INDEX},
    {"Имя", RegListModel::COL_NAME},
    {"Описание", RegListModel::COL_DESCR},
    {"Тип", RegListModel::COL_TYPE},
    {"Минимум", RegListModel::COL_MIN_VAL},
    {"Максимум", RegListModel::COL_MAX_VAL},
    {"По-умолчанию", RegListModel::COL_DEF_VAL},
    {"Значение", -1},
};


static const auto col_count = sizeof(cols) / sizeof(cols[0]);



RegsViewModel::RegsViewModel(QObject* parent)
    :QAbstractProxyModel(parent)
{
}

RegsViewModel::~RegsViewModel()
{
}


QModelIndex RegsViewModel::index(int row, int column, const QModelIndex& parent) const
{
    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QModelIndex();

    return model->index(row, column, parent);
}

QModelIndex RegsViewModel::parent(const QModelIndex& child) const
{
    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QModelIndex();

    return model->parent(child);
}

int RegsViewModel::rowCount(const QModelIndex& parent) const
{
    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return 0;

    return model->rowCount(parent);
}

int RegsViewModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return col_count;
}

QModelIndex RegsViewModel::mapToSource(const QModelIndex& proxyIndex) const
{
    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QModelIndex();

    int row = proxyIndex.row();
    int col = proxyIndex.column();
    QModelIndex parent = proxyIndex.parent();

    return model->index(row, col, parent);
}

QModelIndex RegsViewModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QModelIndex();

    int row = sourceIndex.row();
    int col = sourceIndex.column();
    QModelIndex parent = sourceIndex.parent();

    return model->index(row, col, parent);
}

Qt::ItemFlags RegsViewModel::flags(const QModelIndex& index) const
{
    //qDebug() << "RegsViewModel::flags(" << index << ")";

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


/*bool RegsViewModel::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const
{
    Q_UNUSED(source_parent);

    switch(source_column){
    default:
        break;
    case RegListModel::COL_INDEX:
    case RegListModel::COL_NAME:
    case RegListModel::COL_DEF_VAL:
    case RegListModel::COL_DESCR:
        return  true;
    }

    return false;
}*/

