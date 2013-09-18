#include "command_factory.h"
#include "command_c.h"
#include "decoder.h"
#include "bluetooth.h"

extern Command cmd_local_buf[];
Command cmd_remote_buf[1];

static CommandFactory defaultCommandFactory = {
  LOCAL_CONTROL_MODE,
  cmd_local_buf,
  cmd_local_buf,
  cmd_remote_buf
};
CommandFactory *COMMAND_FACTORY = &defaultCommandFactory;

void initCommandFactory(CommandFactory *this, ControlMode mode) {
	setControlMode(this, mode);
}

Command *getNextCommand(CommandFactory *this) {
	Command *cmd;
  byte_t data[64];
  int rx_len;

	if (this->mode == LOCAL_CONTROL_MODE) {
		cmd = this->cmd_ptr;
		if (cmd->cmd_type == 0) {
			setControlMode(this, REMOTE_CONTROL_MODE);
		} else {
			this->cmd_ptr++;
		}
	} else {
    rx_len = receiveData(data, sizeof(data));
    if (rx_len > 0) {
      decodeCommand(DECODER, data, this->cmd_remote_buf);
      cmd = this->cmd_ptr;
    } else {
      cmd = (void *)0;
    }
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
