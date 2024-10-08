#ifndef REGLISTMODEL_H
#define REGLISTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QMap>
#include "regentry.h"
#include "regobject.h"


typedef QVector<RegEntry*> RegEntryList;


class RegListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit RegListModel(QObject *parent = nullptr);
    ~RegListModel();

    bool hasEntryByIndex(reg_index_t index) const;

    RegEntry* entryByIndex(const QModelIndex& index) const;
    RegObject* objectByIndex(const QModelIndex& index) const;

    bool addEntry(RegEntry* r);
    bool addObject(RegObject* r, const QModelIndex& parent = QModelIndex());

public slots:
    void entryAtIndexModified(const QModelIndex& index);

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    RegEntryList* m_reglist;

};

#endif // REGLISTMODEL_H
