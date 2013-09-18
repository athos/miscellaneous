/* btecho.c for TOPPERS/ATK(OSEK) */
#include <stdio.h>

#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include "command.h"
#include "encoder.h"
#include "decoder.h"
#include "command_factory.h"
#include "bluetooth.h"

#define PASS_KEY    "1234"  /* Bluetooth pass key */

#define MAX_NUM_OF_CHAR (16)
#define MAX_NUM_OF_LINE (8)

static void echo_lcd(U8 *data, int len);

CHAR buf[BT_MAX_RX_BUF_SIZE];

void ecrobot_device_initialize()
{
	ecrobot_init_bt_slave(PASS_KEY);
}

void ecrobot_device_terminate()
{
	ecrobot_term_bt_connection();
}

/* nxtOSEK hook to be invoked from an ISR in category 2 */
void user_1ms_isr_type2(void){ /* do nothing */ }

TASK(OSEK_Task_Background)
{
  Command *cmd;
  byte_t data[64];

  setControlMode(COMMAND_FACTORY, REMOTE_CONTROL_MODE);
        
	while(1)
	{
   		/* Echo back message from a PC terminal software.
   		 * E.g. Tera Term (http://ttssh2.sourceforge.jp/index.html.en)
   		 */
    cmd = getNextCommand(COMMAND_FACTORY);
		if (cmd)
		{
      display_clear(0);
      display_goto_xy(0, 0);
      display_string("cmd_id: ");
      display_int(cmd->cmd_id, 2);
      display_update();
      systick_wait_ms(500);
      encodeCommand(ENCODER, cmd, data, sizeof(data));
      sendData(data);
      systick_wait_ms(500);
		} else {
      systick_wait_ms(50);
    }
	}
}

static void echo_lcd(U8 *data, int len)
{
	static int pos_x = 0;
	static int pos_y = 0;
	int i;

	/* set LCD postion in x, y */
	if (pos_x >= MAX_NUM_OF_CHAR)
	{
		pos_x = 0;
		pos_y++;
	}

	if (pos_y >= MAX_NUM_OF_LINE)
	{
		pos_x = 0;
		pos_y = 0;
	}

	if (pos_x == 0 && pos_y == 0)
	{
		display_clear(0);
	}
	display_goto_xy(pos_x, pos_y);

	for (i = 0; i < len; i++)
	{
		if (data[i] == '\n')
		{
			pos_x = 0;
			pos_y++;
			break;
		}
		else
		{
			display_string((char *)&data[i]);
			display_update();
			if (i == (len - 1))
			{
				pos_x += len;
				break;
			}
		}
	}
}
