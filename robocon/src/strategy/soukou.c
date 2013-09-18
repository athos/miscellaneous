#include "../robot/robot.h"
#include "./controlval.h"
#include "./soukou.h"

#define P_GAIN             2.5F /* 完全停止用モータ制御比例係数 */
#define PWM_ABS_MAX          50 /* 完全停止用モータ制御PWM絶対最大値 */
#define SPEED_DOWN_TIME       4

typedef unsigned short u2;
typedef unsigned char  u1;
typedef char  s1;
typedef short s2;
typedef float f4;

typedef struct _targetComd {
	unsigned short lighttarget;
	unsigned short lightval;
	
	unsigned char  tailangletarget;
	unsigned char  tailangle;

	float d_period;
	float p_data;
	float d_data;
	
	float corner_data;
	unsigned short stopTime;
	unsigned char mode;
} targetComd;

typedef struct _command
{
	unsigned short distance;          //終了条件
	short  gyrohys;                   //終了条件
	short lighttarget;
	unsigned char  tailangletarget;
	float p_data;
	float d_data;
	float corner_data;
	unsigned char  mode_straight;
} command;

static u2 u2s_mode1_count;
static u1 u1s_motor_speed;
static targetComd targetLocal;
static f4 f4s_last_yp;
static u1 u1s_2msflipcount;
static u1 nowCmdNum = 0;
static u2 u2s_count;

void Run( void );
s1 tailControl( unsigned char tailAngletarget , unsigned char tailAngle );
void soukou_tail( targetComd target , motorval *motorvalue );


#define  GRAY_WHITE          (( ( robot.robotSensor.LIGHT.white  + robot.robotSensor.LIGHT.black ) / 2 + robot.robotSensor.LIGHT.white ) /2)
#define  PARA_CHANGE_TIME    200 //2ms*200=400ms
#define  BASE_CORNER_DATA    0.35
#define  BASE_P_DATA         0.3
#define  BASE_D_DATA         0.12

command command1[] =
{
	{ 200 ,   0 , 0  , 95  ,  0.3,  0.12 , 0.35 , 1 },
	{2700 , -40 , 0  , 95  ,  0.3,  0.12 , 0.35 , 1 }, /* up */
	{3500 ,  40 , 0  , 105 ,  0.3,  0.12 , 0.35 , 1 }, /* peak */
	{4200 ,   0 , 0  , 85  ,  0.4,  0.12 , 0.65 , 2 }, /* down */  /* 坂道高低差対策 */
	{5200 ,   0 , 0  , 95  ,  0.3,  0.12 , 0.35 , 1 }, /* down 2 */
	{5650 ,   0 , 0  , 95  ,  0.3,  0.12 , 0.35 , 1 }, /* L1 start */ 
	{7500 ,   0 , 30 , 95  ,  0.45, 0.24 , 0.65 , 2 }, /* L1 end */
	{9200 ,   0 , 0  , 95  ,  0.3,  0.12 , 0.35 , 1 }, /* L2 start */
	{11700 ,  0 , 20 , 95  ,  0.55, 0.24,  0.65 , 2 }, /* L2 end */
	{12200 ,  0 , 0  , 95  ,  0.3,  0.12 , 0.35 , 1 }, /* R1 start */
	{15900 ,  0 , -25, 95  ,  0.45, 0.16,  0.6  , 2 }, /* R1 end */
	{17300 ,  0 , 0  , 95  ,  0.3,  0.12,  0.35 , 1 }, /* L3 start */
	{18800 ,  0 , 30 , 95  ,  0.55, 0.24,  0.7  , 2 }, /* L3 end */
	{19000 ,  0 , 0  , 95  ,  0.3,  0.12 , 0.35 , 1 } 
};


void setBasicStageTargetValue( Command* cmd , ROBOT_STATUS* status )
{
    s2 s2t_distance;

    s2t_distance = (status->motorangle_L + status->motorangle_R ) >>1;
    if (s2t_distance >= command1[nowCmdNum].distance)
    {
        nowCmdNum++;
        u2s_count = 0;
    }

    if ( u2s_count < PARA_CHANGE_TIME )
    {
        u2s_count++;
    }

    targetLocal.lighttarget = GRAY_WHITE + (command1[nowCmdNum].lighttarget * u2s_count / PARA_CHANGE_TIME);
    targetLocal.lightval = status->lightval;
    targetLocal.tailangletarget = command1[nowCmdNum].tailangletarget;
    targetLocal.tailangle = status->motorangle_T;
    targetLocal.d_period = 0.004;
    targetLocal.p_data = BASE_P_DATA + (command1[nowCmdNum].p_data-BASE_P_DATA)* u2s_count / PARA_CHANGE_TIME;
    targetLocal.d_data = BASE_D_DATA + (command1[nowCmdNum].d_data-BASE_D_DATA)* u2s_count / PARA_CHANGE_TIME;
    targetLocal.corner_data = BASE_CORNER_DATA + (command1[nowCmdNum].corner_data-BASE_CORNER_DATA) *  u2s_count / PARA_CHANGE_TIME;
    targetLocal.stopTime = 0;
    targetLocal.mode = command1[nowCmdNum].mode_straight;

/* モニタ用 */
    *((long *)(&data_log_buffer[12])) = (long)(status->motorangle_R + status->motorangle_L) >> 1;
    *((long *)(&data_log_buffer[16])) = status->gyroval;
/* モニタ用 */

    Run();
}

void setBasicToPerTargetValue( Command* cmd , ROBOT_STATUS* status )
{
    targetLocal.lighttarget = GRAY_WHITE + command1[12].lighttarget;
    targetLocal.lightval = status->lightval;
    targetLocal.tailangletarget = 90;
    targetLocal.tailangle = status->motorangle_T;
    targetLocal.d_period = 0.004;
    targetLocal.p_data = command1[12].p_data;
    targetLocal.d_data = command1[12].d_data;
    targetLocal.corner_data = command1[12].corner_data;
    targetLocal.stopTime = 1;
    targetLocal.mode = command1[12].mode_straight;

    Run();
}

void setStopTargetValue( Command* cmd , ROBOT_STATUS* status )  /* 尻尾立つのみ */
{
    targetLocal.tailangletarget = 90;
    targetLocal.tailangle = status->motorangle_T;

    soukou_tail( targetLocal , &motorvalue );
    motorvalue.tailmotor = tailControl( targetLocal.tailangletarget , targetLocal.tailangle );
}

void
Run()
{

    if ( targetLocal.mode == 1 )
    {
        u1s_2msflipcount = 1 - u1s_2msflipcount;
    }
    else if ( targetLocal.mode == 2 )
    {
        u1s_2msflipcount = 1;
    }

    if ( u1s_2msflipcount == 1 )
    {
        soukou_tail( targetLocal , &motorvalue );
        motorvalue.tailmotor = tailControl( targetLocal.tailangletarget , targetLocal.tailangle );
    }
}


s1
tailControl( unsigned char tailAngletarget , unsigned char tailAngle )
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


void
soukou_tail( targetComd target , motorval *motorvalue )
{

    f4 f4t_yp;
    f4 f4t_yd;
    f4 f4t_turn;
    f4 f4t_turn_abs;
    u1 u1t_motor_L;
    u1 u1t_motor_R;
    u1 u1t_motor_max;
    f4 f4t_turnmax;
    f4 f4t_turnmin;

    f4t_yp = (f4)((s2)target.lighttarget - target.lightval);
    f4t_yd = (f4t_yp - f4s_last_yp) / target.d_period;
    f4s_last_yp = f4t_yp;
    f4t_turn = (f4)(target.p_data * f4t_yp) + (f4)(target.d_data * f4t_yd);

    f4t_turn = f4t_turn * target.corner_data;

    u1t_motor_max = 95;

    if ( target.stopTime != 0)
    {
        u2s_mode1_count++;
        u1t_motor_max = u1s_motor_speed;

        if ( ( u2s_mode1_count % SPEED_DOWN_TIME ) == 0 )
        {
            if( u1t_motor_max > 0 )
            {
                u1t_motor_max--;
            }
        }
    }

    u1s_motor_speed = u1t_motor_max;


    f4t_turnmax = ((f4)u1t_motor_max)/2;
    f4t_turnmin = -((f4)u1t_motor_max)/2;

    if ( f4t_turn > f4t_turnmax )
    {
        f4t_turn = f4t_turnmax;
    }
    if ( f4t_turn < f4t_turnmin )
    {
        f4t_turn = f4t_turnmin;
    }

    f4t_turn = -f4t_turn;

    f4t_turn_abs = f4t_turn;
    if ( f4t_turn < 0 )
    {
        f4t_turn_abs = -f4t_turn;
    }
    if ( ( target.mode == 1 )
      && ( f4t_turn_abs >= 5 )
      && ( target.lightval <= target.lighttarget + 5 )
      && ( target.lightval >= target.lighttarget - 5 ))
    {
        if ( f4t_turn > 0 )
        {
            u1t_motor_L = (u1)(( 101 - f4t_turn ) + f4t_turn / 2 + 0.5) + 1;
            u1t_motor_R = (u1)(( 100 - f4t_turn ) - f4t_turn / 2 + 0.5);
        }
        else
        {
            u1t_motor_R = (u1)(( 100 + f4t_turn ) - f4t_turn / 2 + 0.5);
            u1t_motor_L = (u1)(( 100 + f4t_turn ) + f4t_turn / 2 + 0.5) + 1;
        }
    }
    else
    {

        if ( f4t_turn > 0 )
        {
            u1t_motor_L = u1t_motor_max;
            u1t_motor_R = u1t_motor_max - 2*f4t_turn;
        }
        else
        {
            u1t_motor_L = u1t_motor_max + 2*f4t_turn;
            u1t_motor_R = u1t_motor_max;
        }
    }

    if ( ( target.stopTime != 0)
      && ( u1t_motor_max < 20 ) )
    {
            u1t_motor_L = 0;
            u1t_motor_R = 0;
    }


    motorvalue->leftmotor = u1t_motor_L;
    motorvalue->rightmotor = u1t_motor_R;

/* モニタ用 */
    *((unsigned int *)(&data_log_buffer[0])) = 0;
    *((char *)(&data_log_buffer[ 4])) = u1t_motor_L;
    *((char *)(&data_log_buffer[ 5])) = u1t_motor_R;
    *((unsigned short *)(&data_log_buffer[6])) = target.lightval;
    *((long *)(&data_log_buffer[ 8])) = (long)target.lighttarget;
//    *((long *)(&data_log_buffer[12])) = (long)(status->motorangle_R + status->motorangle_L) >> 1;
//    *((long *)(&data_log_buffer[16])) = status->gyroval;
    *((unsigned short *)(&data_log_buffer[  20])) = (short)0;
    *((unsigned short *)(&data_log_buffer[  22])) = (short)0;
    *((unsigned short *)(&data_log_buffer[  24])) = (short)0;
    *((unsigned short *)(&data_log_buffer[  26])) = (short)0;
    *((long *)(&data_log_buffer[28])) = 0;
/* モニタ用 */

}
