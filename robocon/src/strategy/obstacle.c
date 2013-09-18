
#include "../robot/robot.h"
#include "./controlval.h"
#include "./obstacle.h"
#include "balancer.h"


#define P_GAIN             2.5F /* 完全停止用モータ制御比例係数 */
#define PWM_ABS_MAX          50 /* 完全停止用モータ制御PWM絶対最大値 */
#define  K  0.03

/* NXT motor port configuration */
#define PORT_MO_TAIL    NXT_PORT_A
#define PORT_MOTOR_R    NXT_PORT_B
#define PORT_MOTOR_L    NXT_PORT_C

/* NXT sensor port configuration */
#define PORT_TOUCH      NXT_PORT_S4
#define PORT_SONAR      NXT_PORT_S2
#define PORT_LIGHT      NXT_PORT_S3
#define PORT_GYRO       NXT_PORT_S1


#define  ON   1
#define  OFF  0

#define NUM_HISTORIES 10

typedef signed char    s1;
typedef signed short   s2;
typedef signed long    s4;
typedef unsigned char  u1;
typedef unsigned short u2;
typedef unsigned long  u4;
typedef float          f4;
typedef double         f8;


void soukou_tail_obst(u2, f4, f4, f4, u1); 
void setMotorVal(s1, s1, s1);
s1 tailControl( char tailAngletarget , char tailAngle );

#define  GRAY_WHITE     (( ( robot.robotSensor.LIGHT.white  + robot.robotSensor.LIGHT.black ) / 2 + robot.robotSensor.LIGHT.white ) /2)

#define TH_DANSA 75
#define STAT_CNT 200

static u2 u2g_light_data = 0;
//static u2 u2g_sonar_data = 0;
static f4 f4t_motor_L, f4t_motor_R;
static u1 obstacle_state = 0;

static u2 gyro_val_now = 0;
static u2 gyro_val_bef = 0;
static u2 obstacle_cnt = 0;
static u2 obstacle_cnt_1 = 0;
static u1 tail_mokuhyou= 100;

static u1 flg_tail = 0;
static s4 motor_Lc[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
static s4 motor_Rc[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
static u1 array_cnt = 0;

void obstacle( Command* cmd , ROBOT_STATUS* status )
{
    
    s1 tail_angle;
    u1 sonar;
    u1 array_p1;
    s2 motor_def;


    gyro_val_bef = gyro_val_now;                   // １つ前のジャイロセンサー値保存
    gyro_val_now = status->gyroval;                // 現在のジャイロセンサー値保存

    u2g_light_data = status->lightval;                      /*現在の光センサ値保存*/
//    u2g_sonar_data = status->sonarval;                      /*現在の超音波値保存*/

    sonar = ecrobot_get_sonar_sensor(PORT_SONAR);


// motor count
    if ( array_cnt == 15 ) {
        array_cnt = 0;
    } else {
        array_cnt++;
    }
    motor_Lc[array_cnt] = nxt_motor_get_count(PORT_MOTOR_L);
    motor_Rc[array_cnt] = nxt_motor_get_count(PORT_MOTOR_R);
    
    array_p1 = ( array_cnt + 1) & 15;
    motor_def = (motor_Lc[array_p1] - motor_Lc[array_cnt])-(motor_Rc[array_p1] - motor_Rc[array_cnt]);


    if (( gyro_val_now > status->gyro_offset + TH_DANSA )||
        ( gyro_val_now < status->gyro_offset - TH_DANSA )||
       (( motor_Lc[array_p1]-motor_Lc[array_cnt])==0    )){
        obstacle_state = 1;
    }
    
    obstacle_cnt = obstacle_cnt + obstacle_state;
    if ( obstacle_cnt == STAT_CNT ) flg_tail = 1;
    obstacle_cnt_1 = obstacle_cnt_1 + 1;


    display_clear(0);
    display_goto_xy(0, 0);  display_string("LEFT");   display_unsigned(f4t_motor_L,3);
    display_goto_xy(0, 1);  display_string("RIGHT");  display_unsigned(f4t_motor_R,3);
//    display_goto_xy(0, 2);  display_string("SONAR");  display_unsigned(status->sonarval,3);
    display_goto_xy(0, 2);  display_string("SONAR");  display_unsigned(sonar,3);
//    display_goto_xy(0, 3);  display_string("GYRO_A"); display_unsigned(gyro_val_now,3);
//    display_goto_xy(0, 4);  display_string("GYRO_B"); display_unsigned(gyro_val_bef,3);
//    display_goto_xy(0, 5);  display_string("GYRO_S"); display_unsigned(gyro_sabun,3);
    display_goto_xy(0, 3);  display_string("ENC_L"); display_unsigned(nxt_motor_get_count(PORT_MOTOR_L),3);
    display_goto_xy(0, 4);  display_string("ENC_R"); display_unsigned(nxt_motor_get_count(PORT_MOTOR_R),3);
//    display_goto_xy(0, 3);  display_string("tail"); display_unsigned(tail_mokuhyou,3);
//    display_goto_xy(0, 4);  display_string("count"); display_unsigned(obstacle_cnt,3);
//    display_goto_xy(0, 5);  display_string("flg" ); display_unsigned(flg_tail,3);
    display_goto_xy(0, 5);  display_string("DEF" ); display_unsigned(motor_def,3);
    display_goto_xy(0, 6);  display_string("STATE");  display_unsigned(obstacle_state,3);
    display_update(); 


if ( obstacle_state ) {
    if ( flg_tail ){
        f4t_motor_R = 30;
        f4t_motor_L = 30;
        tail_mokuhyou = 75;
    } else {
        f4t_motor_R = 60;
        f4t_motor_L = 60;
        tail_mokuhyou = 110;
    }
} else {
    if ( obstacle_cnt_1 < 500 ){
        f4t_motor_R = 20;
        f4t_motor_L = 20;
    } else  {
        f4t_motor_R = 35;
        f4t_motor_L = 35;
    }
    tail_mokuhyou = 100;
}

        tail_angle = tailControl( tail_mokuhyou , status->motorangle_T ); /*目標尻尾角度、現在の尻尾角度*/

    f4t_motor_R = f4t_motor_R - motor_def;
    f4t_motor_L = f4t_motor_L + motor_def;


    setMotorVal( f4t_motor_L , f4t_motor_R , tail_angle );

    /* user logic end */
}


void soukou_tail_obst(u2 u2t_light, f4 f4t_Kp, f4 f4t_Ki, f4 f4t_Kd, u1 u1t_forward)
{
    f4 f4t_yp, f4t_yi, f4t_yd;
    
    static f4 f4s_integ_y = 0;
    static f4 f4s_y_hist[NUM_HISTORIES];
    static int hist_index = 0;
    
    static f4 f4s_last_yp = 0;
    f4 f4t_turn;
    
    
    f4t_yp = (f4)(u2t_light - u2g_light_data);
    f4s_integ_y = f4s_integ_y + f4t_yp - f4s_y_hist[hist_index];
    f4t_yi = f4s_integ_y / NUM_HISTORIES;
    
    f4t_yd = f4t_yp - f4s_last_yp;
    f4t_turn = f4t_Kp * f4t_yp + f4t_Ki * f4t_yi + f4t_Kd * f4t_yd;
    f4t_turn = f4t_turn * -1;   /*右エッジ*/


//  f4t_turn = adjust_turn(f4t_turn);
    
    f4t_motor_L = (f4)u1t_forward + f4t_turn;
    f4t_motor_R = (f4)u1t_forward - f4t_turn;

    // モータ出力をガードする
//  s1g_motor_L = (s1)GUARD(f4t_motor_L, -100, 100);
//  s1g_motor_R = (s1)GUARD(f4t_motor_R, -100, 100);
    

    f4s_last_yp = f4t_yp;
    f4s_y_hist[hist_index] = f4t_yp;
    hist_index = (hist_index + 1) % NUM_HISTORIES;
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
