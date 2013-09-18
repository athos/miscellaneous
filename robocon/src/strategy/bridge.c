
#include "../robot/robot.h"
#include "./controlval.h"
#include "./bridge.h"
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

#define  GRAY_WHITE     (( ( robot.robotSensor.LIGHT.white  + robot.robotSensor.LIGHT.black ) / 2 + robot.robotSensor.LIGHT.white ) /2)

typedef signed char    s1;
typedef signed short   s2;
typedef signed long    s4;
typedef unsigned char  u1;
typedef unsigned short u2;
typedef unsigned long  u4;
typedef float          f4;
typedef double         f8;

s1 tailControl( unsigned char tailAngletarget , unsigned char tailAngle );
void soukou_tail_bridge(u2, f4, f4, f4, u1); 
void setMotorVal( signed char , signed char , signed char );
void chokushin();

static s1 left;
static s1 right;
static f4 f4t_motor_L,f4t_motor_R;
static u2 u2g_light_data = 0;



void bridge_ride( Command* cmd , ROBOT_STATUS* status )
{

	static u1 u1s_2msflipcount_bridge = 1;
	static u1 u1g_bridge_mode;
	static s1 tail_bridge = 0;
	static u1 trace_bridge = 0;
	static f4 f4t_forward = 0;
	static s4 s4t_motor_bridge_L1 = 0;
	static s4 s4t_motor_bridge_R1 = 0;
	static s4 s4t_motor_bridge_L2 = 0;
	static s4 s4t_motor_bridge_R2 = 0;
	static s4 s4t_distance_bridge = 0;

    static u4 u4_recieve_bridge = 0;
  	u1  receive_data_bridge[90] = {0}; 

	
	s4t_motor_bridge_L1 = nxt_motor_get_count(PORT_MOTOR_L);
	s4t_motor_bridge_R1 = nxt_motor_get_count(PORT_MOTOR_R);

  /* user logic start */
    u1s_2msflipcount_bridge = 1 - u1s_2msflipcount_bridge;
    
   	u2g_light_data = status->lightval;

    if ( u1s_2msflipcount_bridge == 1 )
    {

		switch(u1g_bridge_mode){       /* シーソーシーケンス開始 */
   
   			case 0 :  /* 開始 */

				f4t_forward = 0;
				u1g_bridge_mode++;
				s4t_motor_bridge_L2 = nxt_motor_get_count(PORT_MOTOR_L);
				s4t_motor_bridge_R2 = nxt_motor_get_count(PORT_MOTOR_R);

			break;
			
   			case 1 :  /* 開始 */

				s4t_distance_bridge = (s4t_motor_bridge_L1 - s4t_motor_bridge_L2) + (s4t_motor_bridge_R1 - s4t_motor_bridge_R2);

				trace_bridge = 1;
				f4t_forward = 40;
				
				if(s4t_distance_bridge > 500){
					
					f4t_forward = 40;
					u1g_bridge_mode++;
					
				}
				
			break;
			
			case 2 :
			
				f4t_forward = 0;
				left = 0;
				right = 0;

				trace_bridge = 0;

		    	u4_recieve_bridge = ecrobot_read_bt_packet( receive_data_bridge, 90 );
 
            	if ( u4_recieve_bridge > 0 )
            	{
	            	ecrobot_sound_tone(100, 100, 50);
					s4t_motor_bridge_L2 = nxt_motor_get_count(PORT_MOTOR_L);
					s4t_motor_bridge_R2 = nxt_motor_get_count(PORT_MOTOR_R);
					u1g_bridge_mode++;
					
				}
													
			break;
			
			case 3:
			
				s4t_distance_bridge = (s4t_motor_bridge_L1 - s4t_motor_bridge_L2) + (s4t_motor_bridge_R1 - s4t_motor_bridge_R2);

				trace_bridge = 0;
				left = 40;
				right = 40;
				
				if(s4t_distance_bridge > 500){
					
					left = 0;
					right = 0;
					u1g_bridge_mode++;
					
				}
			
			break;
			
			case 4 :
			
				f4t_forward = 0;
				left = 0;
				right = 0;

				trace_bridge = 0;

		    	u4_recieve_bridge = ecrobot_read_bt_packet( receive_data_bridge, 90 );
 
            	if ( u4_recieve_bridge > 0 )
            	{
	            	ecrobot_sound_tone(100, 100, 50);
					s4t_motor_bridge_L2 = nxt_motor_get_count(PORT_MOTOR_L);
					s4t_motor_bridge_R2 = nxt_motor_get_count(PORT_MOTOR_R);
					u1g_bridge_mode++;
					
				}
													
			break;
			
			case 5:
			
				s4t_distance_bridge = (s4t_motor_bridge_L1 - s4t_motor_bridge_L2) + (s4t_motor_bridge_R1 - s4t_motor_bridge_R2);

				trace_bridge = 0;
				left = 40;
				right = 40;
				
				if(s4t_distance_bridge > 2000){
					
					left = 0;
					right = 0;
					u1g_bridge_mode++;
					
				}
			
			break;
						
   			case 6 :  /* 開始 */

				trace_bridge = 1;
				f4t_forward = 40;
				
			break;			
			
		}
			
	}

	tail_bridge = tailControl( 100, status->motorangle_T);
   
	if(trace_bridge == 1){
		
		soukou_tail_bridge(GRAY_WHITE, 0.3, 0, 0.7, f4t_forward);
		setMotorVal( f4t_motor_L , f4t_motor_R , tail_bridge );

	}else{

		chokushin();
		setMotorVal( left , right , tail_bridge );
		
	}
	
		   
}


void bridge_go( Command* cmd , ROBOT_STATUS* status )
{
}

void bridge_forward_pair( Command* cmd , ROBOT_STATUS* status )
{

	static u1 u1g_bridge_mode;


	static u1 u1s_2msflipcount_for = 1;
	static s4 s4t_motor_pair_L1 = 0;
	static s4 s4t_motor_pair_R1 = 0;
	static s4 s4t_motor_pair_L2 = 0;
	static s4 s4t_motor_pair_R2 = 0;
	static s4 s4t_distance_pair = 0;
	static u2 u2_count_pair = 0;

	s4t_motor_pair_L1 = nxt_motor_get_count(PORT_MOTOR_L);
	s4t_motor_pair_R1 = nxt_motor_get_count(PORT_MOTOR_R);


  /* user logic start */
    u1s_2msflipcount_for = 1 - u1s_2msflipcount_for;

    display_clear(0);
    display_goto_xy(0, 0);  display_string("stage");  display_unsigned(u1g_bridge_mode,3);
    display_goto_xy(0, 1);  display_string("distance");  display_unsigned(s4t_distance_pair,3);

    display_update(); 


    if ( u1s_2msflipcount_for == 1 )
    {

		switch(u1g_bridge_mode)       /* シーソーシーケンス開始 */
		{
			case 0 :  /* 開始 */
		
				left  = 0;
				right = 0;
				u1g_bridge_mode++;
				s4t_motor_pair_L2 = nxt_motor_get_count(PORT_MOTOR_L);
				s4t_motor_pair_R2 = nxt_motor_get_count(PORT_MOTOR_R);

			break;
			
			case 1 :  /* 前進 */
								
				if((s4t_motor_pair_L1 - s4t_motor_pair_L2) > -1000){
					left  = -40;
				}else{
					left  = 0;
				}
				
				if((s4t_motor_pair_R1 - s4t_motor_pair_R2) > -1000){
					right  = -40;
				}else{
					right  = 0;	
				}

				chokushin();

				if(((s4t_motor_pair_L1 - s4t_motor_pair_L2) < -1000) && ((s4t_motor_pair_R1 - s4t_motor_pair_R2) < -1000)){
				
					u1g_bridge_mode++;
					left = 0;
					right = 0;
				}
		
			break;
			
			case 2 :
			
				left  = 0;
				right = 0;
				u2_count_pair++;
				
				if(u2_count_pair > 1000){
					
					u1g_bridge_mode++;
					u2_count_pair = 0;
				}
			
			break;
			
			case 3 :  /* 後退 */
		
				if((s4t_motor_pair_L1 - s4t_motor_pair_L2) < 0){
				
					left  = 40;
					
				}else{
					
					left  = 0;
					
				}

				if((s4t_motor_pair_R1 - s4t_motor_pair_R2) < 0){
				
					right  = 40;
					
				}else{
					
					right  = 0;
					
				}

				chokushin();

				if(((s4t_motor_pair_L1 - s4t_motor_pair_L2) > 0) && ((s4t_motor_pair_R1 - s4t_motor_pair_R2) > 0)){
				
					u1g_bridge_mode++;
					left = 0;
					right = 0;
				}		
			break;

			case 4 :
			
				left  = 0;
				right = 0;
				u2_count_pair++;
				
				if(u2_count_pair > 1000){
					
					u1g_bridge_mode = 1;
					u2_count_pair = 0;
				}
			
			break;



		}

        
        setMotorVal( left , right , -10 );
        
    }


}


void bridge_backward_pair( Command* cmd , ROBOT_STATUS* status )
{
}





void soukou_tail_bridge(u2 u2t_light, f4 f4t_Kp, f4 f4t_Ki, f4 f4t_Kd, u1 u1t_forward)
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
	f4t_turn = f4t_turn * -1;	/*右エッジ*/


//	f4t_turn = adjust_turn(f4t_turn);
	
	f4t_motor_L = (f4)u1t_forward + f4t_turn;
	f4t_motor_R = (f4)u1t_forward - f4t_turn;

	// モータ出力をガードする
//	s1g_motor_L = (s1)GUARD(f4t_motor_L, -100, 100);
//	s1g_motor_R = (s1)GUARD(f4t_motor_R, -100, 100);
	

    f4s_last_yp = f4t_yp;
    f4s_y_hist[hist_index] = f4t_yp;
    hist_index = (hist_index + 1) % NUM_HISTORIES;
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


void setMotorVal( signed char left , signed char right , signed char tail)

{
    motorvalue.leftmotor = left;
    motorvalue.rightmotor = right;
    motorvalue.tailmotor = tail;
}


/*---------------------------*/
/* 直進制御                  */
/*---------------------------*/
void chokushin()
{
    s4 s4t_motor_diff;
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
    	
	
//    s4t_motor_Lc[0] = nxt_motor_get_count(PORT_MOTOR_L);
//    s4t_motor_Rc[0] = nxt_motor_get_count(PORT_MOTOR_R);


    /* 直進開始地点からの左右回転数差に応じてturn */
    s4t_motor_diff = - (s4t_motor_Rc[9] - s4t_motor_Rc[0]) + (s4t_motor_Lc[9] - s4t_motor_Lc[0]);

	left  =  left - s4t_motor_diff * K;
	right = right + s4t_motor_diff * K;
		
}

