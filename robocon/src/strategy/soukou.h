
#ifndef SOUKOU_H
#define SOUKOU_H

#include "../robot/robot_state.h"
#include "../command/command.h"

void setBasicStageTargetValue( Command* cmd , ROBOT_STATUS* status );   /* ベーシック走行用 */
void setBasicToPerTargetValue( Command* cmd , ROBOT_STATUS* status );   /* 停止用 */
void setStopTargetValue( Command* cmd , ROBOT_STATUS* status );         /* DUMMY */

#endif
