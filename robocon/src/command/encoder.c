#include "encoder.h"

static Encoder defaultEncoder = {0};
Encoder *ENCODER = &defaultEncoder;

void initEncoder(Encoder *this) {

}

static void encodeCondition(Condition *cond, ByteBuffer *buffer) {
  putInt(buffer, cond->cond_type);
  if ((cond->cond_type == AND_COND_TYPE) || (cond->cond_type == OR_COND_TYPE )) {
    encodeCondition(LEFT_SUBCOND(cond), buffer);
    encodeCondition(RIGHT_SUBCOND(cond), buffer);
  } else {
    putInt(buffer, COND_VALUE(cond));
  }
}

void encodeCommand(Encoder *this, Command *cmd, byte_t *data, int size) {
  ByteBuffer buffer;

  initByteBuffer(&buffer, data, size);
  putByte(&buffer, (byte_t)0);
  putInt(&buffer, cmd->cmd_id);
  putInt(&buffer, cmd->cmd_type);
  putInt(&buffer, cmd->cmd_value);
  encodeCondition(cmd->cmd_cond, &buffer);

  *data = (byte_t)buffer.pos;
}
