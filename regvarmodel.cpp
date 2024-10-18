#include "regvarmodel.h"
#include "regtypes.h"
#include "regvar.h"
#include <QSize>
#include <QDebug>


static const char* col_names[] = {
    "Свойство",
    "Значение"
};
static const auto col_count = (sizeof(col_names) / sizeof(col_names[0]));

static const char* row_names[] = {
    "Тип данных",
    "Мин. значение",
    "Макс. занчение",
    "Значение. по-умолчанию"
};
static const auto row_count = (sizeof(row_names) / sizeof(row_names[0]));



RegVarModel::RegVarModel(QObject *parent)
    : QAbstractItemModel{parent}
{
    m_regVar = nullptr;
}

RegVarModel::~RegVarModel()
{
}


QModelIndex RegVarModel::index(int row, int column, const QModelIndex& parent) const
{
    if(!parent.isValid()){
        if(row < 0 || row >= static_cast<int>(row_count)) return QModelIndex();
        if(column < 0 || column >= static_cast<int>(col_count)) return QModelIndex();

        return createIndex(row, column, nullptr);
    }

    return QModelIndex();
}

QModelIndex RegVarModel::parent(const QModelIndex& child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int RegVarModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid()) return 0;
    return row_count;
}

int RegVarModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid()) return 0;
    return col_count;
}

QVariant RegVarModel::data(const QModelIndex& index, int role) const
{
    //qDebug() << "RegVarModel::data" << "role:" << role;

    if(!index.isValid()) return QVariant();

    int index_row = index.row();
    int index_col = index.column();

    if(index_col >= static_cast<int>(col_count) ||
       index_row >= static_cast<int>(row_count)) return QVariant();

    switch(role){
    case Qt::EditRole:
        if(index_col == REGVARMODEL_COL_KEY) return QVariant();
        break;
    case Qt::DisplayRole:
    case Qt::ToolTipRole:
        if(index_col == REGVARMODEL_COL_KEY){
            return QVariant(tr(row_names[index.row()]));
        }
        break;
    case Qt::TextAlignmentRole:
        if(index_col == REGVARMODEL_COL_KEY) return Qt::AlignRight;
        if(index_col == REGVARMODEL_COL_VAL) return Qt::AlignLeft;
        return QVariant();
    case Qt::SizeHintRole:
        if(index_col == REGVARMODEL_COL_KEY) return QSize(150, 20);
        return QVariant();
    default:
        return QVariant();
    }

    // (role == Qt::DisplayRole ||
    //  role == Qt::ToolTipRole ||
    //  role == Qt::EditRole)
    //  && index_col != 0.
    if(m_regVar == nullptr) return QVariant();

    switch(index_row){
    default:
        break;
    case REGVARMODEL_ROW_DATATYPE:
        return RegTypes::dataTypeStr(m_regVar->dataType());
    case REGVARMODEL_ROW_MIN_VALUE:
        return m_regVar->minValue();
    case REGVARMODEL_ROW_MAX_VALUE:
        return m_regVar->maxValue();
    case REGVARMODEL_ROW_DEF_VALUE:
        return m_regVar->defaultValue();
    }
    //}
    return QVariant();
}

bool RegVarModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    //qDebug() << "RegVarModel::setData" << "role:" << role << "value" << value;

    if(!index.isValid()) return false;
    if(role != Qt::EditRole) return false;
    if(m_regVar == nullptr) return false;
    if(index.column() >= static_cast<int>(col_count) ||
       index.row() >= static_cast<int>(row_count)) return false;

    if(index.column() != REGVARMODEL_COL_VAL) return false;

    switch(index.row()){
    default:
        return false;
//    case ROW_DATATYPE:
//        break;
//    case ROW_MIN_VALUE:
//        break;
//    case ROW_MAX_VALUE:
//        break;
//    case ROW_DEF_VALUE:
//        break;
//    case ROW_IS_PARAMETER:
//        break;
    }

    return true;
}

QVariant RegVarModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    //qDebug() << "RegVarModel::headerData" << "role:" << role;

    if(orientation != Qt::Horizontal) return QVariant();
    if(section < 0 || section >= static_cast<int>(col_count)) return QVariant();

    switch(role){
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
        return QVariant(tr(col_names[section]));
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    default:
        break;
    }

    return QVariant();
}

Qt::ItemFlags RegVarModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    if(index.isValid() && index.column() == 1){
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

RegVar* RegVarModel::regVar() const
{
    return m_regVar;
}

void RegVarModel::setRegVar(RegVar* newRegVar)
{
    m_regVar = newRegVar;
}
