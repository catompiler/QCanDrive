#ifndef REGLISTMODEL_H
#define REGLISTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QMap>
#include "regtypes.h"


class RegObject;
class RegEntry;
class RegVar;


typedef QVector<RegEntry*> RegEntryList;


class RegListModel : public QAbstractItemModel
{
    Q_OBJECT
public:

    enum ColId {
        COL_INDEX = 0,
        COL_NAME,
        COL_TYPE,
        COL_COUNT,
        COL_DATATYPE,
        COL_MIN_VAL,
        COL_MAX_VAL,
        COL_DEF_VAL,
        COL_BASE,
        COL_FLAGS,
        COL_EXTFLAGS,
        COL_DESCR
    };

    explicit RegListModel(QObject *parent = nullptr);
    ~RegListModel();

    bool hasEntryByRegIndex(reg_index_t index) const;

    QModelIndex entryModelIndex(const RegEntry* entry) const;
    QModelIndex entryModelIndexByModelIndex(const QModelIndex& index) const;
    RegEntry* entryByModelIndex(const QModelIndex& index) const;
    RegObject* objectByModelIndex(const QModelIndex& index) const;

    bool addEntry(RegEntry* r);
    bool addSubObject(RegVar* r, const QModelIndex& parent);

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
    bool removeRows(int row, int count, const QModelIndex& parent) override;

public slots:
    void entryAtIndexModified(const QModelIndex& index);

private:
    RegEntryList* m_reglist;

    //bool setDataEntry(const QModelIndex &index, const QVariant &value, int role);
    //bool setDataVar(const QModelIndex &index, const QVariant &value, int role);

    QVariant dataDisplayRole(const QModelIndex &index) const;
    QVariant dataEditRole(const QModelIndex &index) const;
    QVariant dataSizeHintRole(const QModelIndex &index) const;

    void fixSorting(const QModelIndex& parent);
};

#endif // REGLISTMODEL_H
