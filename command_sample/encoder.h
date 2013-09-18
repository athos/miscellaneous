#ifndef ENCODER_H
#define ENCODER_H

#include "command.h"
#include "byte_buffer.h"

typedef struct {
  int dummy;
} Encoder;

extern Encoder *ENCODER;

void initEncoder(Encoder *encoder);
void encodeCommand(Encoder *encoder, Command *cmd, byte_t *data, int size);

#endif /* ENCODER_H */
