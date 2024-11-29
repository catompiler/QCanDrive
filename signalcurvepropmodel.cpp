#include "signalcurvepropmodel.h"



SignalCurvePropModel::SignalCurvePropModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

SignalCurvePropModel::~SignalCurvePropModel()
{
}

int SignalCurvePropModel::propsCount() const
{
    return m_signalsProps.size();
}

const SignalCurveProp& SignalCurvePropModel::prop(int n) const
{
    return m_signalsProps[n];
}

void SignalCurvePropModel::setProp(int n, const SignalCurveProp& newProp)
{
    if(n >= m_signalsProps.size()) return;

    m_signalsProps[n] = newProp;

    auto mi = index(n);

    emit dataChanged(mi, mi);
}

void SignalCurvePropModel::addProp(const SignalCurveProp& newProp)
{
    int n = m_signalsProps.size();

    if(!insertRow(n)) return;

    setProp(n, newProp);
}

int SignalCurvePropModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_signalsProps.size();
}

QVariant SignalCurvePropModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role != Qt::DisplayRole)
        return QVariant();

    if(index.row() >= m_signalsProps.size()) return QVariant();

    return m_signalsProps[index.row()].name;
}

bool SignalCurvePropModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(parent.isValid())
        return false;

    if(row < 0 || row > m_signalsProps.size()) return false;

    beginInsertRows(parent, row, row + count - 1);

    for(int i = 0; i < count; i ++){
        m_signalsProps.insert(row + i, SignalCurveProp());
    }

    endInsertRows();

    return true;
}

bool SignalCurvePropModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(parent.isValid()) return false;

    if(count <= 0) return false;
    if(row < 0 || row >= m_signalsProps.size()) return false;
    if(row + count > m_signalsProps.size()) return false;

    beginRemoveRows(parent, row, row + count - 1);

    for(int i = 0; i < count; i ++){
        m_signalsProps.removeAt(row);
    }

    endRemoveRows();

    return true;
}


bool SignalCurvePropModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
    if(sourceParent.isValid()) return false;
    if(destinationParent.isValid()) return false;

    if(count <= 0) return false;
    if(sourceRow < 0 || sourceRow >= m_signalsProps.size()) return false;
    if(sourceRow + count > m_signalsProps.size()) return false;
    if(destinationChild < 0 || destinationChild >= m_signalsProps.size()) return false;
    if(destinationChild + count > m_signalsProps.size()) return false;

    emit layoutAboutToBeChanged();

    for(int i = 0; i < count; i ++){
        m_signalsProps.move(sourceRow, destinationChild + i);
    }

    emit layoutChanged();

    return true;
}
