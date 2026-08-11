#ifndef SDOSCOPECHANNELSMODEL_H
#define SDOSCOPECHANNELSMODEL_H

#include <QAbstractItemModel>
#include "regtypes.h"
#include "regutils.h"
#include <QVector>



class SDOScopeChannelsModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit SDOScopeChannelsModel(QObject *parent = nullptr);

    enum Column {
        COL_ENABLED = 0,
        COL_NAME = 1,
        COL_REG = 2,
        COL_DATA_TYPE = 3,
        COL_BASE_VALUE = 4
    };
    ~SDOScopeChannelsModel();

    struct ChannelData {
        bool enabled;
        QString name;
        reg_fullindex_t regFullIndex;
        DataType dataType;
        qreal baseValue;
    };

    using ChannelsData = QVector<ChannelData>;

    void setChannelsData(ChannelsData newData);
    const ChannelsData& channelsData() const;

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    ChannelsData* m_data;
};

#endif // SDOSCOPECHANNELSMODEL_H
