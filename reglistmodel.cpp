#include "reglistmodel.h"


static const char* COLUMN_NAMES[] = {
    "Индекс",
    "Тип"
};

static const auto COLUMN_COUNT = sizeof(COLUMN_NAMES) / sizeof(COLUMN_NAMES[0]);

static const int COL_INDEX = 0;
static const int COL_TYPE = 1;


RegListModel::RegListModel(QList<RegEntry*>* reglist, QObject *parent)
    : QAbstractItemModel{parent}
{
    m_reglist = reglist;
}

RegListModel::~RegListModel()
{

}


QModelIndex RegListModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent)) return QModelIndex();

    return createIndex(row, column, nullptr);
}

QModelIndex RegListModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

QModelIndex RegListModel::sibling(int row, int column, const QModelIndex &idx) const
{
    return QModelIndex();
}

int RegListModel::rowCount(const QModelIndex &parent) const
{
    return m_reglist->count();
}

int RegListModel::columnCount(const QModelIndex &parent) const
{
    return COLUMN_COUNT;
}

bool RegListModel::hasChildren(const QModelIndex &parent) const
{
    return false;
}

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
        return QVariant(entry->index());
    case COL_TYPE:
        return QVariant(RegTypes::typeStr(entry->objectType()));
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
