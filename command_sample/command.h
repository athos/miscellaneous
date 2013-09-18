#ifndef COMMAND_H
#define COMMAND_H

#define AND_COND_TYPE 0
#define OR_COND_TYPE  1

typedef struct _Condition {
	int cond_type;
	union {
		int cond_value;
		struct {
			struct _Condition *left;
			struct _Condition *right;
		} cond_subconds;
	} cond_content;
} Condition;

#define COND_VALUE(cond) ((cond)->cond_content.cond_value)
#define LEFT_SUBCOND(cond) ((cond)->cond_content.cond_subconds.left)
#define RIGHT_SUBCOND(cond) ((cond)->cond_content.cond_subconds.right)

typedef struct _Command {
        int cmd_id;
	int cmd_type;
	int cmd_value;
	Condition *cmd_cond;
} Command;

void initCommand(Command *cmd, int id, int type, int value, Condition *cond);

int getId(Command *cmd);
int getType(Command *cmd);
int getValue(Command *cmd);
Condition *getCondition(Command *cmd);

void printCommand(Command *cmd);

#endif /* COMMAND_H */
