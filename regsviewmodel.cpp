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
    :QSortFilterProxyModel(parent)
{
}

RegsViewModel::~RegsViewModel()
{
}

//qDebug() << "";

//QModelIndex RegsViewModel::index(int row, int column, const QModelIndex& parent) const
//{
//    qDebug() << "RegsViewModel::index" << row << "," << column << "," << parent;

//    QAbstractItemModel* model = sourceModel();
//    if(model == nullptr) return QModelIndex();

//    QModelIndex idx = model->index(row, column, parent);
//    if(!idx.isValid()) qDebug() << "source model index invalid!";
//    else qDebug() << idx;

//    return idx;
//}

//QModelIndex RegsViewModel::parent(const QModelIndex& child) const
//{
//    qDebug() << "RegsViewModel::parent" << child;

//    QAbstractItemModel* model = sourceModel();
//    if(model == nullptr) return QModelIndex();

//    QModelIndex p = model->parent(child);

//    qDebug() << "child" << child;
//    qDebug() << "parent" << p;

//    return p;
//}

int RegsViewModel::rowCount(const QModelIndex& parent) const
{
    //qDebug() << "RegsViewModel::rowCount" << parent;

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return 0;

    int rows = model->rowCount(mapToSource(parent));

    //qDebug() << "rows:" << rows;

    return rows;
}

int RegsViewModel::columnCount(const QModelIndex& parent) const
{
    //qDebug() << "RegsViewModel::columnCount";

    Q_UNUSED(parent);

    //qDebug() << "columns:" << col_count;

    return col_count;
}

QModelIndex RegsViewModel::mapToSource(const QModelIndex& proxyIndex) const
{
    //qDebug() << "RegsViewModel::mapToSource" << proxyIndex;

    if(!proxyIndex.isValid()) return QModelIndex();

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QModelIndex();

    int col = proxyIndex.column();

    //qDebug() << "col" << col;

    if(col < 0 || col >= static_cast<int>(col_count)) return QModelIndex();
    const auto& cm = cols[col];
    if(cm.sourceCol == -1) return QModelIndex();
    col = cm.sourceCol;

    //qDebug() << "->" << col;

    int row = proxyIndex.row();
    QModelIndex parent = proxyIndex.parent();

    QModelIndex idx = model->index(row, col, parent);
    //if(!idx.isValid()) qDebug() << "source model index invalid!";
    //else qDebug() << idx;

    return idx;
}

QModelIndex RegsViewModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    //qDebug() << "RegsViewModel::mapFromSource" << sourceIndex;

    if(!sourceIndex.isValid()) return QModelIndex();

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QModelIndex();

    int col = sourceIndex.column();

    //qDebug() << "col" << col;

    auto it = std::find_if(&cols[0], &cols[col_count], [col](const col_mapping_t& cm){
        return cm.sourceCol == col;
    });
    if(it == &cols[col_count]) return QModelIndex();
    col = std::distance(&cols[0], it);

    //qDebug() << "->" << col;

    int row = sourceIndex.row();
    QModelIndex parent = sourceIndex.parent();

    QModelIndex idx = model->index(row, col, parent);
    //if(!idx.isValid()) qDebug() << "source model index invalid!";
    //else qDebug() << idx;

    return idx;
}

Qt::ItemFlags RegsViewModel::flags(const QModelIndex& index) const
{
    qDebug() << "RegsViewModel::flags" << index;

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return Qt::NoItemFlags;

    Qt::ItemFlags flags = Qt::NoItemFlags;

    QModelIndex source_index = mapToSource(index);
//    if(index.column() == COL_VALUE){
//        qDebug() << "COL_VALUE" << source_index;
//    }
    if(source_index.isValid()){
        flags = model->flags(source_index);
        flags &= ~Qt::ItemIsEditable;
    }else{
        flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    if(index.column() == COL_VALUE){
        RegListModel* reglist_model = qobject_cast<RegListModel*>(model);
        if(reglist_model == nullptr) return flags;

        RegObject* ro = reglist_model->objectByModelIndex(mapToSource(this->index(index.row(), 0, index.parent())));
        if(ro == nullptr) return flags;

        if(ro->parent() != nullptr){
            flags |= Qt::ItemIsEditable;
        }
        //qDebug() << "COL_VALUE is editable";
    }

    return flags;
}

QVariant RegsViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal) return QVariant();
    if(role != Qt::DisplayRole) return QVariant();
    if(section < 0 || section >= static_cast<int>(col_count)) return QVariant();

    return tr(cols[section].name);
}


QVariant RegsViewModel::data(const QModelIndex& index, int role) const
{
    qDebug() << "RegsViewModel::data" << index << "," << role;

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QVariant();

    QModelIndex source_index = mapToSource(index);

    QVariant res;

    if(source_index.isValid()){
        res = model->data(source_index, role);
    }else{
        RegListModel* reglist_model = qobject_cast<RegListModel*>(model);
        if(reglist_model != nullptr){

            RegObject* ro = reglist_model->objectByModelIndex(mapToSource(this->index(index.row(), 0, index.parent())));
            if(ro != nullptr && ro->parent() != nullptr){

                switch(role){
                default:
                    res = QVariant();
                    break;
                case Qt::DisplayRole:
                    res = tr("test");
                    break;
                case Qt::EditRole:
                    res = tr("edit");
                    break;
                }
            }
        }
    }

    qDebug() << index << res;

    return res;
}


//bool RegsViewModel::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const
//{
//    return true;
//}
