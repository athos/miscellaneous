///////////////////////////////////////////////////////////
//  model_impl.c
///////////////////////////////////////////////////////////

#include "kernel.h"
#include "../osek/kernel_id.h"

#include "./robot/robot.h"

// オブジェクトを静的に確保する

#define BT_PASS_KEY     "teamKGK"

ROBOT robot;
ROBOT_STATUS robot_status;
// デバイス初期化用フック関数
// この関数はnxtOSEK起動時に実行されます。
void ecrobot_device_initialize()
{
	// センサ、モータなどの各デバイスの初期化関数を
	// ここで実装することができます
	// ⇒　光センサ赤色LEDをONにする
    startrobot( &robot );
    // BTの初期化関数
//    ecrobot_init_bt_slave(BT_PASS_KEY);

}

// デバイス終了用フック関数
// この関数はSTOPまたはEXITボタンが押された時に実行されます。
void ecrobot_device_terminate()
{
	// センサ、モータなどの各デバイスの終了関数を
	// ここで実装することができます。
	// ⇒　光センサ赤色LEDをOFFにする
    stoprobot( &robot );

}
DeclareCounter(SysTimerCnt);

// 1msec周期割り込み(ISRカテゴリ2)から起動されるユーザー用フック関数
void user_1ms_isr_type2(void)
{
    (void)SignalCounter(SysTimerCnt);

}

TASK(Task_Main)
{

    initrobot( &robot );
    

//    balance_init();    /* balance APIの初期化 */

    TerminateTask();
}


