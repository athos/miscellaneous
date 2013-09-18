#include <stdio.h>
#include "encoder.h"
#include "decoder.h"
#include "command.h"

Condition test_condition1 = {
  0
};

Condition test_condition2 = {
  3, 300
};

Condition test_condition3 = {
  4, 400
};

Command test_command = {
  42, 100, 200, &test_condition1
};

void indent(int depth) {
  int i;

  for (i = 0; i < depth; i++) {
    printf(" ");
  }
}

void printCondition(Condition *cond, int depth) {
  indent(depth);
  printf("cond_type: %d\n", cond->cond_type);
  if ((cond->cond_type == AND_COND_TYPE) || (cond->cond_type == OR_COND_TYPE)) {
    indent(depth);
    printf("left subcond:\n"); 
    printCondition(LEFT_SUBCOND(cond), depth+2);
    indent(depth);
    printf("right subcond:\n");
    printCondition(RIGHT_SUBCOND(cond), depth+2);
  } else {
    indent(depth);
    printf("cond_value: %d\n", COND_VALUE(cond));
  }
}

void printCommand(Command *this) {
  printf("cmd_id: %d\n", this->cmd_id);
  printf("cmd_type: %d\n", this->cmd_type);
  printf("cmd_value: %d\n", this->cmd_value);
  printCondition(this->cmd_cond, 0);
}

int main() {
  byte data_buf[64];
  Command cmd;
  int i;

  LEFT_SUBCOND(&test_condition1) = &test_condition2;
  RIGHT_SUBCOND(&test_condition1) = &test_condition3;

  printCommand(&test_command);

  puts("========== ENCODING ==========");  
  encodeCommand(ENCODER, &test_command, data_buf, sizeof(data_buf));

  printf("data_buf: ");
  for (i = 0; i < data_buf[0]; i++) {
    printf("%02x ", data_buf[i]);
  }
  printf("\n");

  puts("========== DECODING ==========");
  decodeCommand(DECODER, data_buf, &cmd);

  printCommand(&cmd);
  
  return 0;
}
