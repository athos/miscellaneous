
#ifndef INTERNAL_H
#define INTERNAL_H

// INTERNAL CLASS
typedef struct _internal {
	unsigned short BATTERY;		//	�o�b�e���[���
	unsigned int TIMER;			//	�^�C�}�[���

} INTERNAL;

void resetInternalValue ( INTERNAL* this ) ;		//	�������̏�����

unsigned short getBatteryValue ( void );	//	�o�b�e���[���̎擾 [mV]
unsigned int getTimerValue ( void ) ;		//	�^�C�}�[���̎擾 [msec]
//unsigned int getIntervalValue ( void );		//	�O�Ƀ^�C�}�����擾���Ă���̎��� [msec]

#endif
