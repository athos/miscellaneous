
#include "../robot/robot_state.h"
#include "../robot/robot.h"
#include "./soukou.h"
#include "./soukou_touritsu.h"
#include "./performancepart2.h"
#include "./seesaw.h"
#include "./bridge.h"
#include "./obstacle.h"
#include "./recv.h"
#include "../command/command_factory.h"
#include "../command/encoder.h"
#include "../command/bluetooth.h"
#include "./dispatch.h"

#include "./test_strategy.h"

typedef void (*fctPtr)( Command* , ROBOT_STATUS* );
#define NULL (void *)0

#define SPEEDBUFFSIZE 10
int s4s_dispatch_speed[SPEEDBUFFSIZE] = {100};
static unsigned char u1s_speedptr = 0;

const fctPtr commandFuncPtrTbl[] =  /* cmd_typeはindex値 */
{
  NULL                               /* ローカル⇔リモート */
  //,test_strategy
  ,&setBasicStageTargetValue         /* ベーシック走行コマンド */
  ,&setBasicToPerTargetValue         /* 切り替え過渡コマンド（停止） */
//  ,&setSoukouTouritsu                /* 倒立ベーシック */
//  ,&setStopTargetValue               /* 切り替えコマンド */ /* シーソーのコマンドを入れ替えてください */
  ,&recv_4ms                         /* 倒立切り替え */
  ,&seesaw_ride                        /* 難所シーソーに乗る */
//  ,&seesaw_ride_pair               /* 難所シーソーに乗る(ペア) */
  ,&seesaw_go                        /* 難所シーソーを攻略 */
//  ,&seesaw_go_pair                     /* 難所シーソーを攻略（ペア） */
  ,&setPerformanceStagePart2         /* 難所カーブ */
//  ,&bridge_ride                      /* 難所橋に乗る */
//  ,&bridge_go                        /* 難所橋を攻略 */
//  ,&obstacle                         /* 難所障害物 */
//  ,&bridge_forward_pair              /* 難所橋を前に運ぶ */
//  ,&bridge_backward_pair             /* 難所橋を後ろに運ぶ */
  
};

unsigned char judge( void );
unsigned char JudgeFunc( Condition* );
unsigned char condComp( int cond_val , int cond_type );
int s4s_dispatch_speedsum( short s2t_speed );

static Command* nowCmd = NULL;

#define CMD_EXIST   (0)
#define NOCMD       (1)
#define CMD_END     (2)


#define COND_MASK           (0x7)
/* 3bit */
#define COND_TYPE_AND       (0x0)
#define COND_TYPE_OR        (0x1)
#define COND_TYPE_SINGLE    (0x2)

/* 3bit */
#define COND_TYPE_EQUAL     (0x0)
#define COND_TYPE_NOTEQUAL  (0x1)
#define COND_TYPE_LARGE     (0x2)
#define COND_TYPE_SMALL     (0x3)
#define COND_TYPE_UP        (0x4)
#define COND_TYPE_DOWN      (0x5)

/* 3bit */
#define COND_TYPE_DIST        (0x0)
#define COND_TYPE_GYRO        (0x1)
#define COND_TYPE_SPEED       (0x2)
#define COND_TYPE_RECV        (0x3)
#define COND_TYPE_SEESAW_RIDE (0x4)
#define COND_TYPE_TAIL        (0x5)
#define COND_TYPE_BRIDGE_RIDE      (0x6)
#define COND_TYPE_BRIDGE_GO        (0x8)
#define COND_TYPE_SEESAW_RIDE_PAIR (0x7)
#define COND_TYPE_OBSTACLE         (0x9)

void dispatch_main()
{
    unsigned char result;
    //byte_t data[64];

    updateRobotStatus( &robot , &robot_status );    /* 走行体状態取得 */
	result = judge();                               /* コマンド終了かを判断 */

    //if (result == CMD_END && COMMAND_FACTORY->mode == REMOTE_CONTROL_MODE) {
    //    encodeCommand(ENCODER, nowCmd, data, sizeof(data));
    //    sendData(data);
    //    return;
    //}

    if ( result != CMD_EXIST )                      /* コマンド終了 or コマンド無し */
    {
        nowCmd = getNextCommand( COMMAND_FACTORY ); /* 次のコマンドを取る */
    }

    if ( nowCmd != NULL )
    {
        commandFuncPtrTbl[nowCmd->cmd_type]( nowCmd , &robot_status );
    }
}

unsigned char judge( void )
{
    unsigned char ret;
    unsigned char ret_tmp;

    Condition*  localCondPtr;

    if ( nowCmd != NULL )
    {
        localCondPtr = nowCmd->cmd_cond;

        ret_tmp = JudgeFunc( localCondPtr );
        if ( ret_tmp == 0 )            /* 終了条件不満足 */
        {
            ret = CMD_EXIST;
        }
        else
        {
            ret = CMD_END;
        }
    }
    else
    {
       ret = NOCMD;
    }

    return ret;
}

unsigned char condComp( int cond_val , int cond_type )
{
    unsigned char ret;
    unsigned char tmp1;
    unsigned char tmp2;
    int cmd_val;

    ret = 0;
    tmp1 = ( cond_type >> 3 ) & ( 0x07 );
    tmp2 = ( cond_type >> 6 ) & ( 0x0f );
    cmd_val = 0;

    switch( tmp2 )
    {
       case COND_TYPE_DIST: cmd_val = (int)( robot_status.motorangle_L + robot_status.motorangle_R ) >> 1;
                            break;

       case COND_TYPE_GYRO: cmd_val = (int)( robot_status.gyroval - robot.robotSensor.GYRO.gyroOffsetValue );
                            break;

       case COND_TYPE_SPEED: cmd_val = (int)s4s_dispatch_speedsum(robot_status.motorSpeed);
                            break;

       case COND_TYPE_RECV: cmd_val = (int)u4g_dispatch_func_recv();
                            break;

       case COND_TYPE_SEESAW_RIDE: cmd_val = (int)u4g_dispatch_func_seesaw_ride();
                            break;

       case COND_TYPE_SEESAW_RIDE_PAIR: cmd_val = (int)u4g_dispatch_func_seesaw_ride_pair();
                            break;
                            
       case COND_TYPE_TAIL: cmd_val = (int)robot_status.motorangle_T;
                            break;
                            
       case COND_TYPE_BRIDGE_RIDE: cmd_val = (int)u4g_dispatch_func_bridge_ride();
                            break;

       case COND_TYPE_BRIDGE_GO: cmd_val = (int)u4g_dispatch_func_bridge_go();
                            break;


       default:             break;
    }

    switch( tmp1 )
    {
        case COND_TYPE_EQUAL: if( cmd_val == cond_val ) ret = 1;
                              break;

        case COND_TYPE_NOTEQUAL: 
                              if( cmd_val != cond_val ) ret = 1;
                              break;

        case COND_TYPE_LARGE: if( cmd_val > cond_val )  ret = 1;
                              break;

        case COND_TYPE_SMALL: if( cmd_val < cond_val )  ret = 1;
                              break;

        case COND_TYPE_UP:    if( cmd_val >= cond_val ) ret = 1;
                              break;

        case COND_TYPE_DOWN:  if( cmd_val <= cond_val ) ret = 1;
                              break;

        default:              break;
    }
    return ret;

}

unsigned char JudgeFunc( Condition* localCondPtr )
{
    unsigned char ret;
    unsigned char ret_tmp1;
    unsigned char ret_tmp2;
    Condition*  localCondPtrLeft;
    Condition*  localCondPtrRight;

    ret = 0;
    if ( ((localCondPtr->cond_type) & COND_MASK) == COND_TYPE_SINGLE )
    {
       ret = condComp( COND_VALUE( localCondPtr ) , localCondPtr->cond_type );
    }
    else
    {
        localCondPtrLeft  = LEFT_SUBCOND(localCondPtr);
        localCondPtrRight = RIGHT_SUBCOND(localCondPtr);

        if ( ((localCondPtrLeft->cond_type) & COND_MASK) != COND_TYPE_SINGLE )
        {
            ret_tmp1 = JudgeFunc( localCondPtrLeft );
        }
        else
        {
            ret_tmp1 = condComp( COND_VALUE( localCondPtrLeft )  , localCondPtrLeft->cond_type );
        }

        if ( ((localCondPtrRight->cond_type) & COND_MASK) != COND_TYPE_SINGLE )
        {
            ret_tmp2 = JudgeFunc( localCondPtrRight );
        }
        else
        {
            ret_tmp2 = condComp( COND_VALUE( localCondPtrRight ) , localCondPtrRight->cond_type );
        }

        if ( ((localCondPtr->cond_type) & COND_MASK) == COND_TYPE_AND )
        {
            ret = (ret_tmp1 && ret_tmp2);
        }
        else if ( ((localCondPtr->cond_type) & COND_MASK) == COND_TYPE_OR )
        {
            ret = (ret_tmp1 || ret_tmp2);
        }
    }
    return ret;
}

int s4s_dispatch_speedsum( short s2t_speed )
{
    int sum;
    unsigned char i;
    s4s_dispatch_speed[u1s_speedptr] = s2t_speed;
    
    /* データ挿入位置の更新 */
    u1s_speedptr = u1s_speedptr + 1;
    if ( u1s_speedptr >= SPEEDBUFFSIZE )
    {
        u1s_speedptr = 0;
    }
    
    /* 合計算出 */
    sum = 0;
    for( i = 0; i < SPEEDBUFFSIZE; i++ )
    {
        sum += s4s_dispatch_speed[i];
    }
    
    return sum;
    
}
