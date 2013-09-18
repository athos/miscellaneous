#ifndef BYTE_BUFFER
#define BYTE_BUFFER

typedef unsigned char byte_t;

typedef struct {
  int pos;
  int size;
  byte_t *data;
} ByteBuffer;

void initByteBuffer(ByteBuffer *buffer, byte_t *data, int size);
byte_t getByte(ByteBuffer *buffer);
int getInt(ByteBuffer *buffer);
void putByte(ByteBuffer *buffer, byte_t value);
void putInt(ByteBuffer *buffer, int value);
void putBytes(ByteBuffer *buffer, byte_t *bytes, int size);

#endif
