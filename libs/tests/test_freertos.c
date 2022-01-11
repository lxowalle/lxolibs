#include "FreeRTOS.h"
#include "task.h"
#include "common.h"
#include <stdlib.h>


int main(int argc, char *argv[])
{

    vTaskStartScheduler();

    return 0;
}

void __attribute__((weak)) vAssertCalled( const char * const pcFileName,
                    unsigned long ulLine )
{
    ( void ) ulLine;
    ( void ) pcFileName;

    taskENTER_CRITICAL();
    LOGE("FreeRTOS Assert:file:%s line:%ld\n", pcFileName, ulLine);
    taskEXIT_CRITICAL();

    exit(-1);
}

void __attribute__((weak)) vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void __attribute__((weak)) vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void __attribute__((weak)) vApplicationMallocFailedHook( void )
{
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
