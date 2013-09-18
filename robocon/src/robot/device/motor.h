
#ifndef MOTOR_H
#define MOTOR_H

#include "ecrobot_interface.h"
#include "kernel.h"

// ������ێ����邽�߂̍\���̂̒�`
typedef struct Motor
{
	MOTOR_PORT_T outputPort;
    long         motorAngleLast;
    long         motorSpeed;
    float        motorSpeedOut;
    long         motorSpeedIntegrator;
} Motor;

// ���J����
void Motor_init(Motor* this, MOTOR_PORT_T outputPort);
void Motor_resetAngle(Motor* this);
long Motor_getAngle(Motor* this);
void Motor_rotate(Motor* this, int pwm);

long Motor_getSpeed(Motor* this);
long Motor_getLastSpeed(Motor* this);

#endif
