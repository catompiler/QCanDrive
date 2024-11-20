#ifndef SDOVALUE_H
#define SDOVALUE_H

#include <QObject>
#include <stddef.h>
#include "sdocommunication.h"


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
    bool setDataSize(size_t newDataSize);

    SDOCommunication::NodeId nodeId() const;
    bool setNodeId(SDOCommunication::NodeId newNodeId);

    SDOCommunication::Index index() const;
    bool setIndex(SDOCommunication::Index newIndex);

    SDOCommunication::SubIndex subIndex() const;
    bool setSubIndex(SDOCommunication::SubIndex newSubIndex);

    int timeout() const;
    bool setTimeout(int newTimeout);

    SDOCommunication::Error error() const;

    bool running() const;

    template <typename T>
    T value(const T& defVal = T(), bool* isOk = nullptr) const;

    template <typename T>
    bool setValue(const T& val);

    /*template <typename T>
    T valueAt(int i, const T& defVal = T(), bool* isOk = nullptr) const;

    template <typename T>
    bool setValueAt(int i, const T& val);*/

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
    SDOCommunication* m_sdoc;
    SLCanOpenNode* m_slcon;
};

template<typename T>
T SDOValue::value(const T& defVal, bool* isOk) const
{
    if(m_sdoc->data() == nullptr)
    { if(isOk) *isOk = false; return defVal; }
    if(m_sdoc->size() != sizeof(T))
    { if(isOk) *isOk = false; return defVal; }

    return *static_cast<T*>(m_sdoc->data());
}

template<typename T>
bool SDOValue::setValue(const T& val)
{
    if(m_sdoc->data() == nullptr)
    { return false; }
    if(m_sdoc->size() != sizeof(T))
    { return false; }

    *static_cast<T*>(m_sdoc->data()) = val;

    return true;
}

/*template<typename T>
T SDOValue::valueAt(int i, const T& defVal, bool* isOk) const
{
    if(m_sdoc->data() == nullptr)
    { if(isOk) *isOk = false; return defVal; }
    if((i < 0) || (m_sdoc->size() < (i + 1) * sizeof(T)))
    { if(isOk) *isOk = false; return defVal; }

    return static_cast<T*>(m_sdoc->data())[i];
}

template<typename T>
bool SDOValue::setValueAt(int i, const T& val)
{
    if(m_sdoc->data() == nullptr)
    { return false; }
    if((i < 0) || (m_sdoc->size() < (i + 1) * sizeof(T)))
    { return false; }

    static_cast<T*>(m_sdoc->data())[i] = val;

    return true;
}*/

#endif // SDOVALUE_H
