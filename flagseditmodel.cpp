#include "flagseditmodel.h"
#include <QDebug>


static const char* col_names[] = {
    "Флаг",
};

static decltype(sizeof(int)) col_count = (sizeof(col_names) / sizeof(col_names[0]));



FlagsEditModel::FlagsEditModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_flagsValues = 0;
}

FlagsEditModel::~FlagsEditModel()
{
}

QVariant FlagsEditModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal) return QVariant();
    if(role != Qt::DisplayRole) return QVariant();
    if(section >= static_cast<int>(col_count)) return QVariant();

    return tr(col_names[section]);
}

QModelIndex FlagsEditModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid()) return QModelIndex();
    if(row >= m_flagsNames.count()) return QModelIndex();
    if(column >= static_cast<int>(col_count)) return QModelIndex();

    return createIndex(row, column, nullptr);
}

QModelIndex FlagsEditModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);

    return QModelIndex();
}

int FlagsEditModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;

    return m_flagsNames.count();
}

int FlagsEditModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) return 0;

    return static_cast<int>(col_count);
}

QVariant FlagsEditModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) return QVariant();
    if(index.row() >= m_flagsNames.count()) return QVariant();

    switch(static_cast<Qt::ItemDataRole>(role)){
    default:
        break;
    case Qt::DisplayRole:
        return m_flagsNames.at(index.row());
    case Qt::CheckStateRole:
        return (m_flagsValues & (1 << index.row())) ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

bool FlagsEditModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    //qDebug() << "FlagsEditModel::setData" << index << value << static_cast<Qt::ItemDataRole>(role);

    if(!index.isValid()) return false;
    if(index.row() >= m_flagsNames.count()) return false;

    switch(static_cast<Qt::ItemDataRole>(role)){
    default:
        return false;
    case Qt::CheckStateRole:{
        Qt::CheckState check_state = static_cast<Qt::CheckState>(value.toUInt());
        if(check_state != Qt::Unchecked){
            m_flagsValues |= (1 << index.row());
        }else{
            m_flagsValues &= ~(1 << index.row());
        }
        }break;
    }

    return true;
}

Qt::ItemFlags FlagsEditModel::flags(const QModelIndex& index) const
{
    if(!index.isValid()) return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    flags |= Qt::ItemIsUserCheckable;

    return flags;
}

QStringList FlagsEditModel::flagsNames() const
{
    return m_flagsNames;
}

void FlagsEditModel::setFlagsNames(const QStringList& newFlagsNames)
{
    m_flagsNames = newFlagsNames;
}

uint FlagsEditModel::flagsValues() const
{
    return m_flagsValues;
}

void FlagsEditModel::setFlagsValues(uint newFlagsValue)
{
    m_flagsValues = newFlagsValue;
}
