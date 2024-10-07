#include "reglistmodel.h"
#include <algorithm>


static const char* COLUMN_NAMES[] = {
    "Индекс",
    "Тип",
    "Имя"
};

static const auto COLUMN_COUNT = sizeof(COLUMN_NAMES) / sizeof(COLUMN_NAMES[0]);

static const int COL_INDEX = 0;
static const int COL_TYPE = 1;
static const int COL_NAME = 2;


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


QModelIndex RegListModel::index(int row, int column, const QModelIndex &parent) const
{
    //if(!hasIndex(row, column, parent)) return QModelIndex();

    if(!parent.isValid()){
        if(row < 0 || row >= m_reglist->count()) return QModelIndex();
        if(column < 0 || column >= static_cast<int>(COLUMN_COUNT)) return QModelIndex();

        return createIndex(row, column, nullptr);
    }

    return QModelIndex();
}

QModelIndex RegListModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

//QModelIndex RegListModel::sibling(int row, int column, const QModelIndex &idx) const
//{
//    return QModelIndex();
//}

int RegListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()) return 0;
    return m_reglist->count();
}

int RegListModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) return 0;
    return COLUMN_COUNT;
}

//bool RegListModel::hasChildren(const QModelIndex &parent) const
//{
//    return false;
//}

QVariant RegListModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::DisplayRole) return QVariant();
    if(index.column() >= static_cast<int>(COLUMN_COUNT)) return QVariant();
    if(index.row() >= m_reglist->count()) return QVariant();

    auto entry = m_reglist->at(index.row());
    if(entry == nullptr) return QVariant();

    switch(index.column()){
    default:
        return QVariant();
    case COL_INDEX:
        return QVariant(QString("0x") + QString::number(entry->index(), 16));
    case COL_TYPE:
        return QVariant(RegTypes::typeStr(entry->objectType()));
    case COL_NAME:
        return QVariant(entry->name());
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
    if(section < 0 || section >= static_cast<int>(COLUMN_COUNT)) return QVariant();

    return QVariant(COLUMN_NAMES[section]);
}

Qt::ItemFlags RegListModel::flags(const QModelIndex& index) const
{
    return QAbstractItemModel::flags(index);
}
