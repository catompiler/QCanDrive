#ifndef REGLISTMODEL_H
#define REGLISTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>



class RegListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit RegListModel(QObject *parent = nullptr);
    ~RegListModel();

    // QAbstractItemModel interface
public:
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    virtual QModelIndex parent(const QModelIndex &child) const override;
    virtual QModelIndex sibling(int row, int column, const QModelIndex &idx) const override;
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual bool hasChildren(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};

#endif // REGLISTMODEL_H
