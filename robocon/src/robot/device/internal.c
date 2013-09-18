#include "ecrobot_interface.h"
#include "./internal.h"

// INTERNAL CLASS
void resetInternalValue ( INTERNAL* this ) {
	this->BATTERY = getBatteryValue();
	this->TIMER = getTimerValue();
}

unsigned short getBatteryValue ( void ) {
	return ecrobot_get_battery_voltage();
}

unsigned int getTimerValue ( void ) {

	return systick_get_ms();
}

#if 0
unsigned int getIntervalValue ( void ) {
	_TIMER = TIMER;
	return getIntervalValue () - _TIMER;
}
#endif
