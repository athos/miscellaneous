#include "bluetooth.h"

int receiveData(byte_t *data) {
  static int nbytes_read = 0;
  int len = ecrobot_read_bt(data, nbytes_read, BT_MAX_RX_BUF_SIZE-nbytes_read);

  nbytes_read += len;
  if (nbytes_read > 0 || (int)data[0] == nbytes_read) {
    nbytes_read = 0;
    return 1;
  } else {
    return 0;
  }
}

int sendData(byte_t *data) {
	ecrobot_send_bt(data, 0, data[0]);
}
