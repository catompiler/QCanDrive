#ifndef REGSVIEWMODEL_H
#define REGSVIEWMODEL_H

//#include <QAbstractProxyModel>
#include <QSortFilterProxyModel>
#include <QHash>
#include <QQueue>
#include "cotypes.h"
#include "covaluetypes.h"

class SDOValue;
class SLCanOpenNode;
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

    SLCanOpenNode* getSLCanOpenNode();
    const SLCanOpenNode* getSLCanOpenNode() const;
    void setSLCanOpenNode(SLCanOpenNode* slcon);

    //QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    //QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    CO::NodeId nodeId() const;
    void setNodeId(CO::NodeId newNodeId);

protected:
    // QSortFilterProxyModel interface
    bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

private slots:
    void m_modelReseted();
    void m_valueUpdateFinished();

private:
    void setCachedValue(uint16_t regIndex, uint16_t regSubIndex, const QVariant& val);
    void refreshViewValue(uint16_t regIndex, uint16_t regSubIndex);
    void applyUpdatedValue(uint16_t regIndex, uint16_t regSubIndex, const QVariant& val);

    // 0 - нет ошибки, но и обновление не производится.
    // 1 - обновление производится.
    // -1 - ошибка.
    int updateValue(uint16_t regIndex, uint16_t regSubIndex, bool isWrite, COValue::Type type, uint32_t value = 0) const;
    int processQueue() const;

    SLCanOpenNode* m_slcon;
    SDOValue* m_sdoval;
    CO::NodeId m_nodeId;

//    typedef struct _S_CachedValue {
//        uint32_t data;
//        COValue::Type type;
//    } CachedValue;

    typedef QHash<uint32_t, QVariant> ValuesCache; //CachedValue

    typedef struct _S_UpdateCmd {
        uint16_t index;
        uint8_t subindex;
        bool isWrite;
        COValue::Type type;
        uint32_t data;
    } UpdateCmd;

    typedef QQueue<UpdateCmd> UpdateQueue;

    ValuesCache* m_cache;
    UpdateQueue* m_queue;
};

#endif // REGSVIEWMODEL_H
