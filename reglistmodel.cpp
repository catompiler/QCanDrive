#include "reglistmodel.h"



RegListModel::RegListModel(QObject *parent)
    : QAbstractItemModel{parent}
{

}

RegListModel::~RegListModel()
{

}


QModelIndex RegListModel::index(int row, int column, const QModelIndex &parent) const
{
    return QModelIndex();
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
    return 0;
}

int RegListModel::columnCount(const QModelIndex &parent) const
{
    return 0;
}

bool RegListModel::hasChildren(const QModelIndex &parent) const
{
    return false;
}

QVariant RegListModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

bool RegListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

QVariant RegListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}
