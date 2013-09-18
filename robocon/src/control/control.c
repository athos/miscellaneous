
#include "../robot/robot.h"
#include "../strategy/controlval.h"
#include "./control.h"

void control( void )
{
    motorval val;

    val = getControlValue();

    Motor_rotate( &(robot.leftMotor)  , val.leftmotor);
    Motor_rotate( &(robot.rightMotor) , val.rightmotor);
    Motor_rotate( &(robot.tailMotor)  , val.tailmotor);
}
