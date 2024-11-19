#ifndef SLCAN_PORT_QT_H
#define SLCAN_PORT_QT_H

#include "slcan_port.h"


#if defined(__cplusplus)

class QSerialPort;

EXTERN QSerialPort* slcan_serial_getQSerialPort(slcan_serial_handle_t serial_port);

#endif // __cplusplus


#endif // SLCAN_PORT_QT_H
