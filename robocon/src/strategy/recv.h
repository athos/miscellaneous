/* recv.h */

#ifndef _RECV_H_
#define _RECV_H_


/*------------*/
/* 公開マクロ */
/*------------*/

/*----------*/
/* 公開変数 */
/*----------*/
#include "../robot/robot_state.h"
#include "../command/command.h"


/*----------*/
/* 公開関数 */
/*----------*/
void recv_4ms(Command* cmd , ROBOT_STATUS* status);


int u4g_dispatch_func_recv( void );

#endif
