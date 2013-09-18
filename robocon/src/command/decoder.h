#ifndef DECODER_H
#define DECODER_H

#include "command.h"
#include "byte_buffer.h"

typedef struct {
	int dc_buf_size;
	Condition *dc_cond_buf;
} Decoder;

extern Decoder *DECODER;

void initDecoder(Decoder *decoder);
void decodeCommand(Decoder *decoder, byte_t *data, Command *cmd);

#endif /* DECODER_H */
