#include "decoder.h"
#include <stdio.h>

#define COND_BUF_SIZE 8

static Condition cond_buf[COND_BUF_SIZE];
static Decoder defaultDecoder = {COND_BUF_SIZE, cond_buf};
Decoder *DECODER = &defaultDecoder;

void initDecoder(Decoder *this) {

}

static Condition *decodeCondition(ByteBuffer *buffer, Condition **conds) {
  Condition *ret = *conds;
  ++*conds;

  ret->cond_type = getInt(buffer);
  if ((ret->cond_type == AND_COND_TYPE) || (ret->cond_type == OR_COND_TYPE)) {
    LEFT_SUBCOND(ret) = decodeCondition(buffer, conds);
    RIGHT_SUBCOND(ret) = decodeCondition(buffer, conds);
  } else {
    COND_VALUE(ret) = getInt(buffer);
    ++*conds;
  }
  return ret;
}

void decodeCommand(Decoder *this, byte_t *data, Command *cmd) {
  ByteBuffer buffer;
  int cmd_id, cmd_type, cmd_value;
  int size = (int)data[0];
  Condition *cmd_cond;
  Condition *cond_buf = this->dc_cond_buf;

  initByteBuffer(&buffer, data, size);

  getByte(&buffer); /* omit command size */
  cmd_id = getInt(&buffer);
  cmd_type = getInt(&buffer);
  cmd_value = getInt(&buffer);
  cmd_cond = decodeCondition(&buffer, &cond_buf);

  initCommand(cmd, cmd_id, cmd_type, cmd_value, cmd_cond);
}
