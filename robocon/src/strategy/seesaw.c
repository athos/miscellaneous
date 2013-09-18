
#include "../robot/robot.h"
#include "./controlval.h"
#include "./seesaw.h"
#include "balancer.h"

#define P_GAIN             2.5F /* ���S��~�p���[�^������W�� */
#define PWM_ABS_MAX          50 /* ���S��~�p���[�^����PWM��΍ő�l */
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

		switch(u1g_seesaw_mode)       /* �V�[�\�[�V�[�P���X�J�n */
		{
		
			case 0 :  /* �|���J�n */
		
			//	u1s_trace = 1;							 /* �g���[�XOFF */
				u1s_touritu = 1;							/* �g���[�XON */
				f4g_forward = 20;
			    balance_init();    						/* balance API�̏����� */
			    nxt_motor_set_count(PORT_MOTOR_L, 0);    /* �����[�^�E�J�E���g�l[��]��0���Z�b�g */
    			nxt_motor_set_count(PORT_MOTOR_R, 0);    /* �E���[�^�E�J�E���g�l[��]��0���Z�b�g */
				s4t_motor_L2 = nxt_motor_get_count(PORT_MOTOR_L);
				s4t_motor_R2 = nxt_motor_get_count(PORT_MOTOR_R);
				u1g_seesaw_mode = 1;
				u2_count = 0;
				
			break;


//			case 51:
//						f4g_forward = 40;			
						
//			break;

			
			case 1 :  /* �i�����m�܂� */

				s4t_distance = s4t_motor_L1 - s4t_motor_L2 + s4t_motor_R1 - s4t_motor_R2;

				if(s4t_distance < 50){					

					u1s_trace = 0;					/*�g���[�X�̐ݒ�*/				
					f4g_forward = 40;
		
					
				}else if(s4t_distance < 500){					
				
					u1s_trace = 1;					/*�g���[�X�̐ݒ�*/				
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
			
			case 2 :  /* ���m�゠����ȏ��� */
			
				s4t_distance = s4t_motor_L1 - s4t_motor_L2 + s4t_motor_R1 - s4t_motor_R2;

				if(s4t_distance < 100){			//�ް�100

					f4g_forward = 100;
					chokushin2();
					
				}else if(s4t_distance < 300){	//�ׁ[���R�O�O

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
						
			case 3 :  /* Bluetooth������܂őҋ@ */

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
			
			case 4 :  /* Bluetooth������������ */
			
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
			
			case 5 :  /* �i�����m */
		
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
			
			case 6 :  /* ���C�����A */

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

			case 7 :  /* ���C�����A */
			
			if(	s4t_motor_L1 - s4t_motor_L3 > 50){
				
				f4g_forward = -20;
				f4g_turn = f4g_forward;
				
			}else{
					
				f4g_turn = 0;
				f4g_forward = 0;
				u1g_seesaw_mode = 9;
				
				}
				
			break;			

			case 8 :  /* ���C�����A */

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
        /* ����������� */
        
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
            (f4)f4g_forward,                          /* �O��i����(+:�O�i, -:��i) */
            (f4)f4g_turn,                    /* ���񖽗�(+:�E����, -:������) */
//            (f4)-6,                    	/* ���񖽗�(+:�E����, -:������) */
            (f4)status->gyroval,             /* �W���C���Z���T�l */
            (f4)status->gyro_offset,         /* �W���C���Z���T�I�t�Z�b�g�l */
            (f4)status->motorangle_L,        /* �����[�^��]�p�x[deg] */
            (f4)status->motorangle_R,        /* �E���[�^��]�p�x[deg] */
            (f4)status->batteryval,          /* �o�b�e���d��[mV] */
            &left,                           /* �����[�^PWM�o�͒l */
            &right                           /* �E���[�^PWM�o�͒l */
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

		switch(u1g_seesaw_mode)       /* �V�[�\�[�V�[�P���X�J�n */
		{
		
			case 0 :  /* �|���J�n */
		
			//	u1s_trace = 1;							 /* �g���[�XOFF */
				u1s_touritu = 1;							/* �g���[�XON */
				f4g_forward = 20;
			    balance_init();    						/* balance API�̏����� */
			    nxt_motor_set_count(PORT_MOTOR_L, 0);    /* �����[�^�E�J�E���g�l[��]��0���Z�b�g */
    			nxt_motor_set_count(PORT_MOTOR_R, 0);    /* �E���[�^�E�J�E���g�l[��]��0���Z�b�g */
				s4t_motor_L2 = nxt_motor_get_count(PORT_MOTOR_L);
				s4t_motor_R2 = nxt_motor_get_count(PORT_MOTOR_R);
				u1g_seesaw_mode = 1;
				u2_count = 0;
				
			break;


//			case 51:
				
//						u1s_touritu = 0;			
						
//			break;

			
			case 1 :  /* �i�����m�܂� */

				s4t_distance = s4t_motor_L1 - s4t_motor_L2 + s4t_motor_R1 - s4t_motor_R2;

				if(s4t_distance < 50){					

					u1s_trace = 0;					/*�g���[�X�̐ݒ�*/				
					f4g_forward = 40;
		
					
				}else if(s4t_distance < 500){					
				
					u1s_trace = 1;					/*�g���[�X�̐ݒ�*/				
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
			
			case 2 :  /* ���m�゠����ȏ��� */
			
				s4t_distance = s4t_motor_L1 - s4t_motor_L2 + s4t_motor_R1 - s4t_motor_R2;

				if(s4t_distance < 20){			//�ް�100

					f4g_forward = 100;
					chokushin2();
					
				}else if(s4t_distance < 60){	//�ׁ[���R�O�O

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
						
			case 3 :  /* Bluetooth������܂őҋ@ */

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
			
			case 4 :  /* Bluetooth������������ */


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
			
			case 6 :  /* Bluetooth������������ */
				
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
        /* ����������� */
        
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
            (f4)f4g_forward,                          /* �O��i����(+:�O�i, -:��i) */
            (f4)f4g_turn,                    /* ���񖽗�(+:�E����, -:������) */
//            (f4)-6,                    	/* ���񖽗�(+:�E����, -:������) */
            (f4)status->gyroval,             /* �W���C���Z���T�l */
            (f4)status->gyro_offset,         /* �W���C���Z���T�I�t�Z�b�g�l */
            (f4)status->motorangle_L,        /* �����[�^��]�p�x[deg] */
            (f4)status->motorangle_R,        /* �E���[�^��]�p�x[deg] */
            (f4)status->batteryval,          /* �o�b�e���d��[mV] */
            &left,                           /* �����[�^PWM�o�͒l */
            &right                           /* �E���[�^PWM�o�͒l */
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
/* ���i����                  */
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


    /* ���i�J�n�n�_����̍��E��]�����ɉ�����turn */
    s4t_motor_diff =  -(s4t_motor_Rc[9] - s4t_motor_Rc[0]) + (s4t_motor_Lc[9] - s4t_motor_Lc[0]);

	f4g_turn    = f4g_turn + K * s4t_motor_diff;
}

s1
tailControl( unsigned char tailAngletarget , unsigned char tailAngle )
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