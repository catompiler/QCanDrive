#ifndef SDOVALUE_H
#define SDOVALUE_H

#include <QObject>
#include <stddef.h>
#include "cotypes.h"
#include "sdocomm.h"


class SLCanOpenNode;



class SDOValue : public QObject
{
    Q_OBJECT
public:
    explicit SDOValue(QObject *parent = nullptr);
    explicit SDOValue(SLCanOpenNode* slcon, QObject *parent = nullptr);
    ~SDOValue();

    SLCanOpenNode* getSLCanOpenNode();
    bool setSLCanOpenNode(SLCanOpenNode* slcon);

    size_t dataSize() const;
    // already set transferSize to newDataSize, if needed.
    bool setDataSize(size_t newDataSize);

    CO::NodeId nodeId() const;
    bool setNodeId(CO::NodeId newNodeId);

    CO::Index index() const;
    bool setIndex(CO::Index newIndex);

    CO::SubIndex subIndex() const;
    bool setSubIndex(CO::SubIndex newSubIndex);

    int timeout() const;
    bool setTimeout(int newTimeout);

    void* data();
    const void* data() const;

    size_t copyDataFrom(const void* ptr, size_t sz);
    size_t copyDataTo(void* ptr, size_t sz) const;

    size_t transferSize() const;
    bool setTransferSize(size_t newTransferSize);

    size_t transferedDataSize() const;

    SDOComm::Type transferType() const;

    SDOComm::State transferState() const;

    SDOComm::Error error() const;

    bool running() const;

    template <typename T>
    T value(const T& defVal = T(), bool* isOk = nullptr) const;

    template <typename T>
    bool setValue(const T& val);

    template <typename T>
    T valueAt(int i, const T& defVal = T(), bool* isOk = nullptr) const;

    template <typename T>
    bool setValueAt(int i, const T& val);

public slots:
    bool read();
    bool write();

signals:
    void finished();
    void errorOccured();
    void canceled();
    void readed();
    void written();

protected slots:
    void sdocommFinished();

protected:
    SDOComm* m_sdoc;
    SLCanOpenNode* m_slcon;

    void deleteDataAsFinished();
};

template<typename T>
T SDOValue::value(const T& defVal, bool* isOk) const
{
    if(m_sdoc->data() == nullptr)
    { if(isOk) *isOk = false; return defVal; }
    if(m_sdoc->dataSize() != sizeof(T))
    { if(isOk) *isOk = false; return defVal; }

    return *static_cast<T*>(m_sdoc->data());
}

template<typename T>
bool SDOValue::setValue(const T& val)
{
    if(m_sdoc->data() == nullptr)
    { return false; }
    if(m_sdoc->dataSize() != sizeof(T))
    { return false; }

    *static_cast<T*>(m_sdoc->data()) = val;

    return true;
}

template<typename T>
T SDOValue::valueAt(int i, const T& defVal, bool* isOk) const
{
    if(m_sdoc->data() == nullptr)
    { if(isOk) *isOk = false; return defVal; }
    if((i < 0) || (m_sdoc->dataSize() < (i + 1) * sizeof(T)))
    { if(isOk) *isOk = false; return defVal; }

    return static_cast<T*>(m_sdoc->data())[i];
}

template<typename T>
bool SDOValue::setValueAt(int i, const T& val)
{
    if(m_sdoc->data() == nullptr)
    { return false; }
    if((i < 0) || (m_sdoc->dataSize() < (i + 1) * sizeof(T)))
    { return false; }

    static_cast<T*>(m_sdoc->data())[i] = val;

    return true;
}

#endif // SDOVALUE_H
