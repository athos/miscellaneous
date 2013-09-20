#include "bluetooth.h"
#include "ecrobot_interface.h"

/* �o�b�t�@�����O������������Ƃ܂��߂ɂ����
   �����������ǁA������ɂȂ��Ă��Ȃ��̂�
   �P��Bluetooth��API���Ăяo�������ɂ��Ă���B
   
   ����������Ƃ܂��߂ȃo�b�t�@�����O�F
   - �R�}���h�̈ꕔ�̃o�C�g�񂾂�����M����
     ���܂����ꍇ�ɑΉ�����
   - �����̃R�}���h��A�����Ď�M���Ă��܂���
     �ꍇ�ɑΉ�����
*/
int receiveData(byte_t *buf, int buf_size) {
  return ecrobot_read_bt(buf, 0, buf_size);
}

int sendData(byte_t *buf) {
  return ecrobot_send_bt(buf, 0, (int)buf[0]);
}
