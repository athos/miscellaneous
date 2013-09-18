#ifndef COMMAND_FACTORY_H
#define COMMAND_FACTORY_H

#include "command.h"

typedef enum _ControlMode {
	LOCAL_CONTROL_MODE,
	REMOTE_CONTROL_MODE
} ControlMode;

typedef struct _CommandFactory {
	ControlMode mode;
	Command *cmd_ptr;
	Command *cmd_local_buf;
	Command *cmd_remote_buf;
} CommandFactory;

extern CommandFactory *COMMAND_FACTORY;

void initCommandFactory(CommandFactory *factory, ControlMode mode);
Command *getNextCommand(CommandFactory *factory);
void setControlMode(CommandFactory *factory, ControlMode mode);

#endif /* COMMAND_FACTORY_H */
