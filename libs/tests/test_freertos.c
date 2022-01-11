#include "FreeRTOS.h"
#include "task.h"
#include "common.h"
#include <stdlib.h>
#include <signal.h>

static void signal_handle(int signal)
{
    exit(0);
}

static void __attribute__((constructor(101))) start_handler(void)
{
    signal(SIGINT, signal_handle);
    LOGI("Start!\n");
}

static void __attribute__((destructor)) exit_handler(void)
{
    LOGI("Exit!\n");
}

/** 测试任务通知 */
#if 1
TaskHandle_t task_notify_handle1;
TaskHandle_t task_notify_handle2;

void task_notify1(void *param)
{
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    while (1)
    {
        LOGI("task notify1!\n");
        // xTaskNotify(task_notify_handle2, 0, eNoAction);
        xTaskNotifyGive(task_notify_handle2);
        // xTaskNotifyGiveIndexed(task_notify_handle2, 0);
        // uint32_t value = 0x00;
        // xTaskNotifyAndQuery(task_notify_handle2, 2, eSetValueWithOverwrite, &value);
        // LOGI("value:%#x\n", value);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void task_notify2(void *param)
{
    while (1)
    {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        // ulTaskNotifyTakeIndexed(0, pdTRUE, portMAX_DELAY);
        // xTaskNotifyWait(0, 0, 0, portMAX_DELAY);
        
        LOGI("task notify2!\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void __attribute__((constructor)) task_notify_test(void)
{
    BaseType_t res;
    res = xTaskCreate(task_notify1, "task notify", configMINIMAL_STACK_SIZE, NULL, 5, &task_notify_handle1);
    if (res != pdTRUE)  {LOGE("Create task failed!\n"); exit(0);}
    res = xTaskCreate(task_notify2, "task notify", configMINIMAL_STACK_SIZE, NULL, 6, &task_notify_handle2);
    if (res != pdTRUE)  {LOGE("Create task failed!\n"); exit(0);}
}
#endif



int main(int argc, char *argv[])
{
    LOGI("Task start schedule!\n");
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
