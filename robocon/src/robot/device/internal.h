
#ifndef INTERNAL_H
#define INTERNAL_H

// INTERNAL CLASS
typedef struct _internal {
	unsigned short BATTERY;		//	バッテリー情報
	unsigned int TIMER;			//	タイマー情報

} INTERNAL;

void resetInternalValue ( INTERNAL* this ) ;		//	内部情報の初期化

unsigned short getBatteryValue ( void );	//	バッテリー情報の取得 [mV]
unsigned int getTimerValue ( void ) ;		//	タイマー情報の取得 [msec]
//unsigned int getIntervalValue ( void );		//	前にタイマ情報を取得してからの時間 [msec]

#endif
