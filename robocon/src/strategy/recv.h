/* recv.h */

#ifndef _RECV_H_
#define _RECV_H_


/*------------*/
/* ���J�}�N�� */
/*------------*/

/*----------*/
/* ���J�ϐ� */
/*----------*/
#include "../robot/robot_state.h"
#include "../command/command.h"


/*----------*/
/* ���J�֐� */
/*----------*/
void recv_4ms(Command* cmd , ROBOT_STATUS* status);


int u4g_dispatch_func_recv( void );

#endif
