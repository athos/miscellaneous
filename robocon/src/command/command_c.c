#include "./command.h"
#include "./command_factory.h"
#include "./command_c.h"

static Condition cond_1 =   /* ベーシック走行終了条件 */
{
    0x12,                   /* DISTANCE , LARGE , SINGLE */
    {
		18850               /* 終了距離 */
	}
};


static Condition cond_2 =   /* ベーシック→パフォーマンスへの過渡 */
{
    0x82,                   /* MOTORSPEED, EQUAL , SINGLE */
    {
		0                   /* 回転変化は0 */
	}
};

static Condition cond_3 =   /* DUMMY */
{
    0x12,
    {
		20000
	}
};

static Condition cond_41 =   /* 難所カーブ終了条件１ */
{
    0x82,
    {
		0
	}
};
static Condition cond_42 =   /* 難所カーブ終了条件１ */
{
    0x12,
    {
		60
	}
};

static Condition cond_4;    /* 難所カーブ終了条件 */

Command cmd_local_buf[] =
{
    { 0x01 ,   /* ベーシック走行コマンド */
      95 ,     /* speed指示 */
      &cond_1
    },
    
    { 0x02 ,   /* 停止コマンド */
      0x00 ,   /* */
      &cond_2
    },

    { 0x03 ,   /* DUMMY*/
      0x00 ,   /* DUMMY*/
      &cond_3
    },
};


void command_init( void )   /* 宣言時初期化できないため、初期化関数を作成した */
{
    cond_4.cond_type = 0x00;
    cond_4.cond_content.cond_subconds.left = &cond_41;
    cond_4.cond_content.cond_subconds.right = &cond_42;

}
