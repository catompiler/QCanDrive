#ifndef COVALUESHOLDER_H
#define COVALUESHOLDER_H

#include <QObject>
#include <stddef.h>
#include "cotypes.h"
#include "sdovalue.h"
#include <QMap>
#include <QPair>


class QTimer;
class SLCanOpenNode;


class CoValuesHolder : public QObject
{
    Q_OBJECT
public:

    using HoldedSDOValuePtr = const SDOValue*;

    explicit CoValuesHolder(SLCanOpenNode* slcon = nullptr, QObject *parent = nullptr);
    ~CoValuesHolder();

    SLCanOpenNode* getSLCanOpenNode();
    bool setSLCanOpenNode(SLCanOpenNode* slcon);

    bool updatingEnabled() const;
    bool setUpdatingEnabled(bool newUpdatingEnabled);

    int updateInterval() const;
    void setUpdateInterval(int newUpdateInterval);

    HoldedSDOValuePtr addSdoValue(CO::NodeId valNodeId, CO::Index valIndex, CO::SubIndex valSubIndex, size_t dataSize, int timeout = 0);
    void delSdoValue(HoldedSDOValuePtr delSdoVal);
    HoldedSDOValuePtr getSDOValue(CO::NodeId valNodeId, CO::Index valIndex, CO::SubIndex valSubIndex) const;

    template <typename T>
    T value(CO::NodeId valNodeId, CO::Index valIndex, CO::SubIndex valSubIndex,
            const T& defVal = T(), bool* isOk = nullptr) const;

public slots:
    void update();
    void enableUpdating();
    void disableUpdating();

private:
    typedef quint32 FullIndex;

    QMap<FullIndex, QPair<SDOValue*, size_t>> m_sdoValues;

    SLCanOpenNode* m_slcon;
    bool m_updatingEnabled;
    QTimer* m_updateTimer;

    FullIndex makeFullIndex(CO::NodeId valNodeId, CO::Index valIndex, CO::SubIndex valSubIndex) const;
};


#endif // COVALUESHOLDER_H
