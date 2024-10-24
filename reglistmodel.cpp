#include "reglistmodel.h"
#include "regentry.h"
#include "regobject.h"
#include "regvar.h"
#include <QSize>
#include <algorithm>
#include <iterator>
#include <QDebug>


static const char* col_names[] = {
    "Индекс",
    "Имя",
    "Тип",
    "Кол-во",
    "Память",
    "Минимум",
    "Максимум",
    "По-умолчанию",
    "Баз. знач.",
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

bool RegListModel::hasEntryByRegIndex(reg_index_t index) const
{
    return std::find_if(m_reglist->begin(), m_reglist->end(),
                        [index](const RegEntry* ri){
                            return ri->index() == index;
    }) != m_reglist->end();
}

QModelIndex RegListModel::entryModelIndex(const RegEntry* entry) const
{
    auto it = std::find_if(m_reglist->begin(), m_reglist->end(),
                           [entry](const RegEntry* ri){
                               return ri->index() == entry->index();
    });

    if(it == m_reglist->end()) return QModelIndex();

    return createIndex(std::distance(m_reglist->begin(), it), 0, static_cast<void*>(*it));
}

QModelIndex RegListModel::entryModelIndexByModelIndex(const QModelIndex& index) const
{
    if(!index.isValid()) return QModelIndex();

    QModelIndex entry_index = index;
    QModelIndex parent_index = entry_index.parent();

    while(parent_index.isValid()){
        entry_index = parent_index;
        parent_index = entry_index.parent();
    }

    return entry_index;
}

RegEntry* RegListModel::entryByModelIndex(const QModelIndex& index) const
{
    QModelIndex entry_index = entryModelIndexByModelIndex(index);

    if(!entry_index.isValid()) return nullptr;

    int n = entry_index.row();

    if(n >= m_reglist->count()) return nullptr;

    return m_reglist->at(n);
}

RegObject* RegListModel::objectByModelIndex(const QModelIndex& index) const
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

bool RegListModel::addSubObject(RegVar* r, const QModelIndex& parent)
{
    if(!parent.isValid()) return false;

    RegEntry* re = static_cast<RegEntry*>(parent.internalPointer());
    if(re == nullptr) return false;
    if(re->hasVarBySubIndex(r->subIndex())) return false;

    emit layoutAboutToBeChanged(); //parents

    bool res = re->add(r);

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
    fixSorting(index.parent());
}

bool RegListModel::removeRows(int row, int count, const QModelIndex& parent)
{
    //qDebug() << "RegListModel::removeRows(" << row << ", " << count << ", " << parent << ")";

    if(count == 0) return false;

    if(parent.isValid()){
        RegEntry* re = static_cast<RegEntry*>(parent.internalPointer());
        if(re == nullptr) return false;

        if(row < 0 || row >= re->count()) return false;
        if(row + count > re->count()) return false;

        beginRemoveRows(parent, row, row + count - 1);

        while(count --){
            re->remove(row);
        }

        endRemoveRows();

    }else{
        if(row < 0 || row >= m_reglist->count()) return false;
        if(row + count > m_reglist->count()) return false;

        auto first = m_reglist->begin();
        std::advance(first, row);

        auto last = first;
        std::advance(last, count);

        beginRemoveRows(parent, row, row + count - 1);

        qDeleteAll(first, last);
        m_reglist->erase(first, last);

        endRemoveRows();
    }

    return true;
}

QModelIndex RegListModel::index(int row, int column, const QModelIndex &parent) const
{
    //qDebug() << "RegListModel::index(" << row << ", " << column << ", " << parent << ")";

    if(!parent.isValid()){
        if(column < 0 || column >= static_cast<int>(col_count)) return QModelIndex();
        if(row < 0 || row >= m_reglist->count()) return QModelIndex();

        RegEntry* re = m_reglist->at(row);
        if(re == nullptr) return QModelIndex();

        return createIndex(row, column, static_cast<void*>(re));
    }else{ // parent is valid.
        if(column < 0 || column >= static_cast<int>(col_count)) return QModelIndex();

        RegEntry* re = static_cast<RegEntry*>(parent.internalPointer());
        if(re == nullptr) return QModelIndex();

        if(row < 0 || row >= re->count()) return QModelIndex();

        return createIndex(row, column, static_cast<void*>(re->at(row)));
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

    RegEntry* re = static_cast<RegEntry*>(po);
    int child_index = m_reglist->indexOf(re);

    if(child_index == -1) return QModelIndex();

    return createIndex(child_index, 0, static_cast<void*>(re));
}

int RegListModel::rowCount(const QModelIndex &parent) const
{
    //qDebug() << "RegListModel::rowCount(" << parent << ")";

    if(!parent.isValid()){
        return m_reglist->count();
    }

    RegObject* ro = static_cast<RegObject*>(parent.internalPointer());
    if(ro == nullptr) return 0;

    if(ro->parent() != nullptr) return 0;

    RegEntry* re = static_cast<RegEntry*>(ro);

    return re->count();
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

    RegObject* ro = static_cast<RegObject*>(index.internalPointer());
    if(ro == nullptr) return false;

    RegEntry* pe = ro->parent();

    // entry.
    if(pe == nullptr){
        RegEntry* re = static_cast<RegEntry*>(ro);

        switch(static_cast<ColId>(index.column())){
        default:
            break;
        case COL_INDEX:
            return QString("0x%1").arg(static_cast<unsigned int>(re->index()), 0, 16);
        case COL_NAME:
            return re->name();
        case COL_TYPE:
            return RegTypes::typeStr(re->type());
        case COL_COUNT:
            return static_cast<unsigned int>(re->count());
        case COL_MEM_ADDR:
        case COL_MIN_VAL:
        case COL_MAX_VAL:
        case COL_DEF_VAL:
        case COL_BASE:
        case COL_FLAGS:
        case COL_EXTFLAGS:
            break;
        case COL_DESCR:
            return re->description();
        }
    }
    // var.
    else{
        RegVar* rv = static_cast<RegVar*>(ro);

        switch(static_cast<ColId>(index.column())){
        default:
            break;
        case COL_INDEX:
            return QString("0x%1").arg(static_cast<unsigned int>(rv->subIndex()), 0, 16);
        case COL_NAME:
            return ro->name();
        case COL_TYPE:
            return RegTypes::dataTypeStr(rv->dataType());
        case COL_COUNT:
            if(pe->type() != ObjectType::ARR) return QVariant();
            if(rv->count() == 1) return QVariant();
            return QString("[%1]").arg(static_cast<unsigned int>(rv->count()));
        case COL_MEM_ADDR:
            return rv->memAddr();
        case COL_MIN_VAL:
            return rv->minValue();
        case COL_MAX_VAL:
            return rv->maxValue();
        case COL_DEF_VAL:
            return rv->defaultValue();
        case COL_BASE:{
            unsigned int base_index = rv->baseIndex();
            unsigned int base_subindex = rv->baseSubIndex();
            return QString("0x%1.%2")
                    .arg(base_index, 4, 16, QChar('0'))
                    .arg(base_subindex, 2, 16, QChar('0'));
        }
        case COL_FLAGS:
            return QString("0b%1").arg(static_cast<unsigned int>(rv->flags()), 0, 2);
        case COL_EXTFLAGS:
            return QString("0b%1").arg(static_cast<unsigned int>(rv->eflags()), 0, 2);
        case COL_DESCR:
            return rv->description();
        }
    }

    return QVariant();
}

QVariant RegListModel::dataEditRole(const QModelIndex& index) const
{
    if(!index.isValid()) return QVariant();
    if(index.column() >= static_cast<int>(col_count)) return QVariant();

    RegObject* ro = static_cast<RegObject*>(index.internalPointer());
    if(ro == nullptr) return false;

    RegEntry* pe = ro->parent();

    // entry.
    if(pe == nullptr){
        RegEntry* re = static_cast<RegEntry*>(ro);

        switch(static_cast<ColId>(index.column())){
        default:
            break;
        case COL_INDEX:
            return static_cast<unsigned int>(re->index());
        case COL_NAME:
            return re->name();
        case COL_TYPE:
            return static_cast<unsigned int>(re->type());
        case COL_COUNT:
            return static_cast<unsigned int>(re->count());
        case COL_MEM_ADDR:
        case COL_MIN_VAL:
        case COL_MAX_VAL:
        case COL_DEF_VAL:
        case COL_BASE:
        case COL_FLAGS:
        case COL_EXTFLAGS:
            break;
        case COL_DESCR:
            return re->description();
        }
    }
    // var.
    else{
        RegVar* rv = static_cast<RegVar*>(ro);

        switch(static_cast<ColId>(index.column())){
        default:
            break;
        case COL_INDEX:
            return rv->subIndex();
        case COL_NAME:
            return ro->name();
        case COL_TYPE:
            return static_cast<unsigned int>(rv->dataType());
        case COL_COUNT:
            if(pe->type() != ObjectType::ARR) return QVariant();
            return static_cast<unsigned int>(rv->count());
        case COL_MEM_ADDR:
            return rv->memAddr();
        case COL_MIN_VAL:
            return rv->minValue();
        case COL_MAX_VAL:
            return rv->maxValue();
        case COL_DEF_VAL:
            return rv->defaultValue();
        case COL_BASE:{
            unsigned int base_index = rv->baseIndex();
            unsigned int base_subindex = rv->baseSubIndex();
            return static_cast<unsigned int>((base_index << 8) | base_subindex);
        }
        case COL_FLAGS:
            return static_cast<unsigned int>(rv->flags());
        case COL_EXTFLAGS:
            return static_cast<unsigned int>(rv->eflags());
        case COL_DESCR:
            return rv->description();
        }
    }

    return QVariant();
}

QVariant RegListModel::dataSizeHintRole(const QModelIndex& index) const
{
    if(!index.isValid()) return QVariant();

    QSize size = QSize(ITEM_WIDTH_SIZE_HINT, ITEM_HEIGHT_SIZE_HINT);

    return size;
}

void RegListModel::fixSorting(const QModelIndex& parent)
{
    const auto parents_list = QList<QPersistentModelIndex>({parent});

    emit layoutAboutToBeChanged(parents_list, QAbstractItemModel::VerticalSortHint);
    // Entry.
    if(!parent.isValid()){
        std::sort(m_reglist->begin(), m_reglist->end(), [](RegEntry* rl, RegEntry* rr){
            return rl->index() < rr->index();
        });
    }
    // Var.
    else{
        RegEntry* re = static_cast<RegEntry*>(parent.internalPointer());
        std::sort(re->begin(), re->end(), [](RegVar* rl, RegVar* rr){
            return rl->subIndex() < rr->subIndex();
        });
    }
    emit layoutChanged(parents_list, QAbstractItemModel::VerticalSortHint);
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

    RegObject* ro = static_cast<RegObject*>(index.internalPointer());
    if(ro == nullptr) return false;

    RegEntry* pe = ro->parent();

    // entry.
    if(pe == nullptr){
        RegEntry* re = static_cast<RegEntry*>(ro);

        switch(static_cast<ColId>(index.column())){
        case COL_INDEX:{
            reg_index_t newIndex = value.toUInt();
            if(newIndex != re->index()){
                if(hasEntryByRegIndex(newIndex)){
                    return false;
                }
                re->setIndex(newIndex);
                fixSorting(index.parent());
            }
        }break;
        case COL_NAME:
            ro->setName(value.toString());
            break;
        case COL_TYPE:
            re->setType(static_cast<ObjectType>(value.toUInt()));
            break;
        case COL_COUNT:
        case COL_MEM_ADDR:
        case COL_MIN_VAL:
        case COL_MAX_VAL:
        case COL_DEF_VAL:
        case COL_BASE:
        case COL_FLAGS:
        case COL_EXTFLAGS:
            return false;
        case COL_DESCR:
            ro->setDescription(value.toString());
            break;
        }
    }
    // var.
    else{
        RegVar* rv = static_cast<RegVar*>(ro);

        switch(static_cast<ColId>(index.column())){
        case COL_INDEX:{
            reg_index_t newSubIndex = value.toUInt();
            if(newSubIndex != rv->subIndex()){
                if(pe->hasVarBySubIndex(newSubIndex)){
                    return false;
                }
                rv->setSubIndex(newSubIndex);
                fixSorting(index.parent());
            }
        }break;
        case COL_NAME:
            ro->setName(value.toString());
            break;
        case COL_TYPE:
            rv->setDataType(static_cast<DataType>(value.toUInt()));
            break;
        case COL_COUNT:
            if(pe->type() != ObjectType::ARR) return false;
            rv->setCount(value.toUInt());
            break;
        case COL_MEM_ADDR:
            rv->setMemAddr(value.toString());
            break;
        case COL_MIN_VAL:
            rv->setMinValue(value.toString());
            break;
        case COL_MAX_VAL:
            rv->setMaxValue(value.toString());
            break;
        case COL_DEF_VAL:
            rv->setDefaultValue(value.toString());
            break;
        case COL_BASE:{
            unsigned int base = value.toUInt();
            rv->setBaseIndex(base >> 8);
            rv->setBaseSubIndex(base & 0xff);
        }break;
        case COL_FLAGS:
            rv->setFlags(static_cast<reg_flags_t>(value.toUInt()));
            break;
        case COL_EXTFLAGS:
            rv->setEFlags(static_cast<reg_flags_t>(value.toUInt()));
            break;
        case COL_DESCR:
            ro->setDescription(value.toString());
            break;
        }
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

    RegEntry* pe = ro->parent();

    // entry.
    if(pe == nullptr){
        //RegEntry* re = static_cast<RegEntry*>(ro);

        switch(static_cast<ColId>(index.column())){
        case COL_INDEX:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_NAME:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_TYPE:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_COUNT:
            break;
        case COL_MEM_ADDR:
            break;
        case COL_MIN_VAL:
            break;
        case COL_MAX_VAL:
            break;
        case COL_DEF_VAL:
            break;
        case COL_BASE:
            break;
        case COL_FLAGS:
            break;
        case COL_EXTFLAGS:
            break;
        case COL_DESCR:
            break;
        default:
            break;
        }
    }
    // var.
    else{
        //RegVar* rv = static_cast<RegVar*>(ro);

        flags |= Qt::ItemNeverHasChildren;

        switch(static_cast<ColId>(index.column())){
        case COL_INDEX:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_NAME:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_TYPE:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_COUNT:
            if(pe->type() == ObjectType::ARR) flags |= Qt::ItemIsEditable;
            break;
        case COL_MEM_ADDR:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_MIN_VAL:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_MAX_VAL:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_DEF_VAL:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_BASE:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_FLAGS:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_EXTFLAGS:
            flags |= Qt::ItemIsEditable;
            break;
        case COL_DESCR:
            flags |= Qt::ItemIsEditable;
            break;
        default:
            break;
        }
    }

    return flags;
}

