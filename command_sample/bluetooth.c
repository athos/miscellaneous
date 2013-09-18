#include "bluetooth.h"
#include "ecrobot_interface.h"

/* バッファリングをもうちょっとまじめにやった
   方がいいけど、現状問題になっていないので
   単にBluetoothのAPIを呼び出すだけにしている。
   
   もうちょっとまじめなバッファリング：
   - コマンドの一部のバイト列だけを受信して
     しまった場合に対応する
   - 複数のコマンドを連続して受信してしまった
     場合に対応する
*/
int receiveData(byte_t *buf, int buf_size) {
  return ecrobot_read_bt(buf, 0, buf_size);
}

int sendData(byte_t *buf) {
  return ecrobot_send_bt(buf, 0, (int)buf[0]);
}
