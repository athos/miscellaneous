#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "byte_buffer.h"

int receiveData(byte_t *buf, int buf_size);
int sendData(byte_t *buf);

#endif /* BLUETOOTH_H */
