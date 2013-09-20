
#include "../robot/robot.h"
#include "./controlval.h"

#include "balancer.h"
#include "./performancepart2.h"

#define P_GAIN             2.5F /* ���S��~�p���[�^������W�� */
#define PWM_ABS_MAX          50 /* ���S��~�p���[�^����PWM��΍ő�l */

#define SPEEDBUFFSIZE        10
typedef float f4;
typedef char  s1;
void setMotorVal( signed char , signed char , signed char );
s1 tailControl( char tailAngletarget , char tailAngle );
static unsigned char u1s_2msflipcount;
static unsigned char u1s_judge = 0;
static f4 f4s_last_yp = 0;
static unsigned int u4s_distance = 0;
static unsigned char u1s_tail = 0;
static f4 f4s_forward = 0;
static unsigned int u4s_count = 0;
static signed int s4s_speed[SPEEDBUFFSIZE] = {8};
static unsigned short u2s_speeddata_pt = 0;
static unsigned char u1s_count = 0;
static unsigned char u1s_init = 15;

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

void setPerformanceStagePart2( Command* cmd , ROBOT_STATUS* status )
{
    signed char left;
    signed char right;
    signed char tail;
    f4 f4t_yp;
    f4 f4t_yd;
    f4 f4t_turn;
    f4 f4t_forward = 0;
    int s4t_speed_data_sum;
    int i;
    unsigned char u1t_do;
    short s2t_diff;
    unsigned char u1t_dir;

    tail = 0;
    u1t_do = 0;
    /* user logic start */
    u1s_2msflipcount = 1 - u1s_2msflipcount;                    /* �v�Z������4ms�����ɕύX���� */

    s4s_speed[u2s_speeddata_pt] = status->motorSpeed;

    u2s_speeddata_pt = u2s_speeddata_pt + 1;
    if ( u2s_speeddata_pt >= SPEEDBUFFSIZE )
    {
        u2s_speeddata_pt = 0;
    }

    s4t_speed_data_sum = 0;
    for( i = 0; i < SPEEDBUFFSIZE; i++ )
    {
        s4t_speed_data_sum += s4s_speed[i];
    }

    s2t_diff = s4t_speed_data_sum - 7;

    if ( u1s_2msflipcount == 1 )
    {
        if( u1s_init > 0 )
        {
            u1s_init--;
        }

        if ( ( ( status->lightval > GRAY_WHITE - 30 )             /* �J�[�u�i�������Z���T�ω������o */  /* ���Z���T�ŏ���肪����ƁA������~�ɂȂ����I�I�I*/
            && ( u1s_judge == 0 ))                                /* �J�[�u�ʉ߂����ł͂Ȃ� */
          || ( u1s_init > 0 ) )                                  /* �����s�����ԉ��΍� */
        {
            f4t_yp = (f4)(GRAY_WHITE - status->lightval );
            f4t_yd = (f4t_yp - f4s_last_yp) / 0.004;
            f4s_last_yp = f4t_yp;
            f4t_turn = (f4)(TRACE_P * f4t_yp) + (f4)(TRACE_D * f4t_yd);   /* �|����PD���� */

            u4s_distance = status->motorangle_L + status->motorangle_R;   /* �Ō�̋��������b�` */
            f4t_forward = 50 - s2t_diff*3;                                  /* �O�i�̃X�s�[�h���Œ肷�� */
            
            if ( u1s_tail == 1)                                           /* ���C�����A���� */
            {
                f4s_forward -= 0.1;                                       /* ���� */
                if ( f4s_forward <= 0 )                                   /* 0�K�[�h */
				{
					f4s_forward = 0;
				}
				f4t_forward = f4s_forward;

                if ( f4t_forward == 0 )
                {
					u1s_tail = 2;
				}
			}
		}
		else
		{
            /* �i�����x���v�����A�␳���� */
            if ( ( u1s_judge == 0 )
              && ( s4t_speed_data_sum >= 6 ) )
            {
                u1t_do = 1;
            }
            else
            {
                u1s_count++;
            }

            if ( ( u1t_do == 1)
              || ( u1s_count > 10 ) )
            {
                f4t_turn = 6;                                               /* �J�[�u���̐���� 6.2-6.4 */
                u1s_judge = 1;                                                /* �J�[�u�ʉߒ� */
            }
            else
            {
                f4t_turn = 0;                                                 /* �J�[�u���̐���� */
            }

            f4t_forward = 40;                                             /* �J�[�u�ʉߎ��̑O�i�� */

            /* ���C�����A */
            if( ( status->motorangle_L + status->motorangle_R - u4s_distance ) > 1100 )  /* �w�苗���ȏ�O�i���� */
            {
                if ( (status->motorangle_L + status->motorangle_R - u4s_distance ) < 1400)
                {
                    f4t_turn = -20;
                    f4t_forward = 30;
                    u1t_dir = 1;      /* right */
                }
                else
                {
                    f4t_turn = 15;
                    f4t_forward = 40;
                    u1t_dir = 2;      /* left */
                }

                if( status->lightval > GRAY_WHITE + 10 )
			    {
                    if( u1t_dir == 2 ) 
                    {
                        u1s_judge = 0;                                            /* �|���g���[�X���� */
                        u1s_tail = 1;                                             /* ���C�����A���� */
                        f4s_forward = 30;                                         /* �O�i�X�s�[�g�𗎂Ƃ� */
                    }
                    if( u1t_dir == 1 ) 
                    {
                        if( ( status->lightval < GRAY_WHITE + 50 )
                         && ( status->lightraw < status->lightval ))
                        {
                            u1s_judge = 0;                                            /* �|���g���[�X���� */
                            u1s_tail = 1;                                             /* ���C�����A���� */
                            f4s_forward = 30;                                         /* �O�i�X�s�[�g�𗎂Ƃ� */
                        }
                    }
                }
			}
            /* ���C�����A */

#if 0  /* �ȈՃ��C�����A */
			if( ( ( status->motorangle_L + status->motorangle_R - u4s_distance ) > 1000 )
             && ( status->lightval > GRAY_WHITE - 10 ) )
			{
				u1s_judge = 0;
				u1s_tail = 1;
				f4s_forward = 30;
			}
#endif 
		}
		
        if (u1s_tail < 2)                        /* �|���� */
		{
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
            tail = tailControl( 30 , status->motorangle_T );   /* �K����30���ێ� */

        }
        else                                     /* �K����� */
        {
            u4s_count++;
            left = 0;                            /* ��~ */
			right = 0;
            if ( u4s_count < 20 )
        {
                left = 60;                            /* ��~ */
                right = 60;
        }
            tail = tailControl( 90 , status->motorangle_T );  /* �K����90���ێ� */
        }
        setMotorVal( left , right , tail );      /* �o�͂��� */
        
    /* �ȉ��̓��j�^�p */
    *((long *)(&data_log_buffer[8])) = u1s_judge;
    *((long *)(&data_log_buffer[12])) = status->motorSpeed;
    *((long *)(&data_log_buffer[16])) = s4t_speed_data_sum;
    /* �ȏ�̓��j�^�p */
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
