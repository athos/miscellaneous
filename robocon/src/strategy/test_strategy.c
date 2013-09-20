#include "test_strategy.h"
#include "controlval.h"
#include "ecrobot_interface.h"

void setMotorVal(signed char left, signed char right, signed char tail) {
  motorvalue.leftmotor = left;
  motorvalue.rightmotor = right;
  motorvalue.tailmotor = tail;
}

void test_strategy(Command *cmd, ROBOT_STATUS *status) {
  ecrobot_sound_tone(cmd->cmd_value, 100, 50);

  setMotorVal(50, 50, 95);
}
