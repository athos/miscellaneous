#include "command_factory.h"
#include "decoder.h"
#include "bluetooth.h"
#include "byte_buffer.h"

extern Command cmd_local_buf[];
Command cmd_remote_buf[1];

static CommandFactory defaultCommandFactory = {LOCAL_CONTROL_MODE, cmd_local_buf, cmd_local_buf, cmd_remote_buf};
CommandFactory *COMMAND_FACTORY = &defaultCommandFactory;

void initCommandFactory(CommandFactory *this, ControlMode mode) {
	setControlMode(this, mode);
}

Command *getNextCommand(CommandFactory *this) {
	Command *cmd;
	static byte_t data[64];

	if (this->mode == LOCAL_CONTROL_MODE) {
		cmd = this->cmd_ptr;
		if (cmd->cmd_type == 0) {
			setControlMode(this, REMOTE_CONTROL_MODE);
			return getNextCommand(this);
		} else {
			this->cmd_ptr++;
		}
	} else if (receiveData(data)) {
		decodeCommand(DECODER, data, this->cmd_remote_buf);
		cmd = this->cmd_ptr;
	} else {
		cmd = (void *)0;
	}

	return cmd;
}

void setControlMode(CommandFactory *this, ControlMode mode) {
	this->mode = mode;
	if (mode == LOCAL_CONTROL_MODE) {
		this->cmd_ptr = this->cmd_local_buf;
	} else {
		this->cmd_ptr = this->cmd_remote_buf;
		initDecoder(DECODER);
	}
}
