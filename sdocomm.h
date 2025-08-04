#ifndef SDOCOMM_H
#define SDOCOMM_H

#include <QObject>
#include "cotypes.h"
#include "sdocomm_data.h"

//struct SDOComm_data;


class SDOComm : public QObject
{
    Q_OBJECT
public:

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
        ERROR_NONE          = 0,
        ERROR_IO            = 1,
        ERROR_TIMEOUT       = 2,
        ERROR_CANCEL        = 3,
        ERROR_INVALID_SIZE  = 4,
        ERROR_INVALID_VALUE = 5,
        ERROR_ACCESS        = 6,
        ERROR_NOT_FOUND     = 7,
        ERROR_NO_DATA       = 8,
        ERROR_OUT_OF_MEM    = 9,
        ERROR_GENERAL       = 10,
        ERROR_UNKNOWN       = 11
    };

    explicit SDOComm(QObject *parent = nullptr);
    ~SDOComm();

    // get/set.

    Type type() const;
    void setType(Type newType);

    CO::NodeId nodeId() const;
    void setNodeId(CO::NodeId newNodeId);

    CO::Index index() const;
    void setIndex(CO::Index newIndex);

    CO::SubIndex subIndex() const;
    void setSubIndex(CO::SubIndex newSubIndex);

    void* data();
    const void* data() const;
    void setData(void* newData);

    size_t dataSize() const;
    void setDataSize(size_t newSize);

    // ms.
    int timeout() const;
    void setTimeout(int newTimeout);

    State state() const;
    void setState(State newState);

    Error error() const;
    void setError(Error newError);

    bool cancelled() const;
    void setCancel(bool newCancel);

    size_t transferSize() const;
    void setTransferSize(size_t newTransferSize);

    // methods.

    void cancel();
    bool running() const;

    void finish();
    void finish(Error err);

    size_t transferedDataSize() const;
    void resetTransferedSize();
    void setDataTransfered(size_t size);
    void dataTransfered(size_t size);
    bool dataTransferDone() const;
    size_t dataSizeToTransfer() const;
    void* dataToTransfer() const;

    size_t bufferedDataSize() const;
    void resetBufferedSize();
    void setDataBuffered(size_t size);
    void dataBuffered(size_t size);
    bool dataBufferingDone() const;
    size_t dataSizeToBuffering() const;
    void* dataToBuffering() const;

signals:
    void finished();

protected:
    SDOComm_data* m_d;
};

#endif // SDOCOMM_H
