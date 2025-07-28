#ifndef REGSVIEWMODEL_H
#define REGSVIEWMODEL_H

//#include <QAbstractProxyModel>
#include <QSortFilterProxyModel>

class RegListModel;

class RegsViewModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:

    enum ColId {
        COL_INDEX = 0,
        COL_NAME,
        COL_DESCR,
        COL_TYPE,
        COL_MIN_VAL,
        COL_MAX_VAL,
        COL_DEF_VAL,
        COL_VALUE
    };

    explicit RegsViewModel(QObject* parent = nullptr);
    ~RegsViewModel();

//    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    //QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role) const override;

//    // QSortFilterProxyModel interface
//protected:
//    bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;
};

#endif // REGSVIEWMODEL_H
