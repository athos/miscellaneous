#include <stdio.h>
#include "byte_buffer.h"

void initByteBuffer(ByteBuffer *buffer, byte_t *data, int size) {
  buffer->pos = 0;
  buffer->size = size;
  buffer->data = data;
}

#define BYTE_AT(x, i) ((byte_t)((x) >> ((3-(i))*8) & 0x000000ff))

byte_t getByte(ByteBuffer *buffer) {
  byte_t ret = buffer->data[buffer->pos];
  buffer->pos++;
  
  return ret;
}

int getInt(ByteBuffer *buffer) {
  int ret = 0;
  int i;

  for (i = 0; i < 4; i++) {
    ret = (ret << 8) | buffer->data[buffer->pos+i];
  }
  buffer->pos += 4;

  return ret;
}

void putByte(ByteBuffer *buffer, byte_t value) {
  buffer->data[buffer->pos] = value;
  buffer->pos++;
}

void putInt(ByteBuffer *buffer, int value) {
  int i;

  for (i = 0; i < 4; i++) {
    buffer->data[buffer->pos+i] = BYTE_AT(value, i);
  }
  buffer->pos += 4;
}

void putBytes(ByteBuffer *buffer, byte_t *bytes, int size) {
  int i;

  for (i = 0; i < size; i++) {
    buffer->data[i] = bytes[i];
  }
  buffer->pos += size;
}
