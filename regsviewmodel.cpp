#include "regsviewmodel.h"
#include "reglistmodel.h"
#include "regentry.h"
#include "regvar.h"
#include "regutils.h"
#include "covaluetypes.h"
#include <QAbstractItemModel>
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
    m_slcon = nullptr;
    m_cache = new ValuesCache();
    m_queue = new UpdateQueue();

    connect(this, &QAbstractItemModel::modelReset, this, &RegsViewModel::m_modelReseted);
}

RegsViewModel::~RegsViewModel()
{
    delete m_queue;
    delete m_cache;
}

SLCanOpenNode* RegsViewModel::getSLCanOpenNode()
{
    return m_slcon;
}

const SLCanOpenNode* RegsViewModel::getSLCanOpenNode() const
{
    return m_slcon;
}

void RegsViewModel::setSLCanOpenNode(SLCanOpenNode* slcon)
{
    m_slcon = slcon;
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
    {
        //qDebug() << "col" << col;
        if(col < 0 || col >= static_cast<int>(col_count)) return QModelIndex();
        const auto& cm = cols[col];
        if(cm.sourceCol == -1) return QModelIndex();
        col = cm.sourceCol;
        //qDebug() << "->" << col;
    }

    QModelIndex parent = mapToSource(proxyIndex.parent());

    int rows = model->rowCount(parent);
    int row = proxyIndex.row();
    {
        int f_row = -1; // finded row.
        int s_row = 0; // accepted source row.
        int i_row = 0; // iterator.
        for(;i_row < rows; i_row ++){
            if(filterAcceptsRow(i_row, parent)){
                if(s_row == row){
                    f_row = i_row;
                    break;
                }
                s_row ++;
            }
        }
        row = f_row;
//        int s_row = 0;
//        int i_row = 0;
//        do{
//            if(filterAcceptsRow(i_row, parent)){
//                if(s_row == row){
//                    break;
//                }
//                s_row ++;
//            }
//            i_row ++;
//        }while(s_row <= row);
//        row = i_row;
    }
    if(row == -1) return QModelIndex();

    QModelIndex idx = model->index(row, col, parent);

    //if(!idx.isValid()) qDebug() << "source model index invalid!";
    //else qDebug() << idx;

    //qDebug() << "RegsViewModel::mapToSource" << proxyIndex << idx;

    return idx;
}

QModelIndex RegsViewModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    //qDebug() << "RegsViewModel::mapFromSource" << sourceIndex;

    if(!sourceIndex.isValid()) return QModelIndex();

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QModelIndex();

    int col = sourceIndex.column();
    {
        //qDebug() << "col" << col;
        auto it = std::find_if(&cols[0], &cols[col_count], [col](const col_mapping_t& cm){
            return cm.sourceCol == col;
        });
        if(it == &cols[col_count]) return QModelIndex();
        col = std::distance(&cols[0], it);
        //qDebug() << "->" << col;
    }

    QModelIndex parent = sourceIndex.parent();

    int row = sourceIndex.row();
    if(!filterAcceptsRow(row, parent)) return QModelIndex();
    {
        int s_row = -1;
        int i_row = 0;
        for(; i_row <= row; i_row ++){
            if(filterAcceptsRow(i_row, parent)){
                s_row ++;
            }
        }
        row = s_row;
    }
    if(row == -1) return QModelIndex();

    QModelIndex idx = model->index(row, col, parent);
    //if(!idx.isValid()) qDebug() << "source model index invalid!";
    //else qDebug() << idx;

    //qDebug() << "RegsViewModel::mapFromSource" << sourceIndex << idx;

    return idx;
}

Qt::ItemFlags RegsViewModel::flags(const QModelIndex& index) const
{
    //qDebug() << "RegsViewModel::flags" << index;

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

        RegVar* rv = reglist_model->varByModelIndex(mapToSource(this->index(index.row(), 0, index.parent())));
        if(rv == nullptr) return flags;

        if(!(rv->flags() & RegFlag::READONLY)){
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
    //qDebug() << "RegsViewModel::data" << index << "," << role;

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return QVariant();

    QModelIndex source_index = mapToSource(index);

    QVariant res;

    if(source_index.isValid()){
        res = model->data(source_index, role);
    }else{
        RegListModel* reglist_model = qobject_cast<RegListModel*>(model);
        if(reglist_model != nullptr){

            RegVar* rv = reglist_model->varByModelIndex(mapToSource(this->index(index.row(), 0, index.parent())));
            if(rv != nullptr){

                RegEntry* re = rv->parent();

                switch(role){
                default:
                    res = QVariant();
                    break;
                case Qt::ToolTipRole:
                case Qt::EditRole:
                case Qt::DisplayRole:{
                    if(re == nullptr){
                        res = QVariant();
                        break;
                    }

                    reg_fullindex_t val_fi = RegUtils::makeFullIndex(re->index(), rv->subIndex());
                    //reg_fullindex_t base_fi = RegUtils::makeFullIndex(rv->baseIndex(), rv->baseSubIndex());
                    auto it = m_cache->find(val_fi);
                    if(it != m_cache->end()){
                        res = COValue::valueFrom<qreal>(reinterpret_cast<const void*>(&it.value().value), rv->dataType());
                    }else{
                        switch(role){
                        default:
                            res = QVariant();
                            break;
                        case Qt::DisplayRole:
                            updateValue(re->index(), rv->subIndex(), false);
                        __attribute__ ((fallthrough));
                        case Qt::ToolTipRole:
                            res = tr("updating...");
                            break;
                        }
                    }
                }break;
                }
            }
        }
    }

    //qDebug() << index << res;

    return res;
}


bool RegsViewModel::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const
{
    Q_UNUSED(source_column);
    Q_UNUSED(source_parent);

    return true;
}


bool RegsViewModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    //Q_UNUSED(source_row);
    //Q_UNUSED(source_parent);

    //qDebug() << "RegsViewModel::filterAcceptsRow" << source_row << source_parent;

    auto defRet = [&source_row, &source_parent, this]() -> bool {
        return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    };

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return defRet();

    RegListModel* reglist_model = qobject_cast<RegListModel*>(model);
    if(reglist_model == nullptr) return defRet();

    RegEntry* re = reglist_model->entryByModelIndex(source_parent);
    // Reg entry.
    if(re == nullptr){
        return true;
    }

    RegVar* rv = re->at(source_row);
    if(rv == nullptr) return false;

    if(rv->eflags() & RegEFlag::CO_COUNT) return false;

    return defRet();
}

void RegsViewModel::m_modelReseted()
{
    //qDebug() << "RegsViewModel::m_modelReseted()";

    m_cache->clear();
    m_queue->clear();
}

void RegsViewModel::updateValue(uint16_t index, uint16_t subIndex, bool isWrite, uint32_t value) const
{
    UpdateCmd cmd;
    cmd.index = index;
    cmd.subindex = subIndex;
    cmd.isWrite = isWrite;
    cmd.data = value;

    m_queue->enqueue(cmd);

    // TODO: run processing cmd queue.
}
