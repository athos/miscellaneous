#include "./command.h"
#include "./command_factory.h"
#include "./command_c.h"

static Condition cond_1 =   /* �x�[�V�b�N���s�I������ */
{
    0x12,                   /* DISTANCE , LARGE , SINGLE */
    {
		18850               /* �I������ */
	}
};


static Condition cond_2 =   /* �x�[�V�b�N���p�t�H�[�}���X�ւ̉ߓn */
{
    0x82,                   /* MOTORSPEED, EQUAL , SINGLE */
    {
		0                   /* ��]�ω���0 */
	}
};

static Condition cond_3 =   /* DUMMY */
{
    0x12,
    {
		20000
	}
};

static Condition cond_41 =   /* ��J�[�u�I�������P */
{
    0x82,
    {
		0
	}
};
static Condition cond_42 =   /* ��J�[�u�I�������P */
{
    0x12,
    {
		60
	}
};

static Condition cond_4;    /* ��J�[�u�I������ */

Command cmd_local_buf[] =
{
    { 0x01 ,   /* �x�[�V�b�N���s�R�}���h */
      95 ,     /* speed�w�� */
      &cond_1
    },
    
    { 0x02 ,   /* ��~�R�}���h */
      0x00 ,   /* */
      &cond_2
    },

    { 0x03 ,   /* DUMMY*/
      0x00 ,   /* DUMMY*/
      &cond_3
    },
};


void command_init( void )   /* �錾���������ł��Ȃ����߁A�������֐����쐬���� */
{
    cond_4.cond_type = 0x00;
    cond_4.cond_content.cond_subconds.left = &cond_41;
    cond_4.cond_content.cond_subconds.right = &cond_42;

}
