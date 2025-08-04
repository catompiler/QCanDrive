#include "regsviewmodel.h"
#include "reglistmodel.h"
#include "regentry.h"
#include "regvar.h"
#include "regutils.h"
#include "sdovalue.h"
#include "slcanopennode.h"
//#include <algorithm>
#include <QAbstractItemModel>
#include <QDebug>


#define MODEL_DEBUG 0
#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#define INDEX_DEBUG 0
#define PARENT_DEBUG 0
#define MAP_DEBUG 0
#define ROW_COUNT_DEBUG 0
#define FLAGS_DEBUG 0
#define FILTER_DEBUG 0
#define DATA_DEBUG 0
#endif


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
    m_nodeId = 0;
    m_slcon = nullptr;
    m_sdoval = new SDOValue();
    m_cache = new ValuesCache();
    m_queue = new UpdateQueue();

    connect(m_sdoval, &SDOValue::finished, this, &RegsViewModel::m_valueUpdateFinished);
}

RegsViewModel::~RegsViewModel()
{
    delete m_queue;
    delete m_cache;
    delete m_sdoval;
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
    m_sdoval->setSLCanOpenNode(slcon);
}

void RegsViewModel::refreshRegs()
{
    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return;

    m_cache->clear();

    // https://stackoverflow.com/questions/24001613/qt-make-view-to-update-visible-data
    emit dataChanged( QModelIndex(), QModelIndex() );
}

QModelIndex RegsViewModel::buddy(const QModelIndex& index) const
{
    if(index.column() == COL_VALUE) return index;

    return QAbstractProxyModel::buddy(index);
}

//qDebug() << "";

QModelIndex RegsViewModel::index(int row, int column, const QModelIndex& parent) const
{
#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(INDEX_DEBUG) && INDEX_DEBUG == 1
    qDebug() << "RegsViewModel::index" << row << "," << column << "," << parent;
#endif
#endif

    QAbstractItemModel* source_model = sourceModel();
    if(source_model == nullptr) return QModelIndex();

    RegListModel* reglist_model = qobject_cast<RegListModel*>(source_model);
    if(reglist_model == nullptr) return QModelIndex();

    QModelIndex source_parent = mapToSource(parent);
    if(!source_parent.isValid()) return createIndex(row, column, static_cast<quintptr>(-1));

    RegEntry* re = reglist_model->entryByModelIndex(source_parent);
    if(re == nullptr) return QModelIndex();

    RegVar* rv = re->at(row);
    if(rv == nullptr) return QModelIndex();

    QModelIndex res_index = createIndex(row, column, static_cast<quintptr>(RegUtils::makeFullIndex(re->index(), rv->subIndex())));

#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(INDEX_DEBUG) && INDEX_DEBUG == 1
    qDebug() << "RegsViewModel::index" << "return" << res_index;
#endif
#endif

    return res_index;
}

QModelIndex RegsViewModel::parent(const QModelIndex& child) const
{
#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(PARENT_DEBUG) && PARENT_DEBUG == 1
    qDebug() << "RegsViewModel::parent" << child;
#endif
#endif

    QAbstractItemModel* source_model = sourceModel();
    if(source_model == nullptr) return QModelIndex();

    RegListModel* reglist_model = qobject_cast<RegListModel*>(source_model);
    if(reglist_model == nullptr) return QModelIndex();

    reg_fullindex_t fi = static_cast<reg_fullindex_t>(child.internalId());
    if(fi == static_cast<reg_fullindex_t>(-1)) return QModelIndex();

    auto index_subindex = RegUtils::getIndexSubIndex(fi);

    RegVar* rv = reglist_model->varByRegIndex(index_subindex.first, index_subindex.second);
    if(rv == nullptr) return QModelIndex();

    RegEntry* re = rv->parent();
    if(re == nullptr) return QModelIndex();

    QModelIndex parent_index = mapFromSource(reglist_model->entryModelIndex(re));

#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(PARENT_DEBUG) && PARENT_DEBUG == 1
    qDebug() << "RegsViewModel::parent" << "return" << parent_index;
#endif
#endif

    return parent_index;
}

int RegsViewModel::rowCount(const QModelIndex& parent) const
{
#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(ROW_COUNT_DEBUG) && ROW_COUNT_DEBUG == 1
    qDebug() << "RegsViewModel::rowCount" << parent;
#endif
#endif

    QAbstractItemModel* source_model = sourceModel();
    if(source_model == nullptr) return 0;

    QModelIndex source_parent = mapToSource(parent);

    int rows = source_model->rowCount(source_parent);
    int s_row = 0;

    for(int i_row = 0; i_row < rows; i_row ++){
        if(filterAcceptsRow(i_row, source_parent)){
            s_row ++;
        }
    }

#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(ROW_COUNT_DEBUG) && ROW_COUNT_DEBUG == 1
    qDebug() << "RegsViewModel::rowCount" << "return" << s_row;
#endif
#endif

    return s_row;
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
#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(MAP_DEBUG) && MAP_DEBUG == 1
    qDebug() << "RegsViewModel::mapToSource" << proxyIndex;
#endif
#endif

    if(!proxyIndex.isValid()) return QModelIndex();

    QAbstractItemModel* source_model = sourceModel();
    if(source_model == nullptr) return QModelIndex();

    int col = proxyIndex.column();
    {
        //qDebug() << "col" << col;
        if(col < 0 || col >= static_cast<int>(col_count)) return QModelIndex();
        const auto& cm = cols[col];
        if(cm.sourceCol == -1) return QModelIndex();
        col = cm.sourceCol;
        //qDebug() << "->" << col;
    }

    QModelIndex source_parent = mapToSource(proxyIndex.parent());

    int rows = source_model->rowCount(source_parent);
    int row = proxyIndex.row();
    {
        int f_row = -1; // finded row.
        int s_row = 0; // accepted source row.
        int i_row = 0; // iterator.
        for(;i_row < rows; i_row ++){
            if(filterAcceptsRow(i_row, source_parent)){
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
//            if(filterAcceptsRow(i_row, source_parent)){
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

    QModelIndex idx = source_model->index(row, col, source_parent);

    //if(!idx.isValid()) qDebug() << "source model index invalid!";
    //else qDebug() << idx;

#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(MAP_DEBUG) && MAP_DEBUG == 1
    qDebug() << "RegsViewModel::mapToSource" << "return" << idx;
#endif
#endif

    return idx;
}

QModelIndex RegsViewModel::mapFromSource(const QModelIndex& sourceIndex) const
{
#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(MAP_DEBUG) && MAP_DEBUG == 1
    qDebug() << "RegsViewModel::mapFromSource" << sourceIndex;
#endif
#endif

    if(!sourceIndex.isValid()) return QModelIndex();

    QAbstractItemModel* source_model = sourceModel();
    if(source_model == nullptr) return QModelIndex();

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

    QModelIndex source_parent = sourceIndex.parent();

    int row = sourceIndex.row();
    if(!filterAcceptsRow(row, source_parent)) return QModelIndex();
    {
        int s_row = -1;
        int i_row = 0;
        for(; i_row <= row; i_row ++){
            if(filterAcceptsRow(i_row, source_parent)){
                s_row ++;
            }
        }
        row = s_row;
    }
    if(row == -1) return QModelIndex();

    QModelIndex parent = mapFromSource(source_parent);
    QModelIndex idx = index(row, col, parent);
    //if(!idx.isValid()) qDebug() << "source model index invalid!";
    //else qDebug() << idx;

#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(MAP_DEBUG) && MAP_DEBUG == 1
    qDebug() << "RegsViewModel::mapFromSource" << "return" << idx;
#endif
#endif

    return idx;
}

Qt::ItemFlags RegsViewModel::flags(const QModelIndex& index) const
{
#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(FLAGS_DEBUG) && FLAGS_DEBUG == 1
    qDebug() << "RegsViewModel::flags" << index;
#endif
#endif

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return Qt::NoItemFlags;

    Qt::ItemFlags flags = Qt::NoItemFlags;

//    if(index.column() == COL_VALUE){
//        qDebug() << "flags COL_VALUE";
//    }

    QModelIndex source_index = mapToSource(index);

    if(source_index.isValid()){
        flags = model->flags(source_index);
        flags &= ~Qt::ItemIsEditable;
    }else{
        flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    if(index.column() == COL_VALUE){
        RegListModel* reglist_model = qobject_cast<RegListModel*>(model);
        if(reglist_model == nullptr) return flags;

        QModelIndex idx = this->index(index.row(), 0, index.parent());
        if(!idx.isValid()) return flags;

        QModelIndex src_idx = mapToSource(idx);
        if(!src_idx.isValid()) return flags;

        RegVar* rv = reglist_model->varByModelIndex(src_idx);
        if(rv == nullptr) return flags;

        if(!(rv->flags() & RegFlag::READONLY)){
            flags |= Qt::ItemIsEditable;
        }
        //qDebug() << "COL_VALUE is editable";
    }

#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(FLAGS_DEBUG) && FLAGS_DEBUG == 1
    qDebug() << "RegsViewModel::flags" << "return" << flags;
#endif
#endif

    return flags;
}

QVariant RegsViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal) return QVariant();
    if(role != Qt::DisplayRole) return QVariant();
    if(section < 0 || section >= static_cast<int>(col_count)) return QVariant();

    QVariant res = tr(cols[section].name);

    return res;
}


QVariant RegsViewModel::data(const QModelIndex& index, int role) const
{
#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(DATA_DEBUG) && DATA_DEBUG == 1
    qDebug() << "RegsViewModel::data" << index << "," << role;
#endif
#endif

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

                if(re != nullptr){
                    switch(role){
                    default:
                        res = QVariant();
                        break;
                    case Qt::ToolTipRole:
                    case Qt::EditRole:{

                        reg_fullindex_t val_fi = RegUtils::makeFullIndex(re->index(), rv->subIndex());
                        //reg_fullindex_t base_fi = RegUtils::makeFullIndex(rv->baseIndex(), rv->baseSubIndex());
                        auto it = m_cache->find(val_fi);
                        if(it != m_cache->end()){
                            res = it.value();
                        }
                    }break;
                    case Qt::DisplayRole:{

                        reg_fullindex_t val_fi = RegUtils::makeFullIndex(re->index(), rv->subIndex());
                        //reg_fullindex_t base_fi = RegUtils::makeFullIndex(rv->baseIndex(), rv->baseSubIndex());
                        auto it = m_cache->find(val_fi);
                        if(it != m_cache->end()){
                            res = it.value();
                        }else if(m_slcon->isConnected()){
                            int upd_res = updateValue(re->index(), rv->subIndex(), false, rv->dataType());
                            if(upd_res == 1){
                                res = tr("updating...");
                            }else if(upd_res == -1){
                                res = tr("error");
                            }else{
                                res = tr("");
                            }
                        }
                    }break;
                    }
                }
            }
        }
    }

#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(DATA_DEBUG) && DATA_DEBUG == 1
    qDebug() << "RegsViewModel::data" << "return" << res;
#endif
#endif

    return res;
}

bool RegsViewModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(DATA_DEBUG) && DATA_DEBUG == 1
    qDebug() << "RegsViewModel::setData" << index << "," << value << "," << role;
#endif
#endif

    if(role != Qt::EditRole) return false;
    if(!index.isValid()) return false;
    if(index.column() != COL_VALUE) return false;

    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return false;

    RegListModel* reglist_model = qobject_cast<RegListModel*>(model);
    if(reglist_model == nullptr) return false;

    RegVar* rv = reglist_model->varByModelIndex(mapToSource(this->index(index.row(), 0, index.parent())));
    if(rv == nullptr) return false;

    if(!RegTypes::isNumeric(rv->dataType())) return false;

    RegEntry* re = rv->parent();
    if(re == nullptr) return false;

    COValue::Type type = rv->dataType();

    int32_t data = 0;
    size_t data_size = COValue::typeSize(type);
    if(data_size > 4 || data_size == 0) return false;

    if(RegTypes::isFractional(type)){
        if(!COValue::valueTo(static_cast<void*>(&data), type, value.toDouble())){
            return false;
        }
    }else if(RegTypes::isNumeric(type)){
        if(RegTypes::isSigned(type)){
            if(!COValue::valueTo(static_cast<void*>(&data), type, value.toInt())){
                return false;
            }
        }else{
            if(!COValue::valueTo(static_cast<void*>(&data), type, value.toUInt())){
                return false;
            }
        }
    }else if(RegTypes::isBoolean(type)){
        if(!COValue::valueTo(static_cast<void*>(&data), type, value.toBool())){
            return false;
        }
    }else{
        return false;
    }

    int res = updateValue(re->index(), rv->subIndex(), true, rv->dataType(), data);
    if(res != 1) return false;

    // inform view.
    emit dataChanged(index, index);

#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(DATA_DEBUG) && DATA_DEBUG == 1
    qDebug() << "RegsViewModel::setData" << "return" << true;
#endif
#endif

    return true;
}


bool RegsViewModel::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const
{
    Q_UNUSED(source_column);
    Q_UNUSED(source_parent);

#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(FILTER_DEBUG) && FILTER_DEBUG == 1
    qDebug() << "RegsViewModel::filterAcceptsColumn" << source_column << source_parent;
#endif
#endif

#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(FILTER_DEBUG) && FILTER_DEBUG == 1
    qDebug() << "RegsViewModel::filterAcceptsColumn" << "return" << true;
#endif
#endif

    return true;
}


bool RegsViewModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    //Q_UNUSED(source_row);
    //Q_UNUSED(source_parent);

#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(FILTER_DEBUG) && FILTER_DEBUG == 1
    qDebug() << "RegsViewModel::filterAcceptsRow" << source_row << source_parent;
#endif
#endif

    auto defRet = [&source_row, &source_parent]() -> bool {
        (void) source_row;
        (void) source_parent;
        return true; //QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
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

#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
#if defined(FILTER_DEBUG) && FILTER_DEBUG == 1
    qDebug() << "RegsViewModel::filterAcceptsRow" << "return" << defRet();
#endif
#endif

    return defRet();
}

void RegsViewModel::m_modelReseted()
{
#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
    qDebug() << "RegsViewModel::m_modelReseted()";
#endif

    beginResetModel();

    m_cache->clear();
    m_queue->clear();

    endResetModel();
}

void RegsViewModel::m_valueUpdateFinished()
{
    //qDebug() << "RegsViewModel::m_valueUpdateFinished" << Qt::hex << m_sdoval->index() << Qt::dec << m_sdoval->subIndex();

    if(m_sdoval->running()) return;
    if(m_queue->isEmpty()) return;

    QVariant val;
    int32_t data = 0;

    UpdateCmd cmd = m_queue->takeFirst();

    if(m_sdoval->index() != cmd.index || m_sdoval->subIndex() != cmd.subindex){
        qDebug() << "RegsViewModel::m_valueUpdateFinished sdova and cmd index or subindex mismatch";
        val = tr("mismatch");
    }else{
        if(m_sdoval->error() == SDOComm::ERROR_NONE){

            COValue::Type type = cmd.type;

            if(!RegTypes::isMemory(type)){

                size_t data_size = m_sdoval->transferSize();

                if(data_size <= sizeof(data)){
                    if(!m_sdoval->copyDataTo(&data, data_size)){
                        qDebug() << "RegsViewModel::m_valueUpdateFinished error copying data from sdo value";
                    }
                    //std::copy(static_cast<uint8_t*>(m_sdoval->data()), static_cast<uint8_t*>(m_sdoval->data()) + m_sdoval->dataSize(), reinterpret_cast<uint8_t*>(&data));
                }

                //qDebug() << Qt::hex << cmd.index << Qt::dec << cmd.subindex << static_cast<int>(cmd.type) << data << data_size;

                if(RegTypes::isInteger(type)){
                    if(RegTypes::isSigned(type)){
                        val = COValue::valueFrom<int32_t>(reinterpret_cast<const void*>(&data), type);
                    }else{
                        val = COValue::valueFrom<uint32_t>(reinterpret_cast<const void*>(&data), type);
                    }
                }else if(RegTypes::isFractional(type)){
                    val = COValue::valueFrom<qreal>(reinterpret_cast<const void*>(&data), type);// / RegTypes::iqBase(type);
                }else if(RegTypes::isBoolean(type)){
                    val = COValue::valueFrom<bool>(reinterpret_cast<const void*>(&data), type);
                }else{
                    val = tr("UNKNOWN");
                }
            }else{
                val = tr("DATA");
            }
        }else{
            switch(m_sdoval->error()){
            case SDOComm::ERROR_IO:
                val = tr("io error");
                break;
            case SDOComm::ERROR_TIMEOUT:
                val = tr("timeout");
                break;
            case SDOComm::ERROR_CANCEL:
                val = tr("canceled");
                break;
            case SDOComm::ERROR_INVALID_SIZE:
                val = tr("invalid size");
                break;
            case SDOComm::ERROR_INVALID_VALUE:
                val = tr("invalid value");
                break;
            case SDOComm::ERROR_ACCESS:
                val = tr("access error");
                break;
            case SDOComm::ERROR_NOT_FOUND:
                val = tr("not found");
                break;
            case SDOComm::ERROR_NO_DATA:
                val = tr("no data");
                break;
            case SDOComm::ERROR_OUT_OF_MEM:
                val = tr("out of mem");
                break;
            case SDOComm::ERROR_GENERAL:
                val = tr("general error");
                break;
            case SDOComm::ERROR_UNKNOWN:
                val = tr("unknown error");
                break;
            default:
                break;
            }
        }
    }

    //qDebug() << val;

    applyUpdatedValue(m_sdoval->index(), m_sdoval->subIndex(), val);

    processQueue();
}

void RegsViewModel::setCachedValue(uint16_t regIndex, uint16_t regSubIndex, const QVariant& val)
{
    reg_fullindex_t fi = RegUtils::makeFullIndex(regIndex, regSubIndex);

    m_cache->insert(fi, val);
}

void RegsViewModel::refreshViewValue(uint16_t regIndex, uint16_t regSubIndex)
{
    QAbstractItemModel* model = sourceModel();
    if(model == nullptr) return;

    RegListModel* reglist_model = qobject_cast<RegListModel*>(model);
    if(reglist_model == nullptr) return;

//    RegVar* rv = reglist_model->varByRegIndex(m_sdoval->index(), m_sdoval->subIndex());
//    if(rv == nullptr) return;

    QModelIndex source_index = reglist_model->objectModelIndexByRegIndex(regIndex, regSubIndex);
    QModelIndex idx = mapFromSource(source_index);
    QModelIndex valIdx = index(idx.row(), COL_VALUE, idx.parent());

    // inform view.
    emit dataChanged(valIdx, valIdx);
}

void RegsViewModel::applyUpdatedValue(uint16_t regIndex, uint16_t regSubIndex, const QVariant& val)
{
    setCachedValue(regIndex, regSubIndex, val);
    refreshViewValue(regIndex, regSubIndex);
}

int RegsViewModel::updateValue(uint16_t regIndex, uint16_t regSubIndex, bool isWrite, COValue::Type type, uint32_t value) const
{
    //qDebug() << "RegsViewModel::updateValue" << Qt::hex << regIndex << Qt::dec << regSubIndex << isWrite << static_cast<int>(type) << value;

    if(!RegTypes::isNumeric(type)) return 0;

    UpdateCmd cmd;
    cmd.index = regIndex;
    cmd.subindex = regSubIndex;
    cmd.isWrite = isWrite;
    cmd.type = type;
    cmd.data = value;

    m_queue->enqueue(cmd);

    int res = processQueue();

    if(res != 0){
        while(processQueue() != 0){
            //
        }
    }

    return res;
}

int RegsViewModel::processQueue() const
{
    if(m_sdoval->running()) return 0;
    if(m_queue->isEmpty()) return 0;

    UpdateCmd& cmd = m_queue->first();
    size_t data_size = COValue::typeSize(cmd.type);

    m_sdoval->setNodeId(m_nodeId); //m_slcon->nodeId()
    m_sdoval->setIndex(cmd.index);
    m_sdoval->setSubIndex(cmd.subindex);
    if(m_sdoval->dataSize() < data_size){
        m_sdoval->setDataSize(data_size);
    }
    m_sdoval->setTransferSize(data_size);

    bool io_res = false;

    if(cmd.isWrite){
        if(data_size <= sizeof(cmd.data)){
            if(m_sdoval->copyDataFrom(&cmd.data, data_size) == data_size){
                io_res = m_sdoval->write();
            }
        }
    }else{
        io_res = m_sdoval->read();
    }

    if(!io_res){
        m_queue->pop_front();
        return -1;
    }

    return 1;
}

CO::NodeId RegsViewModel::nodeId() const
{
    return m_nodeId;
}

void RegsViewModel::setNodeId(CO::NodeId newNodeId)
{
    m_nodeId = newNodeId;
}


void RegsViewModel::setSourceModel(QAbstractItemModel* sourceModel)
{
#if defined(MODEL_DEBUG) && MODEL_DEBUG == 1
    qDebug() << "RegsViewModel::setSourceModel" << this->sourceModel() << sourceModel;
#endif
    QAbstractItemModel* cur_source_model = this->sourceModel();
    if(cur_source_model){
        disconnect(cur_source_model, nullptr, this, nullptr);
    }

    QAbstractProxyModel::setSourceModel(sourceModel);

    if(sourceModel){
        connect(sourceModel, &QAbstractItemModel::modelReset, this, &RegsViewModel::m_modelReseted);
    }
}
