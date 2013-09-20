
#include "../robot/robot.h"
#include "./controlval.h"

#include "balancer.h"
#include "./performancepart2.h"

#define P_GAIN             2.5F /* 完全停止用モータ制御比例係数 */
#define PWM_ABS_MAX          50 /* 完全停止用モータ制御PWM絶対最大値 */

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

/* ▼▼▼ 以下はサンプルから抜き出した値 ▼▼▼ */
f4  A_D = 0.8F;      /* ローパスフィルタ係数(左右車輪の平均回転角度用) */
f4  A_R = 0.996F;    /* ローパスフィルタ係数(左右車輪の目標平均回転角度用) */

/* 状態フィードバック係数     */
/* K_F[0]: 車輪回転角度係数   */
/* K_F[1]: 車体傾斜角度係数   */
/* K_F[2]: 車輪回転角速度係数 */
/* K_F[3]: 車体傾斜角速度係数 */
f4   K_F[4] = {-0.870303F, -31.9978F, -1.1566F, -2.78873F};
f4   K_I    = -0.44721F;    /* サーボ制御用積分フィードバック係数 */

f4   K_PHIDOT   = 25.0F*2.5F;    /* 車体目標旋回角速度係数 */
f4   K_THETADOT = 7.5F;          /* モータ目標回転角速度係数 */

const  f4   BATTERY_GAIN   = 0.001089F;    /* PWM出力算出用バッテリ電圧補正係数 */
const  f4   BATTERY_OFFSET = 0.625F;       /* PWM出力算出用バッテリ電圧補正オフセット */
/* ▲▲▲ 以下はサンプルから抜き出した値 ▲▲▲ */
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
    u1s_2msflipcount = 1 - u1s_2msflipcount;                    /* 計算周期は4ms周期に変更する */

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

        if ( ( ( status->lightval > GRAY_WHITE - 30 )             /* カーブ進入時光センサ変化を検出 */  /* 光センサ最初問題があると、すぐ停止になった！！！*/
            && ( u1s_judge == 0 ))                                /* カーブ通過したではなく */
          || ( u1s_init > 0 ) )                                  /* 初期不安定状態回避対策 */
        {
            f4t_yp = (f4)(GRAY_WHITE - status->lightval );
            f4t_yd = (f4t_yp - f4s_last_yp) / 0.004;
            f4s_last_yp = f4t_yp;
            f4t_turn = (f4)(TRACE_P * f4t_yp) + (f4)(TRACE_D * f4t_yd);   /* 倒立のPD制御 */

            u4s_distance = status->motorangle_L + status->motorangle_R;   /* 最後の距離をラッチ */
            f4t_forward = 50 - s2t_diff*3;                                  /* 前進のスピードを固定する */
            
            if ( u1s_tail == 1)                                           /* ライン復帰した */
            {
                f4s_forward -= 0.1;                                       /* 減速 */
                if ( f4s_forward <= 0 )                                   /* 0ガード */
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
            /* 進入速度を計測し、補正する */
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
                f4t_turn = 6;                                               /* カーブ時の旋回量 6.2-6.4 */
                u1s_judge = 1;                                                /* カーブ通過中 */
            }
            else
            {
                f4t_turn = 0;                                                 /* カーブ時の旋回量 */
            }

            f4t_forward = 40;                                             /* カーブ通過時の前進量 */

            /* ライン復帰 */
            if( ( status->motorangle_L + status->motorangle_R - u4s_distance ) > 1100 )  /* 指定距離以上前進した */
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
                        u1s_judge = 0;                                            /* 倒立トレースする */
                        u1s_tail = 1;                                             /* ライン復帰した */
                        f4s_forward = 30;                                         /* 前進スピートを落とす */
                    }
                    if( u1t_dir == 1 ) 
                    {
                        if( ( status->lightval < GRAY_WHITE + 50 )
                         && ( status->lightraw < status->lightval ))
                        {
                            u1s_judge = 0;                                            /* 倒立トレースする */
                            u1s_tail = 1;                                             /* ライン復帰した */
                            f4s_forward = 30;                                         /* 前進スピートを落とす */
                        }
                    }
                }
			}
            /* ライン復帰 */

#if 0  /* 簡易ライン復帰 */
			if( ( ( status->motorangle_L + status->motorangle_R - u4s_distance ) > 1000 )
             && ( status->lightval > GRAY_WHITE - 10 ) )
			{
				u1s_judge = 0;
				u1s_tail = 1;
				f4s_forward = 30;
			}
#endif 
		}
		
        if (u1s_tail < 2)                        /* 倒立中 */
		{
            balance_control(
                (f4)f4t_forward,                          /* 前後進命令(+:前進, -:後進) */
                (f4)-f4t_turn,                    /* 旋回命令(+:右旋回, -:左旋回) */
                (f4)status->gyroval,             /* ジャイロセンサ値 */
                (f4)status->gyro_offset,         /* ジャイロセンサオフセット値 */
                (f4)status->motorangle_L,        /* 左モータ回転角度[deg] */
                (f4)status->motorangle_R,        /* 右モータ回転角度[deg] */
                (f4)status->batteryval,          /* バッテリ電圧[mV] */
                &left,                           /* 左モータPWM出力値 */
                &right                           /* 右モータPWM出力値 */
                );
            tail = tailControl( 30 , status->motorangle_T );   /* 尻尾を30°保持 */

        }
        else                                     /* 尻尾状態 */
        {
            u4s_count++;
            left = 0;                            /* 停止 */
			right = 0;
            if ( u4s_count < 20 )
        {
                left = 60;                            /* 停止 */
                right = 60;
        }
            tail = tailControl( 90 , status->motorangle_T );  /* 尻尾を90°保持 */
        }
        setMotorVal( left , right , tail );      /* 出力する */
        
    /* 以下はモニタ用 */
    *((long *)(&data_log_buffer[8])) = u1s_judge;
    *((long *)(&data_log_buffer[12])) = status->motorSpeed;
    *((long *)(&data_log_buffer[16])) = s4t_speed_data_sum;
    /* 以上はモニタ用 */
    }
    /* user logic end */
}

s1
tailControl( char tailAngletarget , char tailAngle )
{
    float pwm = (float)(tailAngletarget - tailAngle) * P_GAIN; /* 比例制御 */
    /* PWM出力飽和処理 */
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
