///////////////////////////////////////////////////////////
//  model_impl.c
///////////////////////////////////////////////////////////

#include "kernel.h"
#include "../osek/kernel_id.h"
#include "./robot/robot.h"
#include "./strategy/dispatch.h"
#include "./control/control.h"


TASK( Task_2ms )
{
    dispatch_main();
    control();
    TerminateTask();
}

TASK( Task_4ms )
{

    monitor( &(data_log_buffer[0]) );

    TerminateTask();
}

TASK( Task_8ms )
{

    TerminateTask();
}

TASK( Task_16ms )
{

    TerminateTask();
}

TASK( Task_20ms )
{

    TerminateTask();
}


TASK( Task_100ms )
{

    TerminateTask();
}


TASK( Task_500ms )
{

    TerminateTask();
}
