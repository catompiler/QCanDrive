#ifndef REGSELECTMODEL_H
#define REGSELECTMODEL_H

#include <QSortFilterProxyModel>

class RegListModel;

class RegSelectModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:

    explicit RegSelectModel(QObject* parent = nullptr);
    ~RegSelectModel();

    Qt::ItemFlags flags(const QModelIndex& index) const override;

protected:
    bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;
};

#endif // REGSELECTMODEL_H
