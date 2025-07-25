#ifndef REGSVIEWMODEL_H
#define REGSVIEWMODEL_H

#include <QAbstractProxyModel>

class RegListModel;

class RegsViewModel : public QAbstractProxyModel
{
    Q_OBJECT
public:

    explicit RegsViewModel(QObject* parent = nullptr);
    ~RegsViewModel();

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;

    // QAbstractProxyModel interface
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

protected:
    //bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;
};

#endif // REGSVIEWMODEL_H
