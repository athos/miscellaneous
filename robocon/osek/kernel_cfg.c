/*
 *  kernel_cfg.c
 *  Tue Sep 11 02:46:10 2012
 *  SG Version 2.00
 *  sg.exe ./osek/teamkgk.oil -os=ECC2 -I../../ecrobot/../toppers_osek/sg/impl_oil -template=../../ecrobot/../toppers_osek/sg/lego_nxt.sgt
 */
#include "osek_kernel.h"
#include "kernel_id.h"
#include "alarm.h"
#include "interrupt.h"
#include "resource.h"
#include "task.h"

#define __STK_UNIT VP
#define __TCOUNT_STK_UNIT(sz) (((sz) + sizeof(__STK_UNIT) - 1) / sizeof(__STK_UNIT))

#define TNUM_ALARM     7
#define TNUM_COUNTER   1
#define TNUM_ISR2      0
#define TNUM_RESOURCE  0
#define TNUM_TASK      8
#define TNUM_EXTTASK   0

const UINT8 tnum_alarm    = TNUM_ALARM;
const UINT8 tnum_counter  = TNUM_COUNTER;
const UINT8 tnum_isr2     = TNUM_ISR2;
const UINT8 tnum_resource = TNUM_RESOURCE;
const UINT8 tnum_task     = TNUM_TASK;
const UINT8 tnum_exttask  = TNUM_EXTTASK;

 /****** Object OS ******/

 /****** Object TASK ******/

const TaskType Task_Main = 0;
const TaskType Task_2ms = 1;
const TaskType Task_4ms = 2;
const TaskType Task_8ms = 3;
const TaskType Task_16ms = 4;
const TaskType Task_20ms = 5;
const TaskType Task_100ms = 6;
const TaskType Task_500ms = 7;

extern void TASKNAME( Task_Main )( void );
extern void TASKNAME( Task_2ms )( void );
extern void TASKNAME( Task_4ms )( void );
extern void TASKNAME( Task_8ms )( void );
extern void TASKNAME( Task_16ms )( void );
extern void TASKNAME( Task_20ms )( void );
extern void TASKNAME( Task_100ms )( void );
extern void TASKNAME( Task_500ms )( void );

static __STK_UNIT _stack_Task_2ms[__TCOUNT_STK_UNIT(512)];
static __STK_UNIT _stack_Task_4ms[__TCOUNT_STK_UNIT(512)];
static __STK_UNIT _stack_Task_8ms[__TCOUNT_STK_UNIT(512)];
static __STK_UNIT _stack_Task_16ms[__TCOUNT_STK_UNIT(512)];
static __STK_UNIT _stack_Task_20ms[__TCOUNT_STK_UNIT(512)];
static __STK_UNIT _stack_Task_100ms[__TCOUNT_STK_UNIT(512)];
static __STK_UNIT _stack_Task_500ms[__TCOUNT_STK_UNIT(512)];
static __STK_UNIT _stack_priority_10[__TCOUNT_STK_UNIT(512)];

const Priority tinib_inipri[TNUM_TASK] = { TPRI_MINTASK + 10, TPRI_MINTASK + 9, TPRI_MINTASK + 8, TPRI_MINTASK + 7, TPRI_MINTASK + 6, TPRI_MINTASK + 5, TPRI_MINTASK + 4, TPRI_MINTASK + 3, };
const Priority tinib_exepri[TNUM_TASK] = { TPRI_MAXTASK, TPRI_MINTASK + 9, TPRI_MINTASK + 8, TPRI_MINTASK + 7, TPRI_MINTASK + 6, TPRI_MINTASK + 5, TPRI_MINTASK + 4, TPRI_MINTASK + 3, };
const UINT8 tinib_maxact[TNUM_TASK] = { (1) - 1, (1) - 1, (1) - 1, (1) - 1, (1) - 1, (1) - 1, (1) - 1, (1) - 1, };
const AppModeType tinib_autoact[TNUM_TASK] = { 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, };
const FP tinib_task[TNUM_TASK] = { TASKNAME( Task_Main ), TASKNAME( Task_2ms ), TASKNAME( Task_4ms ), TASKNAME( Task_8ms ), TASKNAME( Task_16ms ), TASKNAME( Task_20ms ), TASKNAME( Task_100ms ), TASKNAME( Task_500ms ), };
const __STK_UNIT tinib_stk[TNUM_TASK] = { (__STK_UNIT)_stack_priority_10, (__STK_UNIT)_stack_Task_2ms, (__STK_UNIT)_stack_Task_4ms, (__STK_UNIT)_stack_Task_8ms, (__STK_UNIT)_stack_Task_16ms, (__STK_UNIT)_stack_Task_20ms, (__STK_UNIT)_stack_Task_100ms, (__STK_UNIT)_stack_Task_500ms, };
const UINT16 tinib_stksz[TNUM_TASK] = { 512, 512, 512, 512, 512, 512, 512, 512, };

TaskType tcb_next[TNUM_TASK];
UINT8 tcb_tstat[TNUM_TASK];
Priority tcb_curpri[TNUM_TASK];
UINT8 tcb_actcnt[TNUM_TASK];
EventMaskType tcb_curevt[TNUM_EXTTASK+1];
EventMaskType tcb_waievt[TNUM_EXTTASK+1];
ResourceType tcb_lastres[TNUM_TASK];
DEFINE_CTXB(TNUM_TASK);

 /****** Object COUNTER ******/

const CounterType SysTimerCnt = 0;

const TickType cntinib_maxval[TNUM_COUNTER] = { 10000, };
const TickType cntinib_maxval2[TNUM_COUNTER] = { 20001, };
const TickType cntinib_tickbase[TNUM_COUNTER] = { 1, };
const TickType cntinib_mincyc[TNUM_COUNTER] = { 1, };

AlarmType cntcb_almque[TNUM_COUNTER];
TickType cntcb_curval[TNUM_COUNTER];

 /****** Object ALARM ******/

const AlarmType Alarm_task_2ms = 0;
const AlarmType Alarm_task_4ms = 1;
const AlarmType Alarm_task_8ms = 2;
const AlarmType Alarm_task_16ms = 3;
const AlarmType Alarm_task_20ms = 4;
const AlarmType Alarm_task_100ms = 5;
const AlarmType Alarm_task_500ms = 6;

DeclareTask(Task_2ms);
static void _activate_alarm_Alarm_task_2ms( void );
static void _activate_alarm_Alarm_task_2ms( void )
{ (void)ActivateTask( Task_2ms ); }

DeclareTask(Task_4ms);
static void _activate_alarm_Alarm_task_4ms( void );
static void _activate_alarm_Alarm_task_4ms( void )
{ (void)ActivateTask( Task_4ms ); }

DeclareTask(Task_8ms);
static void _activate_alarm_Alarm_task_8ms( void );
static void _activate_alarm_Alarm_task_8ms( void )
{ (void)ActivateTask( Task_8ms ); }

DeclareTask(Task_16ms);
static void _activate_alarm_Alarm_task_16ms( void );
static void _activate_alarm_Alarm_task_16ms( void )
{ (void)ActivateTask( Task_16ms ); }

DeclareTask(Task_20ms);
static void _activate_alarm_Alarm_task_20ms( void );
static void _activate_alarm_Alarm_task_20ms( void )
{ (void)ActivateTask( Task_20ms ); }

DeclareTask(Task_100ms);
static void _activate_alarm_Alarm_task_100ms( void );
static void _activate_alarm_Alarm_task_100ms( void )
{ (void)ActivateTask( Task_100ms ); }

DeclareTask(Task_500ms);
static void _activate_alarm_Alarm_task_500ms( void );
static void _activate_alarm_Alarm_task_500ms( void )
{ (void)ActivateTask( Task_500ms ); }

const CounterType alminib_cntid[TNUM_ALARM] = { 0, 0, 0, 0, 0, 0, 0, };
const FP alminib_cback[TNUM_ALARM] = { _activate_alarm_Alarm_task_2ms, _activate_alarm_Alarm_task_4ms, _activate_alarm_Alarm_task_8ms, _activate_alarm_Alarm_task_16ms, _activate_alarm_Alarm_task_20ms, _activate_alarm_Alarm_task_100ms, _activate_alarm_Alarm_task_500ms, };
const AppModeType alminib_autosta[TNUM_ALARM] = { 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, };
const TickType alminib_almval[TNUM_ALARM] = { 1, 1, 1, 1, 1, 1, 1, };
const TickType alminib_cycle[TNUM_ALARM] = { 2, 4, 8, 16, 20, 100, 500, };

AlarmType almcb_next[TNUM_ALARM];
AlarmType almcb_prev[TNUM_ALARM];
TickType almcb_almval[TNUM_ALARM];
TickType almcb_cycle[TNUM_ALARM];

 /****** Object RESOURCE ******/


const Priority resinib_ceilpri[TNUM_RESOURCE+1] = { 0};

Priority rescb_prevpri[TNUM_RESOURCE+1];
ResourceType rescb_prevres[TNUM_RESOURCE+1];

 /****** Object EVENT ******/


 /****** Object ISR ******/


#define IPL_MAXISR2 0
const IPL ipl_maxisr2 = IPL_MAXISR2;


const Priority isrinib_intpri[TNUM_ISR2+1] = { 0};
ResourceType isrcb_lastres[TNUM_ISR2+1];

 /****** Object APPMODE ******/

void object_initialize( void )
{
	alarm_initialize();
	task_initialize();
}


/*
 *  TOPPERS/OSEK Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      OSEK Kernel
 * 
 *  Copyright (C) 2006 by Witz Corporation, JAPAN
 * 
 *  ��L���쌠�҂́C�ȉ��� (1)�`(4) �̏������CFree Software Foundation 
 *  �ɂ���Č��\����Ă��� GNU General Public License �� Version 2 �ɋL
 *  �q����Ă�������𖞂����ꍇ�Ɍ���C�{�\�t�g�E�F�A�i�{�\�t�g�E�F�A
 *  �����ς������̂��܂ށD�ȉ������j���g�p�E�����E���ρE�Ĕz�z�i�ȉ��C
 *  ���p�ƌĂԁj���邱�Ƃ𖳏��ŋ�������D
 *  (1) �{�\�t�g�E�F�A���\�[�X�R�[�h�̌`�ŗ��p����ꍇ�ɂ́C��L�̒���
 *      ���\���C���̗��p��������щ��L�̖��ۏ؋K�肪�C���̂܂܂̌`�Ń\�[
 *      �X�R�[�h���Ɋ܂܂�Ă��邱�ƁD
 *  (2) �{�\�t�g�E�F�A���C���C�u�����`���ȂǁC���̃\�t�g�E�F�A�J���Ɏg
 *      �p�ł���`�ōĔz�z����ꍇ�ɂ́C�Ĕz�z�ɔ����h�L�������g�i���p
 *      �҃}�j���A���Ȃǁj�ɁC��L�̒��쌠�\���C���̗��p��������щ��L
 *      �̖��ۏ؋K����f�ڂ��邱�ƁD
 *  (3) �{�\�t�g�E�F�A���C�@��ɑg�ݍ��ނȂǁC���̃\�t�g�E�F�A�J���Ɏg
 *      �p�ł��Ȃ��`�ōĔz�z����ꍇ�ɂ́C���̂����ꂩ�̏����𖞂�����
 *      �ƁD
 *    (a) �Ĕz�z�ɔ����h�L�������g�i���p�҃}�j���A���Ȃǁj�ɁC��L�̒�
 *        �쌠�\���C���̗��p��������щ��L�̖��ۏ؋K����f�ڂ��邱�ƁD
 *    (b) �Ĕz�z�̌`�Ԃ��C�ʂɒ�߂���@�ɂ���āCTOPPERS�v���W�F�N�g��
 *        �񍐂��邱�ƁD
 *  (4) �{�\�t�g�E�F�A�̗��p�ɂ�蒼�ړI�܂��͊ԐړI�ɐ����邢���Ȃ鑹
 *      �Q������C��L���쌠�҂����TOPPERS�v���W�F�N�g��Ɛӂ��邱�ƁD
 * 
 *  �{�\�t�g�E�F�A�́C���ۏ؂Œ񋟂���Ă�����̂ł���D��L���쌠�҂�
 *  ���TOPPERS�v���W�F�N�g�́C�{�\�t�g�E�F�A�Ɋւ��āC���̓K�p�\����
 *  �܂߂āC�����Ȃ�ۏ؂��s��Ȃ��D�܂��C�{�\�t�g�E�F�A�̗��p�ɂ�蒼
 *  �ړI�܂��͊ԐړI�ɐ����������Ȃ鑹�Q�Ɋւ��Ă��C���̐ӔC�𕉂�Ȃ��D
 * 
 */




