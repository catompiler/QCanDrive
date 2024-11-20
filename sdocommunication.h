#ifndef SDOCOMMUNICATION_H
#define SDOCOMMUNICATION_H

#include <QObject>

class SDOCommunication_data;


class SDOCommunication : public QObject
{
    Q_OBJECT
public:

    using NodeId = quint8;
    using Index = quint16;;
    using SubIndex = quint8;

    enum Type {
        NONE = 0,
        UPLOAD = 1,
        DOWNLOAD = 2
    };

    enum State {
        IDLE = 0,
        QUEUED = 1,
        INIT = 2,
        DATA = 3,
        RUN = 4,
        DONE = 5
    };

    enum Error {
        ERROR_NONE = 0,
        ERROR_IO = 1,
        ERROR_TIMEOUT = 2,
        ERROR_CANCEL = 3,
        ERROR_INVALID_SIZE = 4
    };

    explicit SDOCommunication(QObject *parent = nullptr);
    /*SDOCommunication(const SDOCommunication& sdo_comm);
    SDOCommunication(SDOCommunication&& sdo_comm);*/
    ~SDOCommunication();

    // get/set.

    Type type() const;
    void setType(Type newType);

    NodeId nodeId() const;
    void setNodeId(NodeId newNodeId);

    Index index() const;
    void setIndex(Index newIndex);

    SubIndex subIndex() const;
    void setSubIndex(SubIndex newSubIndex);

    void* data() const;
    void setData(void* newData);

    size_t size() const;
    void setSize(size_t newSize);

    // ms.
    int timeout() const;
    void setTimeout(int newTimeout);

    State state() const;
    void setState(State newState);

    Error error() const;
    void setError(Error newError);

    bool cancelled() const;
    void setCancel(bool newCancel);

    // methods.

    void cancel();
    bool running() const;

    void finish();
    void finish(Error err);

    void resetDataTransfered();
    void dataTransfered(size_t size);
    bool dataTransferDone() const;
    size_t dataSizeToTransfer() const;
    void* dataToTransfer() const;

signals:
    void finished();

protected:
    SDOCommunication_data* m_d;
};

#endif // SDOCOMMUNICATION_H
