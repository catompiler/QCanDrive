#ifndef REGSELECTMODEL_H
#define REGSELECTMODEL_H

#include <QAbstractProxyModel>

class RegListModel;

class RegSelectModel : public QAbstractProxyModel
{
    Q_OBJECT
public:

    enum Columns {
        COL_INDEX = 0,
        COL_NAME = 1,
        COL_DESCR = 2,
        ColumnsCount = 3
    };

    explicit RegSelectModel(QObject* parent = nullptr);
    ~RegSelectModel();

    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;

    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
};

#endif // REGSELECTMODEL_H
