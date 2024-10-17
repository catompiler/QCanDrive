#include "reglistmodel.h"
#include "regarray.h"
#include "regvar.h"
#include <QSize>
#include <algorithm>
#include <QDebug>


static const char* col_names[] = {
    "Индекс",
    "Имя",
    "Тип",
    "Кол-во",
    "Тип данных",
    "Минимум",
    "Максимум",
    "По-умолчанию",
    "Флаги",
    "Доп. флаги",
    "Описание"
};

static const auto col_count = sizeof(col_names) / sizeof(col_names[0]);


static const int HEADER_WIDTH_SIZE_HINT = 100;
static const int HEADER_HEIGHT_SIZE_HINT = 20;

static const int ITEM_WIDTH_SIZE_HINT = 100;
static const int ITEM_HEIGHT_SIZE_HINT = 20;


RegListModel::RegListModel(QObject *parent)
    : QAbstractItemModel{parent}
{
    m_reglist = new RegEntryList();
}

RegListModel::~RegListModel()
{
    for(auto entry: qAsConst(*m_reglist)){
        delete entry;
    }
    delete m_reglist;
}

bool RegListModel::hasEntryByIndex(reg_index_t index) const
{
    return std::find_if(m_reglist->begin(), m_reglist->end(),
                        [index](const RegEntry* ri){
                            return ri->index() == index;
    }) != m_reglist->end();
}

QModelIndex RegListModel::entryIndex(const QModelIndex& index) const
{
    if(!index.isValid()) return QModelIndex();

    QModelIndex entry_index = index;

    while(entry_index.parent().isValid()){
        entry_index = entry_index.parent();
    }

    return entry_index;
}

RegEntry* RegListModel::entryByIndex(const QModelIndex& index) const
{
    QModelIndex entry_index = entryIndex(index);

    if(!entry_index.isValid()) return nullptr;

    int n = entry_index.row();

    if(n >= m_reglist->count()) return nullptr;

    return m_reglist->at(n);
}

RegObject* RegListModel::objectByIndex(const QModelIndex& index) const
{
    if(!index.isValid()) return nullptr;

    return static_cast<RegObject*>(index.internalPointer());
}

bool RegListModel::addEntry(RegEntry* r)
{
    reg_index_t index = r->index();

    auto it = std::find_if(m_reglist->begin(), m_reglist->end(),
                                [index](const RegEntry* ri){
        return ri->index() >= index;
    });

    // index already exist.
    if(it != m_reglist->end()){
        if((*it)->index() == index){
            return false;
        }
    }

    emit layoutAboutToBeChanged();

    m_reglist->insert(it, r);

    emit layoutChanged();

    return true;
}

bool RegListModel::addSubObject(RegObject* r, const QModelIndex& parent)
{
    if(!parent.isValid()) return false;

    RegObject* ro = static_cast<RegObject*>(parent.internalPointer());

    if(ro == nullptr) return false;
    if(ro->type() != ObjectType::ARR && ro->type() != ObjectType::REC) return false;

    RegMultiObject* mo = static_cast<RegMultiObject*>(ro);

    //auto parents = QList<QPersistentModelIndex>() << QPersistentModelIndex(parent);

    emit layoutAboutToBeChanged(); //parents

    bool res = mo->add(r);

    if(res){
        emit layoutChanged(); //parents
    }

    return res;
}

void RegListModel::entryAtIndexModified(const QModelIndex& index)
{
    QModelIndex topleft = createIndex(index.row(), 0);
    QModelIndex botright = createIndex(index.row(), col_count - 1);
    emit dataChanged(topleft, botright);
}

QModelIndex RegListModel::index(int row, int column, const QModelIndex &parent) const
{
    //qDebug() << "RegListModel::index(" << row << ", " << column << ", " << parent << ")";

    if(!parent.isValid()){
        //if(m_reglist->isEmpty()) return QModelIndex();
        if(column < 0 || column >= static_cast<int>(col_count)) return QModelIndex();
        if(row < 0 || row >= m_reglist->count()) return QModelIndex();

        RegEntry* re = m_reglist->at(row);
        if(re == nullptr) return QModelIndex();

        RegObject* ro = re->object();

        return createIndex(row, column, static_cast<void*>(ro));
    }else{ // parent is valid.
        if(column < 0 || column >= static_cast<int>(col_count)) return QModelIndex();

        RegObject* po = static_cast<RegObject*>(parent.internalPointer());

        if(po == nullptr) return QModelIndex();
        if(po->type() != ObjectType::ARR && po->type() != ObjectType::REC){
            return QModelIndex();
        }

        RegMultiObject* mo = static_cast<RegMultiObject*>(po);

        if(row == 0){
            return createIndex(row, column, static_cast<void*>(mo->countVariable()));
        }

        if(row < 1 || row >= mo->count() + 1) return QModelIndex();

        return createIndex(row, column, static_cast<void*>(mo->at(row - 1)));
    }

    return QModelIndex();
}

QModelIndex RegListModel::parent(const QModelIndex &child) const
{
    //qDebug() << "RegListModel::parent(" << child << ")";

    if(!child.isValid()) return QModelIndex();

    RegObject* ro = static_cast<RegObject*>(child.internalPointer());

    if(ro == nullptr) return QModelIndex();

    RegObject* po = ro->parent();

    if(po == nullptr) return QModelIndex();

    RegObject* ppo = po->parent();

    if(ppo == nullptr){
        auto it = std::find_if(m_reglist->begin(), m_reglist->end(), [po](const RegEntry* re){
            return re->object() == po;
        });

        if(it == m_reglist->end()) return QModelIndex();

        return createIndex(std::distance(m_reglist->begin(), it), 0, po);
    }

    if(ppo->type() != ObjectType::ARR && ppo->type() != ObjectType::REC){
        return QModelIndex();
    }

    RegMultiObject* mo = static_cast<RegMultiObject*>(ppo);

    if(po == mo->countVariable()){
        return createIndex(0, 0, po);
    }

    int po_i = 0;
    for(; po_i < mo->count(); po_i ++){
        if(mo->at(po_i) == po){
            return createIndex(po_i + 1, 0, po);
        }
    }

    return QModelIndex();
}

int RegListModel::rowCount(const QModelIndex &parent) const
{
    //qDebug() << "RegListModel::rowCount(" << parent << ")";

    if(!parent.isValid()){
        return m_reglist->count();
    }

    RegObject* ro = static_cast<RegObject*>(parent.internalPointer());
    if(ro == nullptr) return 0;

    if(ro->type() != ObjectType::ARR && ro->type() != ObjectType::REC) return 0;

    RegMultiObject* mo = static_cast<RegMultiObject*>(ro);

    return mo->count() + 1 /* number of subentries var */;
}

int RegListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
//    qDebug() << "RegListModel::columnCount(" << parent << ")";

//    if(!parent.isValid()){
//        return col_count;
//    }

//    RegObject* ro = static_cast<RegObject*>(parent.internalPointer());
//    if(ro == nullptr) return 0;

//    if(ro->type() != ObjectType::ARR && ro->type() != ObjectType::REC) return 0;

    return col_count;
}

QVariant RegListModel::dataDisplayRole(const QModelIndex& index) const
{
    if(!index.isValid()) return QVariant();
    if(index.column() >= static_cast<int>(col_count)) return QVariant();

    RegEntry* re = nullptr;
    RegObject* ro = nullptr;

    if(!index.parent().isValid()){
        if(index.row() >= m_reglist->count()) return QVariant();

        re = m_reglist->at(index.row());
        if(re == nullptr) return QVariant();

        ro = re->object();
    }else{
        ro = static_cast<RegObject*>(index.internalPointer());
    }

    if(ro == nullptr) return QVariant();

    //RegObject* po = ro->parent();

    switch(index.column()){
    default:
        break;
    case COL_INDEX:
        if(re){
            return QString("0x") + QString::number(re->index(), 16);
        }
        if(ro->type() == ObjectType::VAR){
            return QString("0x") + QString::number(static_cast<RegVar*>(ro)->subIndex(), 16);
        }
        break;
    case COL_NAME:
        return ro->name();
    case COL_TYPE:
        return RegTypes::typeStr(ro->type());
    case COL_COUNT:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::ARR:
        case ObjectType::REC:
            return static_cast<RegMultiObject*>(ro)->count();
        }

    case COL_DATATYPE:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::VAR:
            return RegTypes::dataTypeStr(static_cast<RegVar*>(ro)->dataType());
        }
    case COL_MIN_VAL:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::VAR:
            return static_cast<RegVar*>(ro)->minValue();
        }
    case COL_MAX_VAL:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::VAR:
            return static_cast<RegVar*>(ro)->maxValue();
        }
    case COL_DEF_VAL:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::VAR:
            return static_cast<RegVar*>(ro)->defaultValue();
        }
    case COL_FLAGS:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::VAR:
            return QString("0b") + QString::number(static_cast<RegVar*>(ro)->flags(), 2);
        }
    case COL_EXTFLAGS:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::VAR:
            return QString("0b") + QString::number(static_cast<RegVar*>(ro)->eflags(), 2);
        }
    case COL_DESCR:
        return ro->description();
    }

    return QVariant();
}

QVariant RegListModel::dataEditRole(const QModelIndex& index) const
{
    if(!index.isValid()) return QVariant();
    if(index.column() >= static_cast<int>(col_count)) return QVariant();

    RegEntry* re = nullptr;
    RegObject* ro = nullptr;

    if(!index.parent().isValid()){
        if(index.row() >= m_reglist->count()) return QVariant();

        re = m_reglist->at(index.row());
        if(re == nullptr) return QVariant();

        ro = re->object();
    }else{
        ro = static_cast<RegObject*>(index.internalPointer());
    }

    if(ro == nullptr) return QVariant();

    //RegObject* po = ro->parent();

    switch(index.column()){
    default:
        break;
    case COL_INDEX:
        if(re == nullptr && ro->type() == ObjectType::VAR){
            return static_cast<RegVar*>(ro)->subIndex();
        }
        break;
    case COL_NAME:
        return ro->name();
    case COL_TYPE:
        return static_cast<int>(ro->type());
    case COL_COUNT:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::ARR:
        case ObjectType::REC:
            return static_cast<RegMultiObject*>(ro)->count();
        }

    case COL_DATATYPE:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::VAR:
            return static_cast<unsigned int>(static_cast<RegVar*>(ro)->dataType());
        }
    case COL_MIN_VAL:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::VAR:
            return static_cast<RegVar*>(ro)->minValue();
        }
    case COL_MAX_VAL:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::VAR:
            return static_cast<RegVar*>(ro)->maxValue();
        }
    case COL_DEF_VAL:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::VAR:
            return static_cast<RegVar*>(ro)->defaultValue();
        }
    case COL_FLAGS:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::VAR:
            return static_cast<unsigned int>(static_cast<RegVar*>(ro)->flags());
        }
    case COL_EXTFLAGS:
        switch(ro->type()){
        default:
            return QVariant();
        case ObjectType::VAR:
            return static_cast<unsigned int>(static_cast<RegVar*>(ro)->eflags());
        }
    case COL_DESCR:
        return ro->description();
    }

    return QVariant();
}

QVariant RegListModel::dataSizeHintRole(const QModelIndex& index) const
{
    if(!index.isValid()) return QVariant();

    QSize size = QSize(ITEM_WIDTH_SIZE_HINT, ITEM_HEIGHT_SIZE_HINT);

    return size;
}

QVariant RegListModel::data(const QModelIndex &index, int role) const
{
    //qDebug() << "RegListModel::data(" << index <<", " << static_cast<Qt::ItemDataRole>(role) << ")";

    switch(role){
    default:
        break;
    case Qt::DisplayRole:
    case Qt::ToolTipRole:
        return dataDisplayRole(index);
    case Qt::EditRole:
        return dataEditRole(index);
    case Qt::SizeHintRole:
        return dataSizeHintRole(index);
    }

    return QVariant();
}

bool RegListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //qDebug() << "RegListModel::setData(" << index <<", " << value << ", " << static_cast<Qt::ItemDataRole>(role) << ")";

    if(role != Qt::EditRole) return false;

    if(!index.isValid()) return false;
    if(index.column() >= static_cast<int>(col_count)) return false;

    RegEntry* re = nullptr;
    RegObject* ro = nullptr;

    if(!index.parent().isValid()){
        if(index.row() >= m_reglist->count()) return false;

        re = m_reglist->at(index.row());
        if(re == nullptr) return false;

        ro = re->object();
    }else{
        ro = static_cast<RegObject*>(index.internalPointer());
    }

    if(ro == nullptr) return false;

    //RegObject* po = ro->parent();

    switch(index.column()){
    default:
        return false;
    case COL_INDEX:
        if(re == nullptr && ro->type() == ObjectType::VAR){
            static_cast<RegVar*>(ro)->setSubIndex(static_cast<reg_subindex_t>(value.toUInt()));
            break;
        }
        return false;
    case COL_NAME:
        ro->setName(value.toString());
        break;
    case COL_TYPE:
        return false;
    case COL_COUNT:
        return false;
    case COL_DATATYPE:
        switch(ro->type()){
        default:
            return false;
        case ObjectType::VAR:
            static_cast<RegVar*>(ro)->setDataType(static_cast<DataType>(value.toUInt()));
            break;
        }
        break;
    case COL_MIN_VAL:
        switch(ro->type()){
        default:
            return false;
        case ObjectType::VAR:
            static_cast<RegVar*>(ro)->setMinValue(value);
            break;
        }
        break;
    case COL_MAX_VAL:
        switch(ro->type()){
        default:
            return false;
        case ObjectType::VAR:
            static_cast<RegVar*>(ro)->setMaxValue(value);
            break;
        }
        break;
    case COL_DEF_VAL:
        switch(ro->type()){
        default:
            return false;
        case ObjectType::VAR:
            static_cast<RegVar*>(ro)->setDefaultValue(value);
            break;
        }
        break;
    case COL_FLAGS:
        switch(ro->type()){
        default:
            return false;
        case ObjectType::VAR:
            static_cast<RegVar*>(ro)->setFlags(static_cast<reg_flags_t>(value.toUInt()));
            break;
        }
    case COL_EXTFLAGS:
        switch(ro->type()){
        default:
            return false;
        case ObjectType::VAR:
            static_cast<RegVar*>(ro)->setEFlags(static_cast<reg_flags_t>(value.toUInt()));
            break;
        }
        break;
    case COL_DESCR:
        ro->setDescription(value.toString());
        break;
    }

    emit dataChanged(index, index); // QVector<int>{Qt::EditRole, Qt::DisplayRole}

    return true;
}

QVariant RegListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    //qDebug() << "RegListModel::headerData(" << section <<", " << orientation <<", " << static_cast<Qt::ItemDataRole>(role) << ")";

    if(orientation != Qt::Horizontal){
        return QAbstractItemModel::headerData(section, orientation, role);
    }
    if(section < 0 || section >= static_cast<int>(col_count)){
        return QAbstractItemModel::headerData(section, orientation, role);
    }

    switch(role){
    default:
        return QAbstractItemModel::headerData(section, orientation, role);
    case Qt::DisplayRole:
    case Qt::ToolTipRole:
        break;
    case Qt::TextAlignmentRole:
        return static_cast<int>(Qt::AlignHCenter | Qt::AlignVCenter);
    case Qt::SizeHintRole:
        return QSize(HEADER_WIDTH_SIZE_HINT, HEADER_HEIGHT_SIZE_HINT);
    }

    return QVariant(col_names[section]);
}

Qt::ItemFlags RegListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    if(!index.isValid()) return flags;

    RegObject* ro = static_cast<RegObject*>(index.internalPointer());
    if(ro == nullptr) return flags;

    RegObject* po = ro->parent();
    bool hasParent = po != nullptr;

    switch(ro->type()){
    case ObjectType::ARR:
        switch(index.column()){
        default:
            break;
        case COL_NAME:
            flags |= Qt::ItemIsEditable;
            break;
        }
        break;
    case ObjectType::REC:
        switch(index.column()){
        default:
            break;
        case COL_NAME:
            flags |= Qt::ItemIsEditable;
            break;
        }
        break;
    case ObjectType::VAR:
        flags |= Qt::ItemNeverHasChildren;

        switch(index.column()){
        default:
            break;
        case COL_INDEX:
            if(hasParent && index.row() != 0) flags |= Qt::ItemIsEditable;
            break;
        case COL_NAME:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_DATATYPE:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_MIN_VAL:
        case COL_MAX_VAL:
        case COL_DEF_VAL:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_FLAGS:
        case COL_EXTFLAGS:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_DESCR:
            flags |= Qt::ItemIsEditable;
            break;
        }
        break;
    }

    return flags;
}
