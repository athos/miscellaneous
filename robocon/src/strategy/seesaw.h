#ifndef SEESAW_H
#define SEESAW_H

#include "../robot/robot_state.h"
#include "../command/command.h"

void seesaw_ride( Command* cmd , ROBOT_STATUS* status );
void seesaw_ride_pair( Command* cmd , ROBOT_STATUS* status );
void seesaw_go( Command* cmd , ROBOT_STATUS* status );
void seesaw_go_pair( Command* cmd , ROBOT_STATUS* status );

int u4g_dispatch_func_seesaw_ride(); 
int u4g_dispatch_func_seesaw_ride_pair(); 

#endif
