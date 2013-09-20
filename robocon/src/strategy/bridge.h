#ifndef BRIDGE_H
#define BRIDGE_H

#include "../robot/robot_state.h"
#include "../command/command.h"

void bridge_ride( Command* cmd , ROBOT_STATUS* status );
void bridge_go( Command* cmd , ROBOT_STATUS* status );
void bridge_forward_pair( Command* cmd , ROBOT_STATUS* status );
void bridge_backward_pair( Command* cmd , ROBOT_STATUS* status );

int u4g_dispatch_func_bridge_ride(); 
int u4g_dispatch_func_bridge_go(); 

#endif
