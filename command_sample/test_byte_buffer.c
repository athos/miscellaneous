#include <stdio.h>
#include "byte_buffer.h"

int main() {
  byte data[32];
  ByteBuffer buffer1;
  ByteBuffer buffer2;

  byte v1;
  int v2, v3;
  
  initByteBuffer(&buffer1, data, sizeof(data));

  putByte(&buffer1, (byte)42);
  putInt(&buffer1, 12345);
  putInt(&buffer1, 54321);
  
  initByteBuffer(&buffer2, data, sizeof(data));

  v1 = getByte(&buffer2);
  v2 = getInt(&buffer2);
  v3 = getInt(&buffer2);

  printf("v1: %d\n", v1);
  printf("v2: %d\n", v2);
  printf("v3: %d\n", v3);
  
  return 0;
}
