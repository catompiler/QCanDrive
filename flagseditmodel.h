#ifndef FLAGSEDITMODEL_H
#define FLAGSEDITMODEL_H

#include <QAbstractItemModel>
#include <QString>
#include <QStringList>



class FlagsEditModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit FlagsEditModel(QObject *parent = nullptr);
    ~FlagsEditModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QStringList flagsNames() const;
    void setFlagsNames(const QStringList& newFlagsNames);

    uint flagsValues() const;
    void setFlagsValues(uint newFlagsValue);

private:
    QStringList m_flagsNames;
    uint m_flagsValues;
};

#endif // FLAGSEDITMODEL_H
