#include "FreeRTOS.h"
#include "common.h"
#include "task.h"
#include "../../../libs/uartp/inc/mf_uartp.h"

typedef struct
{
    mf_err_t type;
    char str[30];
}mf_err_str_t;

const mf_err_str_t mf_err_str_arr[] = 
{
    {MF_OK, "OK"},
    {MF_ERR_PARAM, "Param is invalid"},
    {MF_ERR_MEM, "Memory is overflow"},
    {MF_ERR_NORMAL, "Normal error"},
    {MF_ERR_REDEFINE, "Something had redefine"},
    {MF_ERR_UNDEFINE, "Somthing have not define"},
    {MF_ERR_UNKNOWN, "Unknown cause of error"},
    {MF_ERR_TODO, "TODO error"},
    {MF_ERR_INIT, "Init error"},
    {MF_ERR_MAX, "Can't find error string"},
};

const char *mf_err_str(mf_err_t type)
{
    int max_size = sizeof(mf_err_str_arr) / sizeof(mf_err_str_arr[0]);
    for (int i = 0; i < max_size - 1; i ++)
    {
        if (type == mf_err_str_arr[i].type)
        {
            return mf_err_str_arr[i].str;
        }
    }
    return mf_err_str_arr[max_size - 1].str;
}

static TaskHandle_t user_handle0 = NULL;
void user_task0(void *param);

static TaskHandle_t uartp_irq_handle = NULL;
void uartp_irq_task(void *param);

static void __attribute__((constructor)) _start_handler(void)
{
    LOGI("start handler!\n");
    BaseType_t res;
    mf_err_t err = MF_OK;
    err = mf_uartp_choose(UARTP_TYPE_BIN);
    if (MF_OK != err)   {LOGE("uartp choose failed! reason: %s\n", mf_err_str(err)); exit(1);}
    err = mf_uartp.init();
    if (MF_OK != err)   {LOGE("uartp choose failed! reason: %s\n", mf_err_str(err)); exit(1);}
    res = xTaskCreate(uartp_irq_task, "uartp irq task", configMINIMAL_STACK_SIZE, NULL, 4, &uartp_irq_handle);
    if (pdPASS != res)  {LOGE("Create uartp irq task failed! res: %ld\n", res); exit(1);}

    res = xTaskCreate(user_task0, "user task0", configMINIMAL_STACK_SIZE, (void *)1, 3, &user_handle0);
    if (pdPASS != res)  {LOGE("Create user task0 failed! res: %ld\n", res); exit(1);}
}

static void __attribute__((destructor)) _exit_handler(void)
{
    LOGI("exit handler!\n");
}

int main(void)
{
    LOGI("FreeRTOS Demo\n");

	vTaskStartScheduler();

    return 0;
}

void user_task0(void *param)
{
    static int cnt = 0;
    BaseType_t res;
    while (1)
    {
        printf("=>%d  cnt:%d\n", __LINE__, cnt ++);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void uartp_irq_task(void *param)
{
    uint8_t buff[128];
    int cnt = 0;
    int real_cnt = 0;
    while (1)
    {
        mf_uartp.loop();

        vTaskDelay(pdMS_TO_TICKS(50));
    }
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
