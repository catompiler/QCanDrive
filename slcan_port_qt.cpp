#include "slcan_port_qt.h"
#include <time.h>
#include <QSerialPort>
#include <QDebug>


int slcan_clock_gettime (struct timespec *tp)
{
    return clock_gettime(CLOCK_MONOTONIC, tp);
}


// Преобразует тип последовательного порта в slcan_serial_handle_t.
#define SERIAL_TO_HANDLE(S) (reinterpret_cast<void*>(S))
// Преобразует slcan_serial_handle_t в тип последовательного порта.
#define HANDLE_TO_SERIAL(H) (reinterpret_cast<QSerialPort*>(H))



QSerialPort* slcan_serial_getQSerialPort(slcan_serial_handle_t serial_port)
{
    if(serial_port == SLCAN_IO_INVALID_HANDLE) return nullptr;

    return HANDLE_TO_SERIAL(serial_port);
}


int slcan_serial_open(const char* serial_port_name, slcan_serial_handle_t* serial_port)
{
    if(serial_port == NULL) return SLCAN_IO_FAIL;

    auto port = new QSerialPort();
    port->setPortName(serial_port_name);

    *serial_port = SERIAL_TO_HANDLE(port);

    return SLCAN_IO_SUCCESS;
}

int slcan_serial_configure(slcan_serial_handle_t serial_port, const slcan_port_conf_t* conf)
{
    static const unsigned int slcan_port_bauds_impl[] = {
        230400,
        115200,
        57600,
        38400,
        19200,
        9600,
        2400,
    };

    static const unsigned int bauds_count = sizeof(slcan_port_bauds_impl) / sizeof(slcan_port_bauds_impl[0]);

    if(conf->baud >= bauds_count){
        return SLCAN_IO_FAIL;
    }

    if(serial_port == SLCAN_IO_INVALID_HANDLE) return SLCAN_IO_FAIL;
    auto port = HANDLE_TO_SERIAL(serial_port);
    if(port == nullptr) return SLCAN_IO_FAIL;

    if(port->isOpen()){
        port->close();
    }

    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setBaudRate(slcan_port_bauds_impl[conf->baud]);
    port->setDataBits(QSerialPort::Data8);
    if(conf->stop_bits == SLCAN_PORT_STOP_BITS_2){
        port->setStopBits(QSerialPort::TwoStop);
    }else{
        port->setStopBits(QSerialPort::OneStop);
    }
    switch(conf->parity){
    default:
    case SLCAN_PORT_PARITY_NONE:
        port->setParity(QSerialPort::NoParity);
        break;
    case SLCAN_PORT_PARITY_EVEN:
        port->setParity(QSerialPort::EvenParity);
        break;
    case SLCAN_PORT_PARITY_ODD:
        port->setParity(QSerialPort::OddParity);
        break;
    }

    if(!port->open(QIODevice::ReadWrite)){
        return SLCAN_IO_FAIL;
    }

    return SLCAN_IO_SUCCESS;
}

void slcan_serial_close(slcan_serial_handle_t serial_port)
{
    if(serial_port == SLCAN_IO_INVALID_HANDLE) return;

    auto port = HANDLE_TO_SERIAL(serial_port);
    if(port == nullptr) return;

    port->close();
    port->deleteLater();
}

int slcan_serial_read(slcan_serial_handle_t serial_port, void* data, size_t data_size)
{
    if(serial_port == SLCAN_IO_INVALID_HANDLE) return SLCAN_IO_FAIL;
    auto port = HANDLE_TO_SERIAL(serial_port);
    if(port == nullptr) return SLCAN_IO_FAIL;

    return static_cast<int>(port->read(static_cast<char*>(data), data_size));
}

int slcan_serial_write(slcan_serial_handle_t serial_port, const void* data, size_t data_size)
{
    if(serial_port == SLCAN_IO_INVALID_HANDLE) return SLCAN_IO_FAIL;
    auto port = HANDLE_TO_SERIAL(serial_port);
    if(port == nullptr) return SLCAN_IO_FAIL;

    return static_cast<int>(port->write(static_cast<const char*>(data), data_size));
}

int slcan_serial_flush(slcan_serial_handle_t serial_port)
{
    if(serial_port == SLCAN_IO_INVALID_HANDLE) return SLCAN_IO_FAIL;
    auto port = HANDLE_TO_SERIAL(serial_port);
    if(port == nullptr) return SLCAN_IO_FAIL;

    port->waitForReadyRead(1);
    if(port->flush()) port->waitForBytesWritten(1);

    return SLCAN_IO_SUCCESS;
}

// POLLIN
// POLLPRI
// POLLOUT
// POLLERR
// POLLHUP
// POLLNVAL
int slcan_serial_poll(slcan_serial_handle_t serial_port, int events, int* revents, int timeout)
{
    Q_UNUSED(timeout);

    if(revents == NULL) return SLCAN_IO_FAIL;

    if(serial_port == SLCAN_IO_INVALID_HANDLE) return SLCAN_IO_FAIL;
    auto port = HANDLE_TO_SERIAL(serial_port);
    if(port == nullptr) return SLCAN_IO_FAIL;

    short out_events = 0;

    qint64 bytesAvail = port->bytesAvailable();
    qint64 bytesToWrite = port->bytesToWrite();

    //qDebug() << "bytesAvail" << bytesAvail;
    //qDebug() << "bytesToWrite" << bytesToWrite;

    if((events & SLCAN_POLLIN) && (bytesAvail != 0)) out_events |= SLCAN_POLLIN;
    if((events & SLCAN_POLLOUT) && (bytesToWrite == 0)) out_events |= SLCAN_POLLOUT;

    *revents = out_events;

    return SLCAN_IO_SUCCESS;
}

int slcan_serial_nbytes(slcan_serial_handle_t serial_port, size_t* size)
{
    if(size == NULL) return SLCAN_IO_FAIL;

    if(serial_port == SLCAN_IO_INVALID_HANDLE) return SLCAN_IO_FAIL;
    auto port = HANDLE_TO_SERIAL(serial_port);
    if(port == nullptr) return SLCAN_IO_FAIL;

    *size = (size_t)port->bytesAvailable();

    return SLCAN_IO_SUCCESS;
}
