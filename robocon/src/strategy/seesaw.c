
#include "../robot/robot.h"
#include "./controlval.h"
#include "./seesaw.h"
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

typedef signed char    s1;
typedef signed short   s2;
typedef signed long    s4;
typedef unsigned char  u1;
typedef unsigned short u2;
typedef unsigned long  u4;
typedef float          f4;
typedef double         f8;

s1 tailControl( unsigned char tailAngletarget , unsigned char tailAngle );
void setMotorVal( signed char , signed char , signed char );
void chokushin2();

static u1 u1s_2msflipcount_see = 1;

static f4 f4s_last_yp = 0;
static f4 f4g_forward = 0;
static f4 f4g_turn = 0;

static s4 s4t_motor_diff;

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

void seesaw_ride( Command* cmd , ROBOT_STATUS* status )
{

    signed char left;
    signed char right;
    f4 f4t_yp;
    f4 f4t_yd;
	static u1 u1g_seesaw_mode = 0;    
	static u2 u2_count = 0;
	static u2 u2_gyro_temp=0;
	static s4 s4t_distance = 0;
	static s4 s4t_motor_L1 = 0;
	static s4 s4t_motor_R1 = 0;
	static s4 s4t_motor_L2 = 0;
	static s4 s4t_motor_R2 = 0;
	static s4 s4t_motor_L3 = 0;
	static s4 s4t_motor_R3 = 0;

	static u1 u1s_trace = 0;
	static u1 u1s_touritu = 0;	
	
	s4t_motor_L1 = nxt_motor_get_count(PORT_MOTOR_L);
	s4t_motor_R1 = nxt_motor_get_count(PORT_MOTOR_R);

    static u4 u4_recieve = 0;
  	u1  receive_data[90] = {0}; 

    /* user logic start */
    u1s_2msflipcount_see = 1 - u1s_2msflipcount_see;

    display_clear(0);
    display_goto_xy(0, 0);  display_string("stage");  display_unsigned(u1g_seesaw_mode,3);
    display_goto_xy(0, 1);  display_string("LEFT");  display_unsigned(left,3);
    display_goto_xy(0, 2);  display_string("RIGHT");  display_unsigned(right,3);
    display_goto_xy(0, 3);  display_string("count");  display_unsigned(f4g_forward,3);
    display_update(); 


    if ( u1s_2msflipcount_see == 1 )
    {

		switch(u1g_seesaw_mode)       /* シーソーシーケンス開始 */
		{
		
			case 0 :  /* 倒立開始 */
		
			//	u1s_trace = 1;							 /* トレースOFF */
				u1s_touritu = 1;							/* トレースON */
				f4g_forward = 20;
			    balance_init();    						/* balance APIの初期化 */
			    nxt_motor_set_count(PORT_MOTOR_L, 0);    /* 左モータ・カウント値[°]を0リセット */
    			nxt_motor_set_count(PORT_MOTOR_R, 0);    /* 右モータ・カウント値[°]を0リセット */
				s4t_motor_L2 = nxt_motor_get_count(PORT_MOTOR_L);
				s4t_motor_R2 = nxt_motor_get_count(PORT_MOTOR_R);
				u1g_seesaw_mode = 1;
				u2_count = 0;
				
			break;


//			case 51:
//						f4g_forward = 40;			
						
//			break;

			
			case 1 :  /* 段差検知まで */

				s4t_distance = s4t_motor_L1 - s4t_motor_L2 + s4t_motor_R1 - s4t_motor_R2;

				if(s4t_distance < 50){					

					u1s_trace = 0;					/*トレースの設定*/				
					f4g_forward = 40;
		
					
				}else if(s4t_distance < 500){					
				
					u1s_trace = 1;					/*トレースの設定*/				
					f4g_forward = 40;
				
				}else{
					
					u1s_trace = 0;
					chokushin2();
				
					 if(status->gyroval > status->gyro_offset + 40 || status->gyroval < status->gyro_offset - 40){
				
						u1g_seesaw_mode++;
						f4g_forward = 100;
						s4t_motor_L2 = nxt_motor_get_count(PORT_MOTOR_L);
						s4t_motor_R2 = nxt_motor_get_count(PORT_MOTOR_R);
						u2_count = 0;
						u1s_trace = 0;
						u2_gyro_temp = robot.robotSensor.GYRO.gyroOffsetValue;
					
						//robot.robotSensor.GYRO.gyroOffsetValue = robot.robotSensor.GYRO.gyroOffsetValue -10;
					}
				
				}

			break;
			
			case 2 :  /* 検知後ある一定以上上る */
			
				s4t_distance = s4t_motor_L1 - s4t_motor_L2 + s4t_motor_R1 - s4t_motor_R2;

				if(s4t_distance < 100){			//ﾍﾞｰｽ100

					f4g_forward = 100;
					chokushin2();
					
				}else if(s4t_distance < 300){	//べーす３００

					robot.robotSensor.GYRO.gyroOffsetValue = u2_gyro_temp;
					f4g_forward = 40;
					chokushin2();
				
				}else{
				
					u2_count = 0;
					f4g_forward = 0;
					chokushin2();
					u1g_seesaw_mode++;
				}
			
			break;
						
			case 3 :  /* Bluetoothが来るまで待機 */

				f4g_forward = 0;
				chokushin2();

		    	u4_recieve = ecrobot_read_bt_packet( receive_data, 90 );
 
            	if ( u4_recieve > 0 )
            	{
	            	ecrobot_sound_tone(100, 100, 50);
					u2_count = 0;
					f4g_forward = 0;
					chokushin2();
					u1g_seesaw_mode++;
									
				}
				
			break;
			
			case 4 :  /* Bluetoothが来たから上る */
			
				if(u2_count < 250){
				
					f4g_forward = 40;
					chokushin2();
					u2_count++;

				}else{
				
					 if(status->gyroval > status->gyro_offset + 40 || status->gyroval < status->gyro_offset - 40){
				
						u1g_seesaw_mode++;
						s4t_motor_L3 =nxt_motor_get_count(PORT_MOTOR_L);
						s4t_motor_R3 =nxt_motor_get_count(PORT_MOTOR_R);
						u2_count = 0;
					
					}else{
					
						f4g_forward = 40;
						chokushin2();
					}								
				}
				
			break;
			
			case 5 :  /* 段差検知 */
		
				s4t_distance = s4t_motor_L1 - s4t_motor_L3 + s4t_motor_R1 - s4t_motor_R3;

				if(s4t_distance < 100){
				
					f4g_forward = 60;
					chokushin2();
				
				}else{
				
					f4g_forward = 0;
					chokushin2();
					u2_count++;

					if(u2_count > 500){
				
						s4t_motor_L3 =nxt_motor_get_count(PORT_MOTOR_L);
						s4t_motor_R3 =nxt_motor_get_count(PORT_MOTOR_R);
						u1g_seesaw_mode = 6;
						u2_count = 0;
					}
				}

			break;
			
			case 6 :  /* ライン復帰 */

			 if(s4t_motor_L1 - s4t_motor_L3 < 200){
				
				if(status->lightval > GRAY_WHITE){
				
					f4g_turn = f4g_forward;
					u1g_seesaw_mode = 8;
	            	ecrobot_sound_tone(100, 100, 50);
					
				}else{
					
					f4g_turn = f4g_forward;
					
				}
				
			}else{
				
				f4g_turn = 0;
				u1g_seesaw_mode = 7;
					
			}

			break;

			case 7 :  /* ライン復帰 */
			
			if(	s4t_motor_L1 - s4t_motor_L3 > 50){
				
				f4g_forward = -20;
				f4g_turn = f4g_forward;
				
			}else{
					
				f4g_turn = 0;
				f4g_forward = 0;
				u1g_seesaw_mode = 9;
				
				}
				
			break;			

			case 8 :  /* ライン復帰 */

				if(status->lightval > GRAY_WHITE - 30){
					
					f4g_turn = f4g_forward;
	            	//ecrobot_sound_tone(1000, 1000, 50);

					
				}else{
					
					f4g_forward = 0;
					f4g_turn = -20;
					
					if((s4t_motor_L1 - s4t_motor_L3) - (s4t_motor_R1 - s4t_motor_R3) < -10 ){
						
						f4g_forward = 0;
					    f4g_turn = 0;
						u1g_seesaw_mode = 9; 
					}
				}

			break;			

			case 9 :

				u1s_trace = 1;
				f4g_forward = 30;

			break;
			
		}


        if (u1s_trace == 1)
       {
		
        f4t_yp = (f4)(GRAY_WHITE - status->lightval );
        f4t_yd = (f4t_yp - f4s_last_yp) / 0.004;
        f4s_last_yp = f4t_yp;
        f4g_turn = (f4)((f4)TRACE_P * f4t_yp) + (f4)((f4)TRACE_D * f4t_yd);
//        f4g_turn = -f4g_turn;
        /* 旋回方向決定 */
        
        if(f4g_turn > 50){
			
			f4g_turn = 50;
			
		}else if(f4g_turn < -50){
			
			f4g_turn = -50;
			
		}else{
			
			f4g_turn = f4g_turn;
			
		}

		f4g_turn = -f4g_turn;


		}

        if (u1s_touritu == 1)
        {
        balance_control(
            (f4)f4g_forward,                          /* 前後進命令(+:前進, -:後進) */
            (f4)f4g_turn,                    /* 旋回命令(+:右旋回, -:左旋回) */
//            (f4)-6,                    	/* 旋回命令(+:右旋回, -:左旋回) */
            (f4)status->gyroval,             /* ジャイロセンサ値 */
            (f4)status->gyro_offset,         /* ジャイロセンサオフセット値 */
            (f4)status->motorangle_L,        /* 左モータ回転角度[deg] */
            (f4)status->motorangle_R,        /* 右モータ回転角度[deg] */
            (f4)status->batteryval,          /* バッテリ電圧[mV] */
            &left,                           /* 左モータPWM出力値 */
            &right                           /* 右モータPWM出力値 */
        );
                
        }
        
        setMotorVal( left , right , -10 );
        
    }
    /* user logic end */
}

void seesaw_ride_pair( Command* cmd , ROBOT_STATUS* status )
{
	
}


void seesaw_go( Command* cmd , ROBOT_STATUS* status )
{

    signed char left;
    signed char right;
    f4 f4t_yp;
    f4 f4t_yd;
	static u1 u1g_seesaw_mode = 0;    
	static u2 u2_count = 0;
	static u2 u2_gyro_temp=0;
	static s4 s4t_distance = 0;
	static s4 s4t_motor_L1 = 0;
	static s4 s4t_motor_R1 = 0;
	static s4 s4t_motor_L2 = 0;
	static s4 s4t_motor_R2 = 0;
	static s4 s4t_motor_L3 = 0;
	static s4 s4t_motor_R3 = 0;
	static s4 s4t_motor_L4 = 0;
	static s4 s4t_motor_R4 = 0;

	static s1 tail_seesaw = 0; 


	static u1 u1s_trace = 0;
	static u1 u1s_touritu = 0;	
	
	s4t_motor_L1 = nxt_motor_get_count(PORT_MOTOR_L);
	s4t_motor_R1 = nxt_motor_get_count(PORT_MOTOR_R);

    static u4 u4_recieve = 0;
  	u1  receive_data[90] = {0}; 

    /* user logic start */
    u1s_2msflipcount_see = 1 - u1s_2msflipcount_see;

    display_clear(0);
    display_goto_xy(0, 0);  display_string("stage");  display_unsigned(u1g_seesaw_mode,3);
    display_goto_xy(0, 1);  display_string("LEFT");  display_unsigned(left,3);
    display_goto_xy(0, 2);  display_string("RIGHT");  display_unsigned(right,3);
    display_goto_xy(0, 3);  display_string("count");  display_unsigned(status->motorangle_T,3);
    display_update(); 


    if ( u1s_2msflipcount_see == 1 )
    {

		switch(u1g_seesaw_mode)       /* シーソーシーケンス開始 */
		{
		
			case 0 :  /* 倒立開始 */
		
			//	u1s_trace = 1;							 /* トレースOFF */
				u1s_touritu = 1;							/* トレースON */
				f4g_forward = 20;
			    balance_init();    						/* balance APIの初期化 */
			    nxt_motor_set_count(PORT_MOTOR_L, 0);    /* 左モータ・カウント値[°]を0リセット */
    			nxt_motor_set_count(PORT_MOTOR_R, 0);    /* 右モータ・カウント値[°]を0リセット */
				s4t_motor_L2 = nxt_motor_get_count(PORT_MOTOR_L);
				s4t_motor_R2 = nxt_motor_get_count(PORT_MOTOR_R);
				u1g_seesaw_mode = 1;
				u2_count = 0;
				
			break;


//			case 51:
				
//						u1s_touritu = 0;			
						
//			break;

			
			case 1 :  /* 段差検知まで */

				s4t_distance = s4t_motor_L1 - s4t_motor_L2 + s4t_motor_R1 - s4t_motor_R2;

				if(s4t_distance < 50){					

					u1s_trace = 0;					/*トレースの設定*/				
					f4g_forward = 40;
		
					
				}else if(s4t_distance < 500){					
				
					u1s_trace = 1;					/*トレースの設定*/				
					f4g_forward = 40;
				
				}else{
					
					u1s_trace = 0;
					chokushin2();
				
					 if(status->gyroval > status->gyro_offset + 40 || status->gyroval < status->gyro_offset - 40){
				
						u1g_seesaw_mode++;
						f4g_forward = 100;
						s4t_motor_L2 = nxt_motor_get_count(PORT_MOTOR_L);
						s4t_motor_R2 = nxt_motor_get_count(PORT_MOTOR_R);
						u2_count = 0;
						u1s_trace = 0;
						u2_gyro_temp = robot.robotSensor.GYRO.gyroOffsetValue;
					
						//robot.robotSensor.GYRO.gyroOffsetValue = robot.robotSensor.GYRO.gyroOffsetValue -10;
					}
				
				}

			break;
			
			case 2 :  /* 検知後ある一定以上上る */
			
				s4t_distance = s4t_motor_L1 - s4t_motor_L2 + s4t_motor_R1 - s4t_motor_R2;

				if(s4t_distance < 20){			//ﾍﾞｰｽ100

					f4g_forward = 100;
					chokushin2();
					
				}else if(s4t_distance < 60){	//べーす３００

					robot.robotSensor.GYRO.gyroOffsetValue = u2_gyro_temp;
					f4g_forward = 40;
					chokushin2();
				
				}else{
				
					u2_count = 0;
					f4g_forward = 0;
					chokushin2();
					u1g_seesaw_mode = 3;
				}
			
			break;
						
			case 3 :  /* Bluetoothが来るまで待機 */

				f4g_forward = 0;
				chokushin2();

		    	u4_recieve = ecrobot_read_bt_packet( receive_data, 90 );
 
            	if ( u4_recieve > 0 )
            	{
	            	ecrobot_sound_tone(100, 100, 50);
					u2_count = 0;
					f4g_forward = 0;
					chokushin2();
					u1g_seesaw_mode++;
									
				}
				
			break;
			
			case 4 :  /* Bluetoothが来たから上る */


				s4t_distance = s4t_motor_L1 - s4t_motor_L2 + s4t_motor_R1 - s4t_motor_R2;
			
						
				if(s4t_distance < 1035){
				
					f4g_forward = 20;
					chokushin2();

				}else{
				
					robot.robotSensor.GYRO.gyroOffsetValue = robot.robotSensor.GYRO.gyroOffsetValue -15;
					u1g_seesaw_mode++;
//					ecrobot_sound_tone(100, 100, 50);

					
				}
				
			break;
			
			case 5 :
			
				if(status->gyroval < status->gyro_offset - 80){
					
					robot.robotSensor.GYRO.gyroOffsetValue = u2_gyro_temp;
					u1g_seesaw_mode++;
					nxt_motor_set_count(PORT_MO_TAIL, 0);
					s4t_motor_L3 =nxt_motor_get_count(PORT_MOTOR_L);
					s4t_motor_R3 =nxt_motor_get_count(PORT_MOTOR_R);
				
					
				}else{
					
					f4g_forward = 20;
					chokushin2();
					
				}
			
			break;
			
			case 6 :  /* Bluetoothが来たから上る */
				
				s4t_motor_L4 =s4t_motor_L3;
				s4t_motor_R4 =s4t_motor_R3;
				s4t_motor_L3 =nxt_motor_get_count(PORT_MOTOR_L);
				s4t_motor_R3 =nxt_motor_get_count(PORT_MOTOR_R);
				
				if((s4t_motor_R3 - s4t_motor_R4) < 0 && (s4t_motor_L3 - s4t_motor_L4)){
					
	//				u1s_touritu = 0;
	//				left = 0;
	//				right = 0;
	//				f4g_forward = 0;
	//				f4g_turn = 0;
					u1g_seesaw_mode++;
					
				}else{
					
					f4g_forward = -60;
					chokushin2();
					
				}

			break;

			case 7 :
			
				s4t_distance = s4t_motor_L1 - s4t_motor_L2 + s4t_motor_R1 - s4t_motor_R2;



				if(s4t_distance > 1050){
				
					f4g_forward = -20;
					chokushin2();

				}else{
				
					u1g_seesaw_mode++;
	//				ecrobot_sound_tone(100, 100, 50);
					
				}


			case 8 :

					u1s_touritu = 0;
					left = 0;
					right = 0;
					f4g_forward = 0;
					f4g_turn = 0;

			
			break;
			
		}


        if (u1s_trace == 1)
       {
		
        f4t_yp = (f4)(GRAY_WHITE - status->lightval );
        f4t_yd = (f4t_yp - f4s_last_yp) / 0.004;
        f4s_last_yp = f4t_yp;
        f4g_turn = (f4)((f4)TRACE_P * f4t_yp) + (f4)((f4)TRACE_D * f4t_yd);
//        f4g_turn = -f4g_turn;
        /* 旋回方向決定 */
        
        if(f4g_turn > 50){
			
			f4g_turn = 50;
			
		}else if(f4g_turn < -50){
			
			f4g_turn = -50;
			
		}else{
			
			f4g_turn = f4g_turn;
			
		}

		f4g_turn = -f4g_turn;


		}

        if (u1s_touritu == 1)
        {
        balance_control(
            (f4)f4g_forward,                          /* 前後進命令(+:前進, -:後進) */
            (f4)f4g_turn,                    /* 旋回命令(+:右旋回, -:左旋回) */
//            (f4)-6,                    	/* 旋回命令(+:右旋回, -:左旋回) */
            (f4)status->gyroval,             /* ジャイロセンサ値 */
            (f4)status->gyro_offset,         /* ジャイロセンサオフセット値 */
            (f4)status->motorangle_L,        /* 左モータ回転角度[deg] */
            (f4)status->motorangle_R,        /* 右モータ回転角度[deg] */
            (f4)status->batteryval,          /* バッテリ電圧[mV] */
            &left,                           /* 左モータPWM出力値 */
            &right                           /* 右モータPWM出力値 */
        );

	        setMotorVal( left , right , -10);

                
        }else{
			
			tail_seesaw = tailControl( 40, nxt_motor_get_count(PORT_MO_TAIL));
			setMotorVal( left , right , tail_seesaw );
			
		}

        
        
    }


}

void seesaw_go_pair( Command* cmd , ROBOT_STATUS* status )
{
	
}





void setMotorVal( signed char left , signed char right , signed char tail)

{
    motorvalue.leftmotor = left;
    motorvalue.rightmotor = right;
    motorvalue.tailmotor = tail;
}


/*---------------------------*/
/* 直進制御                  */
/*---------------------------*/
void chokushin2()
{

	static u1 flag=0;
	
    static  s4  s4t_motor_Lc[10], s4t_motor_Rc[10];
    
    s1  i;
    
    if(flag==0)
    {
		
    	for( i = 9; i > -1; i--)
    	{
			s4t_motor_Lc[i]=nxt_motor_get_count(PORT_MOTOR_L);
			s4t_motor_Rc[i]=nxt_motor_get_count(PORT_MOTOR_R);
		}
		flag++;
	}
    
    for( i = 9; i > 0; i--)
    {
		s4t_motor_Lc[i]=s4t_motor_Lc[i-1];
		s4t_motor_Rc[i]=s4t_motor_Rc[i-1];
	}
    	
	
    s4t_motor_Lc[0] = nxt_motor_get_count(PORT_MOTOR_L);
    s4t_motor_Rc[0] = nxt_motor_get_count(PORT_MOTOR_R);


    /* 直進開始地点からの左右回転数差に応じてturn */
    s4t_motor_diff =  -(s4t_motor_Rc[9] - s4t_motor_Rc[0]) + (s4t_motor_Lc[9] - s4t_motor_Lc[0]);

	f4g_turn    = f4g_turn + K * s4t_motor_diff;
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