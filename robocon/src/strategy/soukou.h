
#ifndef SOUKOU_H
#define SOUKOU_H

#include "../robot/robot_state.h"
#include "../command/command.h"

void setBasicStageTargetValue( Command* cmd , ROBOT_STATUS* status );   /* �x�[�V�b�N���s�p */
void setBasicToPerTargetValue( Command* cmd , ROBOT_STATUS* status );   /* ��~�p */
void setStopTargetValue( Command* cmd , ROBOT_STATUS* status );         /* DUMMY */

#endif
