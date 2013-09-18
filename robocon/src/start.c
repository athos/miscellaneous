///////////////////////////////////////////////////////////
//  model_impl.c
///////////////////////////////////////////////////////////

#include "kernel.h"
#include "../osek/kernel_id.h"

#include "./robot/robot.h"

// �I�u�W�F�N�g��ÓI�Ɋm�ۂ���

#define BT_PASS_KEY     "teamKGK"

ROBOT robot;
ROBOT_STATUS robot_status;
// �f�o�C�X�������p�t�b�N�֐�
// ���̊֐���nxtOSEK�N�����Ɏ��s����܂��B
void ecrobot_device_initialize()
{
	// �Z���T�A���[�^�Ȃǂ̊e�f�o�C�X�̏������֐���
	// �����Ŏ������邱�Ƃ��ł��܂�
	// �ˁ@���Z���T�ԐFLED��ON�ɂ���
    startrobot( &robot );
    // BT�̏������֐�
//    ecrobot_init_bt_slave(BT_PASS_KEY);

}

// �f�o�C�X�I���p�t�b�N�֐�
// ���̊֐���STOP�܂���EXIT�{�^���������ꂽ���Ɏ��s����܂��B
void ecrobot_device_terminate()
{
	// �Z���T�A���[�^�Ȃǂ̊e�f�o�C�X�̏I���֐���
	// �����Ŏ������邱�Ƃ��ł��܂��B
	// �ˁ@���Z���T�ԐFLED��OFF�ɂ���
    stoprobot( &robot );

}
DeclareCounter(SysTimerCnt);

// 1msec�������荞��(ISR�J�e�S��2)����N������郆�[�U�[�p�t�b�N�֐�
void user_1ms_isr_type2(void)
{
    (void)SignalCounter(SysTimerCnt);

}

TASK(Task_Main)
{

    initrobot( &robot );
    

//    balance_init();    /* balance API�̏����� */

    TerminateTask();
}


