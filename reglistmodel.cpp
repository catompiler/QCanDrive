#include "reglistmodel.h"
#include "regarray.h"
#include "regvar.h"
#include <algorithm>


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

enum ColNames {
    COL_INDEX = 0,
    COL_NAME,
    COL_TYPE,
    COL_COUNT,
    COL_DATATYPE,
    COL_MIN_VAL,
    COL_MAX_VAL,
    COL_DEF_VAL,
    COL_FLAGS,
    COL_EXTFLAGS,
    COL_DESCR
};


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

RegEntry* RegListModel::entryByIndex(const QModelIndex& index) const
{
    if(!index.isValid()) return nullptr;

    QModelIndex entry_index = index;

    while(entry_index.parent().isValid()){
        entry_index = entry_index.parent();
    }

    int n = entry_index.row();

    if(n >= m_reglist->count()) return nullptr;

    return m_reglist->at(n);
}

RegObject* RegListModel::objectByIndex(const QModelIndex& index) const
{
    RegEntry* re = entryByIndex(index);

    if(re == nullptr) return nullptr;

    return re->object();
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

bool RegListModel::addObject(RegObject* r, const QModelIndex& parent)
{
    if(!parent.isValid()){
        return false;
    }

    return true;
}

void RegListModel::entryAtIndexModified(const QModelIndex& index)
{
    QModelIndex topleft = createIndex(index.row(), 0);
    QModelIndex botright = createIndex(index.row(), col_count - 1);
    emit dataChanged(topleft, botright);
}

QModelIndex RegListModel::index(int row, int column, const QModelIndex &parent) const
{
    //if(!hasIndex(row, column, parent)) return QModelIndex();

    if(!parent.isValid()){
        if(row < 0 || row >= m_reglist->count()) return QModelIndex();
        if(column < 0 || column >= static_cast<int>(col_count)) return QModelIndex();

        return createIndex(row, column, nullptr);
    }

    return QModelIndex();
}

QModelIndex RegListModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int RegListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()) return 0;
    return m_reglist->count();
}

int RegListModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) return 0;
    return col_count;
}

QVariant RegListModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::DisplayRole) return QVariant();
    if(index.column() >= static_cast<int>(col_count)) return QVariant();
    if(index.row() >= m_reglist->count()) return QVariant();

    auto entry = m_reglist->at(index.row());
    if(entry == nullptr) return QVariant();

    auto regobj = objectByIndex(index);
    if(regobj == nullptr) return QVariant();

    switch(index.column()){
    default:
        return QVariant();
    case COL_INDEX:
        return QString("0x") + QString::number(entry->index(), 16);
    case COL_NAME:
        return regobj->name();
    case COL_TYPE:
        return RegTypes::typeStr(entry->objectType());
    case COL_COUNT:
        switch(regobj->type()){
        default:
        case ObjectType::VAR:
            return QString("");
        //case ObjectType::ARR:
            //return static_cast<RegArray*>(regobj)
        }

    case COL_DATATYPE:
    case COL_MIN_VAL:
    case COL_MAX_VAL:
    case COL_DEF_VAL:
    case COL_FLAGS:
    case COL_EXTFLAGS:
        return QString("");
    case COL_DESCR:
        return regobj->description();
    }

    return QVariant();
}

bool RegListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

QVariant RegListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal) return QVariant();
    if(role != Qt::DisplayRole) return QVariant();
    if(section < 0 || section >= static_cast<int>(col_count)) return QVariant();

    return QVariant(col_names[section]);
}

Qt::ItemFlags RegListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    if(!index.isValid()) return flags;

    if(index.column() > COL_INDEX) flags |= Qt::ItemIsEditable;

    return flags;
}
