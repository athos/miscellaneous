#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "ecrobot_interface.h"
#include "byte_buffer.h"

int receiveData(byte_t *data);
int sendData(byte_t *data);

#endif /* BLUETOOTH_H */
