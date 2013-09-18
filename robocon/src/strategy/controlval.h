#ifndef CONTROLVAL_H
#define CONTROLVAL_H

typedef struct _motorvalue
{
    char tailmotor;
	char leftmotor;
	char rightmotor;

} motorval;

motorval getControlValue( void );

extern motorval motorvalue;

#endif
