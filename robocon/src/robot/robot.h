
#ifndef ROBOT_H
#define ROBOT_H

#include "./device/internal.h"
#include "./device/sensor.h"
#include "./device/motor.h"
#include "./robot_state.h"

typedef struct _robot {

    SENSOR robotSensor;
    Motor  leftMotor;
    Motor  rightMotor;
    Motor  tailMotor;

    INTERNAL system;

} ROBOT;

void startrobot( ROBOT* this );
void stoprobot( ROBOT* this );

void initrobot( ROBOT* this );

void setTailAngle( unsigned char tailAngle );
void monitor( unsigned char data_log_buffer[] );

void updateRobotStatus( ROBOT* this, ROBOT_STATUS* this_status );

extern ROBOT robot;

extern unsigned char data_log_buffer[];
#endif
