/* recv.c */

#include "../robot/robot.h"
#include "./controlval.h"
#include "./recv.h"
#include "balancer.h"


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

#define P_GAIN             2.5F

typedef signed char    s1;
typedef signed short   s2;
typedef signed long    s4;
typedef unsigned char  u1;
typedef unsigned short u2;
typedef unsigned long  u4;
typedef float          f4;
typedef double         f8;

/*----------------*/
/*      適合      */
/*----------------*/
/* 電池MAX 本番ロボで適合済み */
#define SOUKOU_LIGHT       580   /* soukou_tail光センサ閾値 */
#define GENSOKU_TIME       1000  /* 減速時間 */
#define GENSOKU_SPEED      60    /* 減速時フォワード */
#define GENSOKU_TAIL       90    /* 減速時尻尾角度 */
#define WAIT_TIME          1000  /* 安定待ち時間 */
#define STANDUP_TAIL_MAX   100   /* 直立時尻尾角度 */
#define TAIL_PWM_MAX       60    /* 尻尾モーターのMAXPWM */
#define BACK_SPEED         -50    /* 倒立時モータPWM */



void stairs_tail(u1);
void tail_control_recv(signed int);
void setMotorVal( signed char , signed char , signed char );

/*----------------*/
/* グローバル変数 */
/*----------------*/

static u1 u1s_2msflipcount_recv;
static u1 u1_recv_state = 0;
static u1 u1_standup_tail;
static u1 u1_pwm_abs_max = 50;
static u1 u1s_touritu_recv;
static f4 f4g_forward=0;
static f4 f4g_turn=0;
static s1 left  = 0;
static s1 right =0;
static float pwm=0;

/*---------------*/
/* 復帰制御(4ms) */
/*---------------*/
void recv_4ms(Command* cmd , ROBOT_STATUS* status)
{

    static u4 u4_time_tmp;
    static u1 u1_flg1 = OFF;
    static u4 u4_motor;
    static u4 u4_zenkai;
    static u4 u4_cnt = 0;
    static s4 s4t_motor_L1 = 0;
    static s4 s4t_motor_R1 = 0;
    static s4 s4t_motor_L2 = 0;
    static s4 s4t_motor_R2 = 0;
    static s4 s4t_distance = 0;

    
    s4t_motor_L1 = nxt_motor_get_count(PORT_MOTOR_L);
    s4t_motor_R1 = nxt_motor_get_count(PORT_MOTOR_R);




     /* user logic start */
    u1s_2msflipcount_recv = 1 - u1s_2msflipcount_recv;

    display_clear(0);
    display_goto_xy(0, 0);  display_string("stage");  display_unsigned(u1_recv_state,3);
    display_goto_xy(0, 1);  display_string("motor");  display_unsigned(status->gyro_offset,3);
    display_goto_xy(0, 2);  display_string("MAX");  display_unsigned(status->gyro_offset,3);
    display_update(); 


    if ( u1s_2msflipcount_recv == 1 )
    {
    
    switch(u1_recv_state)
    {
        
        case 0:
        
        
//             if(status->gyroval < status->gyro_offset + 5 && status->gyroval > status->gyro_offset - 5){
                
//                u4_cnt++;
                
//           }else{
                
//                u4_cnt = 0;
                
//            }
            
//            if(u4_cnt > 0){
                
                u4_cnt = 0;
                u1_recv_state++;
//                setMotorVal( 0 , 0 , 0 );
 //          }
  
        
/*            u4_cnt++;
            if(u4_cnt > 1000){
                
                u4_cnt = 0;
                u1_recv_state++;
            }
*/
        break;
        
        case 1 :
                u4_motor  = nxt_motor_get_count(PORT_MO_TAIL);
                u4_zenkai = nxt_motor_get_count(PORT_MO_TAIL);
//              nxt_motor_set_speed(PORT_MO_TAIL, 500 , 1);

                /* 電池電圧で尻尾角度決める */
                if(ecrobot_get_battery_voltage() >= 9000) /* 電圧値別尻尾角度入力 */
                {
                    u1_standup_tail = 105;
                }
                else if((8300 < ecrobot_get_battery_voltage()) && (ecrobot_get_battery_voltage() < 9000))
                {
                    u1_standup_tail = 105;
                }
                else
                {
                u1_standup_tail = 106;
                }
                
                u1_recv_state++;

        break;
        
        
        case 2 :
        
            stairs_tail(u1_standup_tail);                       /* 尻尾制御 */
//            tail_control_recv(u1_standup_tail);
            
            
            if(nxt_motor_get_count(PORT_MO_TAIL) <= u4_zenkai)
            {
                u1_pwm_abs_max = u1_pwm_abs_max + 10;
                u4_zenkai = nxt_motor_get_count(PORT_MO_TAIL);
            }
            else
            {
                if(u1_pwm_abs_max >= TAIL_PWM_MAX)
                {
                    u1_pwm_abs_max = u1_pwm_abs_max -5;
                    u4_zenkai = nxt_motor_get_count(PORT_MO_TAIL);
                }
                else if(u1_pwm_abs_max < TAIL_PWM_MAX)
                {
                    u1_pwm_abs_max = TAIL_PWM_MAX;
                    u4_zenkai = nxt_motor_get_count(PORT_MO_TAIL);
                }
            }

            
            if(nxt_motor_get_count(PORT_MO_TAIL) >= 103)    /* デバッグ用 */
            {
                //ecrobot_sound_tone(1000,100,5);
            }
            
            
            if(nxt_motor_get_count(PORT_MO_TAIL) >= 103)
            {
                if(u1_flg1 == OFF)
                {
                    u4_time_tmp = ecrobot_get_systick_ms();
                    u1_flg1 = ON;
                    u1_standup_tail = STANDUP_TAIL_MAX;
                    
                }
                if((ecrobot_get_systick_ms() - u4_time_tmp) > 500)
                {
                    left = 0;
                    right = 0;
                    f4g_forward = 0;
                    u1_recv_state++;
                    u1_flg1 = OFF;
                    balance_init();
                    nxt_motor_set_count(PORT_MOTOR_R,0);
                    nxt_motor_set_count(PORT_MOTOR_L,0);
                    u1s_touritu_recv = ON;
                   s4t_motor_L2 = nxt_motor_get_count(PORT_MOTOR_L);
                    s4t_motor_R2 = nxt_motor_get_count(PORT_MOTOR_R);

                }
            }
            else// if(nxt_motor_get_count(PORT_MO_TAIL) <= 98)
            {
                if(u4_cnt > 25)
                {
                    u1_standup_tail++;
                    u4_cnt = 0;
                }
                else 
                {
                   u4_cnt++;
               }
            }
                
        break;
        
        case 3 :
//          touritu_pwon();
            left = 0;
            right = 0;
            f4g_turn = 0;
            f4g_forward = 40;
            u1_pwm_abs_max = 50;  /* 元に戻す */
            u1s_touritu_recv = ON;

//            u4_time_tmp = ecrobot_get_systick_ms();
            u1_recv_state++;
            u4_cnt = 0;
 //           tail_control_recv(20);
            
        break;
        
        case 4 :   /* いらない処理かも */
//            tail_control_recv(20);
            u1s_touritu_recv = ON;
                        
            if(status->gyroval < status->gyro_offset + 10 && status->gyroval > status->gyro_offset - 10){
                
                u4_cnt++;
                
            }else{
                
                u4_cnt = 0;
                
            }
            
            if(status->motorangle_T > 70){
                
                pwm = -10;
                
            }else{
                
                pwm = 0;
            
            }
            
            if(status->motorangle_T < 70 && u4_cnt > 10){
                
              u1_recv_state++;
              f4g_turn = 0;
              f4g_forward = 20;

                
                }

        break;
        
         case 5 :

           s4t_distance = s4t_motor_L1 - s4t_motor_L2 + s4t_motor_R1 - s4t_motor_R2;

              if(s4t_distance > 0){                  

//                  u1s_trace = 0;                  /*トレースの設定*/              
                  f4g_forward = 20;
                   // chokushin2();
                    u1_recv_state++;
                    
                    }else{
                        
                      f4g_forward = 20;
                      
                      }

         
        break;


    }
    
    
          if (u1s_touritu_recv == 1)
        {

        balance_control(
            (f4)f4g_forward,                          /* 前後進命令(+:前進, -:後進) */
            (f4)f4g_turn,                    /* 旋回命令(+:右旋回, -:左旋回) */
//            (f4)-6,                       /* 旋回命令(+:右旋回, -:左旋回) */
            (f4)status->gyroval,             /* ジャイロセンサ値 */
            (f4)status->gyro_offset,         /* ジャイロセンサオフセット値 */
//            (f4)status->motorangle_L,        /* 左モータ回転角度[deg] */
//            (f4)status->motorangle_R,        /* 右モータ回転角度[deg] */
            (f4)nxt_motor_get_count(PORT_MOTOR_L),
            (f4)nxt_motor_get_count(PORT_MOTOR_R),
            (f4)status->batteryval,          /* バッテリ電圧[mV] */
            &left,                           /* 左モータPWM出力値 */
            &right                           /* 右モータPWM出力値 */
        );
        
        setMotorVal( left , right , pwm );
        
        }

    }
    
}

void setMotorVal( signed char left , signed char right , signed char tail)
{
    motorvalue.leftmotor = left;
    motorvalue.rightmotor = right;
    motorvalue.tailmotor = tail;
}


/*-----------------*/
/* 階段用尻尾制御  */
/*-----------------*/
void stairs_tail(u1 angle)
{
    static u1 u1_pwm = 95;
    static s4 s4_tail_angle;
    
    s4_tail_angle = nxt_motor_get_count(PORT_MO_TAIL);   /* 現在の尻尾角度取得 */
    
    if((angle - 20) > s4_tail_angle)
    {
        setMotorVal( left , right , u1_pwm );         /* Left  Motor PWM output: -100% ~ 100% */
    }
    else if((angle + 20) < s4_tail_angle)
    {
        setMotorVal( left , right , -u1_pwm );         /* Left  Motor PWM output: -100% ~ 100% */
    }
    else
    {
        tail_control_recv(angle);
    }
}

//*****************************************************************************
// 関数名 : tail_control
// 引数 : angle (モータ目標角度[度])
// 返り値 : 無し
// 概要 : 走行体完全停止用モータの角度制御
//*****************************************************************************
void tail_control_recv(signed int angle)
{
    float pwm = (float)(angle - nxt_motor_get_count(PORT_MO_TAIL))*P_GAIN; /* 比例制御 */
    /* PWM出力飽和処理 */
    if (pwm > u1_pwm_abs_max)
    {
        pwm = u1_pwm_abs_max;
    }
    else if (pwm < -u1_pwm_abs_max)
    {
        pwm = -u1_pwm_abs_max;
    }
    
    setMotorVal( left , right , pwm );
}

int
u4g_dispatch_func_recv( void )
{
    return u1_recv_state;
} 



