
#include "../robot/robot.h"
#include "./controlval.h"

#include "balancer.h"
#include "./soukou_touritsu.h"

#define P_GAIN             2.5F /* ���S��~�p���[�^������W�� */
#define PWM_ABS_MAX          50 /* ���S��~�p���[�^����PWM��΍ő�l */

typedef float f4;
typedef char  s1;
void setMotorVal( signed char , signed char , signed char );
s1 tailControl( char tailAngletarget , char tailAngle );
static unsigned char u1s_2msflipcount;
static unsigned char u1s_judge = 0;
static f4 f4s_last_yp=0;
static int s4s_distance = 0;
static unsigned char u1s_tail = 0;
static f4 f4s_forward = 70;


/* ������ �ȉ��̓T���v�����甲���o�����l ������ */
f4  A_D = 0.8F;      /* ���[�p�X�t�B���^�W��(���E�ԗւ̕��ω�]�p�x�p) */
f4  A_R = 0.996F;    /* ���[�p�X�t�B���^�W��(���E�ԗւ̖ڕW���ω�]�p�x�p) */

/* ��ԃt�B�[�h�o�b�N�W��     */
/* K_F[0]: �ԗ։�]�p�x�W��   */
/* K_F[1]: �ԑ̌X�Ίp�x�W��   */
/* K_F[2]: �ԗ։�]�p���x�W�� */
/* K_F[3]: �ԑ̌X�Ίp���x�W�� */
f4   K_F[4] = {-0.870303F, -31.9978F, -1.1566F, -2.78873F};
f4   K_I    = -0.44721F;    /* �T�[�{����p�ϕ��t�B�[�h�o�b�N�W�� */

f4   K_PHIDOT   = 25.0F*2.5F;    /* �ԑ̖ڕW����p���x�W�� */
f4   K_THETADOT = 7.5F;          /* ���[�^�ڕW��]�p���x�W�� */

const  f4   BATTERY_GAIN   = 0.001089F;    /* PWM�o�͎Z�o�p�o�b�e���d���␳�W�� */
const  f4   BATTERY_OFFSET = 0.625F;       /* PWM�o�͎Z�o�p�o�b�e���d���␳�I�t�Z�b�g */
/* ������ �ȉ��̓T���v�����甲���o�����l ������ */
#define  GRAY_WHITE     (( ( robot.robotSensor.LIGHT.white  + robot.robotSensor.LIGHT.black ) / 2 + robot.robotSensor.LIGHT.white ) /2)
#define  TRACE_P   0.65
#define  TRACE_D   0.08

#define  FORWARD_MIN  70
#define  FORWARD_MIN_SHORT  20
#define  FORWARD_MAX  120


void setSoukouTouritsu( Command* cmd , ROBOT_STATUS* status )
{
    signed char left;
    signed char right;
    signed char tail;
    f4 f4t_yp;
    f4 f4t_yd;
    f4 f4t_turn;
    f4 f4t_forward = 0;
    int s4t_distance;
    int s4t_disdiff;
    unsigned short u2t_light;
    f4 f4t_min;
    
    
    tail = 0;
    /* user logic start */
    u1s_2msflipcount = 1 - u1s_2msflipcount;

    if ( u1s_2msflipcount == 1 )
    {
        f4t_yp = (f4)(GRAY_WHITE - status->lightval );
        f4t_yd = (f4t_yp - f4s_last_yp) / 0.004;
        f4s_last_yp = f4t_yp;
        f4t_turn = (f4)(TRACE_P * f4t_yp) + (f4)(TRACE_D * f4t_yd);

        s4t_distance = ( status->motorangle_L + status->motorangle_R ) >> 1;
        f4t_forward = f4s_forward;
//        f4t_forward += 0.1;

        if ( f4t_forward < FORWARD_MIN )
        {
            f4t_min = FORWARD_MIN_SHORT;
        }
        else
        {
            f4t_min = FORWARD_MIN;
        }

        if ( s4t_distance < 2000 )
        {
            f4t_forward += 0.1;
        }
        else
        {
			if ( s4t_distance < 4200)
			{
				f4t_forward -= 0.1;
			}
			else
			{
                if ( ( s4t_distance > 7300 )
                  && ( u1s_judge == 0 ) )
                {
				    s4s_distance = s4t_distance;
				    u1s_judge = 1;
				}
				else
				{
					if ( u1s_judge == 2 )
					{
	                    f4t_forward += 0.15;
	                }
	                else
	                {
	                    f4t_forward += 0.1;
					}
				}
			}
        }

        if ( u1s_judge == 1 )
        {
			f4t_forward -= 0.2;
			f4t_min = FORWARD_MIN_SHORT;
			f4t_turn = 32;
			s4t_disdiff = (int)(s4t_distance - s4s_distance);
			u2t_light = status->lightval;

            if( s4t_disdiff > 700)
			{
				f4t_turn = 5;
			}
			if( ( s4t_disdiff > 750 )
             && ( u2t_light > ( GRAY_WHITE + 10 ) ) )
            {
		         u1s_judge = 2;
		    }
		}
		
		if( s4t_distance > 13000 )
		{
			f4t_forward -= 0.3;
            f4t_min = 0;
		}

        if ( f4t_forward > FORWARD_MAX )
        {
			f4t_forward = FORWARD_MAX;
		}
		if ( f4t_forward < f4t_min )
		{
			f4t_forward = f4t_min;
		}

        f4s_forward = f4t_forward;
 
        balance_control(
           (f4)f4t_forward,                          /* �O��i����(+:�O�i, -:��i) */
           (f4)-f4t_turn,                    /* ���񖽗�(+:�E����, -:������) */
           (f4)status->gyroval,             /* �W���C���Z���T�l */
           (f4)status->gyro_offset,         /* �W���C���Z���T�I�t�Z�b�g�l */
           (f4)status->motorangle_L,        /* �����[�^��]�p�x[deg] */
           (f4)status->motorangle_R,        /* �E���[�^��]�p�x[deg] */
           (f4)status->batteryval,          /* �o�b�e���d��[mV] */
            &left,                           /* �����[�^PWM�o�͒l */
            &right                           /* �E���[�^PWM�o�͒l */
            );

		tail = tailControl( 30 , status->motorangle_T );

        setMotorVal( left , right , tail );

       *((unsigned short *)(&data_log_buffer[  20])) = (short)s4t_distance - s4s_distance;
       *((unsigned short *)(&data_log_buffer[  22])) = (short)status->lightval;
       *((unsigned short *)(&data_log_buffer[  24])) = (short)s4t_distance;
    }
    /* user logic end */
}

s1
tailControl( char tailAngletarget , char tailAngle )
{
    float pwm = (float)(tailAngletarget - tailAngle) * P_GAIN; /* ��ᐧ�� */
    /* PWM�o�͖O�a���� */
    if (pwm > PWM_ABS_MAX)
    {
        pwm = PWM_ABS_MAX;
    }
    else if (pwm < -PWM_ABS_MAX)
    {
        pwm = -PWM_ABS_MAX;
    }
    
   return (s1)pwm;
}

void setMotorVal( signed char left , signed char right , signed char tail)
{
    motorvalue.leftmotor = left;
    motorvalue.rightmotor = right;
    motorvalue.tailmotor = tail;
}
