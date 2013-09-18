/****************************************************
 *  Motor.c                                         
 *  Created on: 2012/05/05 7:35:26                      
 *  Implementation of the Class Motor       
 *  Original author: hiroyuki.watanabe                     
 ****************************************************/

#include "Motor.h"


// ‰Šú‰»‚·‚é
void Motor_init(Motor* this, MOTOR_PORT_T outputPort)
{
	this->outputPort = outputPort;
    this->motorAngleLast = 0;
}

// ‰ñ“]Šp“x‚ðƒŠƒZƒbƒg‚·‚é
void Motor_resetAngle(Motor* this)
{
	return nxt_motor_set_count(this->outputPort, 0);
}

// ‰ñ“]Šp“x‚ð“¾‚é
long Motor_getAngle(Motor* this)
{
	return (nxt_motor_get_count(this->outputPort));
}

// ‰ñ“]‚³‚¹‚é
void Motor_rotate(Motor* this, int pwm)
{
	nxt_motor_set_speed(this->outputPort, pwm, 1);
} 


long Motor_getSpeed(Motor* this)
{
    long motorAngle;

    motorAngle = Motor_getAngle(this);
    this->motorSpeed = motorAngle - this->motorAngleLast;
    this->motorAngleLast = motorAngle;
    
    return ( this->motorSpeed );
}

long Motor_getLastSpeed(Motor* this)
{
    return ( this->motorSpeed );
}


