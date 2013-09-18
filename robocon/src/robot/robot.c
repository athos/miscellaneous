
#include "./robot_state.h"
#include "balancer.h"
#include "../command/command_c.h"

#include "./robot.h"

#define TAIL_ANGLE           95
#define P_GAIN             2.5F /* ���S��~�p���[�^������W�� */
#define PWM_ABS_MAX          50 /* ���S��~�p���[�^����PWM��΍ő�l */
#define BT_PASS_KEY     "teamKGK"

unsigned char data_log_buffer[32];

static unsigned char robotTailAngle;
extern unsigned char data_log_buffer[];

void holdTailAngle( ROBOT* this ,unsigned char tailAngle );


void startrobot( ROBOT* this )
{
    startSensor( &(this->robotSensor) );

	Motor_init( &(this->leftMotor),  NXT_PORT_C);
	Motor_init( &(this->rightMotor), NXT_PORT_B);
	Motor_init( &(this->tailMotor),  NXT_PORT_A);

    ecrobot_init_bt_slave(BT_PASS_KEY);

    command_init();

    setTailAngle( TAIL_ANGLE );

}


void stoprobot( ROBOT* this )
{
    stopSensor( &(this->robotSensor) );
}

void initrobot( ROBOT* this )
{
    unsigned int u4t_start_time;

    u4t_start_time = getTimerValue();
    while ( (getTimerValue() - u4t_start_time) < 150U );
    while ( (getTimerValue() - u4t_start_time) < 1500U );

    /* �@�� */
    while ( ecrobot_get_touch_sensor(NXT_PORT_S4) == 0U );    /* �X�C�b�`������҂� */
    setWhiteValue( &((this->robotSensor).LIGHT)  );    /* ��1�����F����l, ��2�����F�Z���T��� (1:���A2:�W���C��) */
    display_goto_xy(0, 0);  display_unsigned( (this->robotSensor).LIGHT.white, 3 );
    display_update();
    
    /* �A�� */
    while ( ecrobot_get_touch_sensor(NXT_PORT_S4) == 0U );    /* �X�C�b�`������҂� */
    setBlackValue( &((this->robotSensor).LIGHT)  );    /* ��1�����F����l, ��2�����F�Z���T��� (1:���A2:�W���C��) */
    display_goto_xy(0, 1);  display_unsigned( (this->robotSensor).LIGHT.black, 3 );
    display_update();
#if 0
    /* �B�D */
    while ( ecrobot_get_touch_sensor(NXT_PORT_S4) == 0U );    /* �X�C�b�`������҂� */
    setGreyValue( &((this->robotSensor).LIGHT)  );    /* ��1�����F����l, ��2�����F�Z���T��� (1:���A2:�W���C��) */
    display_goto_xy(0, 2);  display_unsigned( (this->robotSensor).LIGHT.grey, 3 );
    display_update();
#endif
    
    /* �C�Q�����ăW���C�� */
    while ( ecrobot_get_touch_sensor(NXT_PORT_S4) == 0U );    /* �X�C�b�`������҂� */
    setGyroOffsetValue( &((this->robotSensor).GYRO)  );
    display_goto_xy(0, 3);  display_unsigned( (this->robotSensor).GYRO.gyroOffsetValue, 3 );
    display_update();
    ecrobot_sound_tone(1000, 100, 50);

    u4t_start_time = ecrobot_get_systick_ms();
    while ( (getTimerValue() - u4t_start_time) < 150U );

    while ( ecrobot_get_touch_sensor(NXT_PORT_S4) == 1U );    /* �X�C�b�`������҂� */

    /* �D�K���ݒ� */
    while ( ecrobot_get_touch_sensor(NXT_PORT_S4) == 0U );    /* �X�C�b�`������҂� */

    Motor_resetAngle( &(this->tailMotor) );
    Motor_resetAngle( &(this->leftMotor) );
    Motor_resetAngle( &(this->rightMotor) );
    balance_init();    /* balance API�̏����� */

    while( Motor_getAngle( &(this->tailMotor) ) < robotTailAngle )
    {
    	Motor_rotate( &(this->tailMotor) , 50 );
    }
    ecrobot_sound_tone(1000, 100, 50);
    holdTailAngle( this ,robotTailAngle );

    while ( ecrobot_get_touch_sensor(NXT_PORT_S4) == 1U );    /* �X�C�b�`������҂� */

    while ( ecrobot_get_touch_sensor(NXT_PORT_S4) == 0U )     /* �X�C�b�`������҂� */
    {
        holdTailAngle( this, robotTailAngle );
    }
    ecrobot_sound_tone(10000, 100, 50);
    
}




void setTailAngle( unsigned char tailAngle )
{
    robotTailAngle = tailAngle;
}


void holdTailAngle( ROBOT* this ,unsigned char tailAngle )
{
    float pwm = (float)(tailAngle - Motor_getAngle( &(this->tailMotor) )) * P_GAIN; /* ��ᐧ�� */
    /* PWM�o�͖O�a���� */
    if (pwm > PWM_ABS_MAX)
    {
        pwm = PWM_ABS_MAX;
    }
    else if (pwm < -PWM_ABS_MAX)
    {
        pwm = -PWM_ABS_MAX;
    }
    
    Motor_rotate( &(this->tailMotor) , (signed char)pwm );
}

void monitor( unsigned char data_log_buffer[] )
{
    ecrobot_send_bt_packet(data_log_buffer, 32);
}


void updateRobotStatus( ROBOT* this, ROBOT_STATUS* this_status )
{
    this_status->lightval     = getLightValue( &((this->robotSensor).LIGHT));
    this_status->gyroval      = getGyroValue( &(this->robotSensor.GYRO) );
    this_status->gyro_offset  = (this->robotSensor.GYRO).gyroOffsetValue;
    this_status->sonarval     = getSonarValue( &(this->robotSensor.SONAR) );

    this_status->motorangle_L = Motor_getAngle( &(this->leftMotor));
    this_status->motorangle_R = Motor_getAngle( &(this->rightMotor));
    this_status->motorangle_T = Motor_getAngle( &(this->tailMotor));

    this_status->motorSpeed =   Motor_getSpeed( &(this->leftMotor)) + Motor_getSpeed( &(this->rightMotor));

    this_status->batteryval   = getBatteryValue();
    this_status->nowtime      = getTimerValue();

}

