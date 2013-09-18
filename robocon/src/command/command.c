#include "command.h"

void initCommand(Command *this, int id, int type, int value, Condition *cond) {
        this->cmd_id = id;
	this->cmd_type = type;
	this->cmd_value = value;
	this->cmd_cond = cond;
}

int getId(Command *this) {
        return this->cmd_id;
}

int getType(Command *this) {
	return this->cmd_type;
}

int getValue(Command *this) {
	return this->cmd_value;
}

Condition *getCondition(Command *this) {
	return this->cmd_cond;
}
