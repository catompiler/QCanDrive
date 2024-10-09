#ifndef REGVARMODEL_H
#define REGVARMODEL_H

#include <QAbstractItemModel>

// Номера столбцов.
#define REGVARMODEL_COL_KEY 0
#define REGVARMODEL_COL_VAL 1

// Номера строк.
#define REGVARMODEL_ROW_DATATYPE 0
#define REGVARMODEL_ROW_MIN_VALUE 1
#define REGVARMODEL_ROW_MAX_VALUE 2
#define REGVARMODEL_ROW_DEF_VALUE 3


class RegVar;


class RegVarModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit RegVarModel(QObject *parent = nullptr);
    ~RegVarModel();

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    RegVar* regVar() const;
    void setRegVar(RegVar* newRegVar);

private:
    RegVar* m_regVar;
};

#endif // REGVARMODEL_H
