#include "FreeRTOS.h"
#include "common.h"
#include "task.h"
#include "mf_uartp.h"
#include "use_sdl2_show_v4l2.h"
#include "vivo.h"
#include "flow.h"
#include "aiao.h"
#include "db.h"

static TaskHandle_t uartp_process_handle = NULL;
static TaskHandle_t camera_process_handle = NULL;
void camera_process(void *param);
void uartp_process(void *param);
void handle_sigint(int signal);

static void __attribute__((constructor)) _start_handler(void)
{
    LOGI("start handler!\n");
    signal( SIGINT, handle_sigint );

    BaseType_t res;
    mf_err_t err = MF_OK;
    /* uartp init */
    err = mf_uartp_choose(UARTP_TYPE_BIN);
    if (MF_OK != err)   {LOGE("uartp choose failed! reason: %d\n", err); exit(1);}
    err = mf_uartp.init();
    if (MF_OK != err)   {LOGE("uartp init failed! reason: %d\n", err); exit(1);}
    res = xTaskCreate(uartp_process, "uartp irq task", configMINIMAL_STACK_SIZE, NULL, 4, &uartp_process_handle);
    if (pdPASS != res)  {LOGE("Create uartp irq task failed! res: %ld\n", res); exit(1);}
#if 1
    /* vivo init */
    err = vivo_choose(VIVO_TYPE_USB_CAM);
    if (MF_OK != err)   {LOGE("vivo choose failed!\n"); exit(1);}
    err = vivo.init(VIVO_FORMAT_JPEG, 320, 240);
    if (MF_OK != err)   {LOGE("vivo init failed!\n"); exit(1);}
    res = xTaskCreate(camera_process, "camera snap task", configMINIMAL_STACK_SIZE, NULL, 5, &camera_process_handle);
    if (pdPASS != res)  {LOGE("Create camera snap task failed! res: %ld\n", res); exit(1);}
#endif
    /* aiao init */
    err = aiao_choose(AIAO_TYPE_NORMAL);
    if (MF_OK != err)   {LOGE("aiao choose failed!\n"); exit(1);}
    err = aiao.init();
    if (MF_OK != err)   {LOGE("vivo init failed!\n"); exit(1);}
}

static void __attribute__((destructor)) _exit_handler(void)
{
    LOGI("exit handler!\n");
    if (mf_uartp.deinit)
        mf_uartp.deinit();
    if (vivo.deinit)
        vivo.deinit();
    if (aiao.deinit)
        aiao.deinit();
}

int main(int argc, char const *argv[])
{
	vTaskStartScheduler();
    return 0;
}

void uartp_process(void *param)
{
    while (1)
    {
        mf_uartp.loop();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void camera_process(void *param)
{
    image_t snap_img;
    snap_img.h = 240;
    snap_img.w = 320;
    snap_img.pixel = 3;
    snap_img.addr = (uint8_t *)malloc(snap_img.w * snap_img.h * snap_img.pixel);
    if (!snap_img.addr)
    {
        LOGE("MEMORY OVERFLOW!\n");
        exit(-1);
    }

    vivo_err_t err = VIVO_OK;
    while (1)
    {
        portENTER_CRITICAL();
        err = vivo.loop();
        if (err != VIVO_OK)   continue;
        portEXIT_CRITICAL();

        err = vivo.snap(0, &snap_img);
        if (err == VIVO_OK)
        {
            LOGI("Snap OK!\n"); 
            
            user_sdl_t *sdl = (user_sdl_t *)get_sdl_handle();
            SDL_RWops *dst = SDL_RWFromMem(snap_img.addr, 154189);
            if (!dst)
            {
                LOGE("SDL_RWFromFile failed\n");
            }
            SDL_Surface *sur = IMG_LoadJPG_RW(dst);
            if (!sur)
            {
                LOGE("IMG_LoadJPG_RW failed\n");
            }
            SDL_Texture *tet = SDL_CreateTextureFromSurface(sdl->ren, sur);
            SDL_FreeRW(dst);
            SDL_FreeSurface(sur);

            SDL_RenderClear(sdl->ren);
            SDL_RenderCopy(sdl->ren, tet, NULL, NULL);
            SDL_RenderPresent(sdl->ren);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelete(NULL);
}

void handle_sigint(int signal)
{
    LOGI("Get exit signal!\n");
    exit(0);
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
