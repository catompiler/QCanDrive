#include "sdoscopechannelsmodel.h"
#include "regtypes.h"
#include "regutils.h"
#include <QSize>
#include <QDebug>


static const char* col_names[] = {
    "Включен",
    "Имя",
    "Регистр",
    "Тип данных",
    "Базовое значение"
};

static const int col_count = (sizeof(col_names) / sizeof(col_names[0]));


SDOScopeChannelsModel::SDOScopeChannelsModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_data = new ChannelsData();
}

SDOScopeChannelsModel::~SDOScopeChannelsModel()
{
    delete m_data;
}

void SDOScopeChannelsModel::setChannelsData(ChannelsData newData)
{
    beginResetModel();

    m_data->swap(newData);

    endResetModel();

    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

const SDOScopeChannelsModel::ChannelsData& SDOScopeChannelsModel::channelsData() const
{
    return *m_data;
}

QVariant SDOScopeChannelsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    //qDebug() << "SDOScopeChannelsModel::headerData(" << section <<", " << orientation <<", " << static_cast<Qt::ItemDataRole>(role) << ")";

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
    }

    return QVariant(col_names[section]);
}

QModelIndex SDOScopeChannelsModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid()) return QModelIndex();

    return createIndex(row, column, nullptr);
}

QModelIndex SDOScopeChannelsModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);

    return QModelIndex();
}

int SDOScopeChannelsModel::rowCount(const QModelIndex &parent) const
{
    // no childs.
    if(parent.isValid()){
        return 0;
    }

    return m_data->size();
}

int SDOScopeChannelsModel::columnCount(const QModelIndex &parent) const
{
    //Q_UNUSED(parent);
    if(parent.isValid()) return 0;

    return col_count;
}

QVariant SDOScopeChannelsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() >= m_data->size()) return QVariant();
    if(index.column() >= col_count) return QVariant();

    const ChannelData& ch_data = m_data->at(index.row());

    switch(role){
    default:
        break;
    case Qt::DisplayRole:
    case Qt::ToolTipRole:
        switch(index.column()){
        default:
            break;
        case COL_ENABLED:
            return RegTypes::boolStr(ch_data.enabled);
        case COL_NAME:
            return ch_data.name;
        case COL_REG:
            return RegUtils::fullIndexToString(ch_data.regFullIndex);
        case COL_DATA_TYPE:
            return RegTypes::dataTypeStr(ch_data.dataType);
        case COL_BASE_VALUE:
            return ch_data.baseValue;
        }
        break;
    case Qt::EditRole:
        switch(index.column()){
        default:
            break;
        case COL_ENABLED:
            return ch_data.enabled;
        case COL_NAME:
            return ch_data.name;
        case COL_REG:
            return ch_data.regFullIndex;
        case COL_DATA_TYPE:
            return static_cast<int>(ch_data.dataType);
        case COL_BASE_VALUE:
            return ch_data.baseValue;
        }
        break;
    }

    return QVariant();
}

bool SDOScopeChannelsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags SDOScopeChannelsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    if(!index.isValid()) return flags;

    flags |= Qt::ItemIsEditable | Qt::ItemNeverHasChildren;

    return flags;
}
