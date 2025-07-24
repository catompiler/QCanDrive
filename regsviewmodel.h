#ifndef REGSVIEWMODEL_H
#define REGSVIEWMODEL_H

#include <QSortFilterProxyModel>

class RegListModel;

class RegsViewModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:

    explicit RegsViewModel(QObject* parent = nullptr);
    ~RegsViewModel();

    Qt::ItemFlags flags(const QModelIndex& index) const override;

protected:
    bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;
};

#endif // REGSVIEWMODEL_H
