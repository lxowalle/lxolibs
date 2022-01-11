#include "FreeRTOS.h"
#include "common.h"
#include "task.h"
#include "mf_uartp.h"
#include "use_sdl2_show_v4l2.h"
#include "vi.h"
#include "flow.h"

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
    {MF_ERR_REINIT, "Something had redefine"},
    {MF_ERR_UNINIT, "Somthing have not define"},
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
static TaskHandle_t uartp_irq_handle = NULL;
static TaskHandle_t video_handle = NULL;

void user_task0(void *param);
void uartp_irq_task(void *param);
void vi_task(void *param);

void handle_sigint( int signal )
{
    LOGI("Get exit signal!\n");
    exit(0);
}
static void __attribute__((constructor)) _start_handler(void)
{
    LOGI("start handler!\n");

    signal( SIGINT, handle_sigint );

    BaseType_t res;
    mf_err_t err = MF_OK;
    err = mf_uartp_choose(UARTP_TYPE_BIN);
    if (MF_OK != err)   {LOGE("uartp choose failed! reason: %s\n", mf_err_str(err)); exit(1);}
    err = mf_uartp.init();
    if (MF_OK != err)   {LOGE("uartp init failed! reason: %s\n", mf_err_str(err)); exit(1);}
    res = xTaskCreate(uartp_irq_task, "uartp irq task", configMINIMAL_STACK_SIZE, NULL, 4, &uartp_irq_handle);
    if (pdPASS != res)  {LOGE("Create uartp irq task failed! res: %ld\n", res); exit(1);}

    res = xTaskCreate(user_task0, "user task0", configMINIMAL_STACK_SIZE, (void *)1, 3, &user_handle0);
    if (pdPASS != res)  {LOGE("Create user task0 failed! res: %ld\n", res); exit(1);}
#if 1
    err = vi_choose(VI_TYPE_USB_CAM);
    if (MF_OK != err)   {LOGE("vi choose failed!\n"); exit(1);}
    err = vi.init(VI_FORMAT_JPEG, 320, 240);
    if (MF_OK != err)   {LOGE("vi init failed!\n"); exit(1);}

    res = xTaskCreate(vi_task, "vi task", configMINIMAL_STACK_SIZE, NULL, 5, &video_handle);
    if (pdPASS != res)  {LOGE("Create vi task failed! res: %ld\n", res); exit(1);}
#endif
}

static void __attribute__((constructor)) _show_video(void)
{
#if 0
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

    vi_err_t err = VI_OK;
    while(1)
    {
        err = vi.loop();
        if (err != VI_OK)   continue;

        err = vi.snap(0, &snap_img);
        if (err == VI_OK)
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
    }
#endif
}

static void __attribute__((destructor)) _exit_handler(void)
{
    LOGI("exit handler!\n");
    if (mf_uartp.deinit)
        mf_uartp.deinit();
    if (vi.deinit)
        vi.deinit();
}

int main(int argc, char const *argv[])
{
    LOGI("FreeRTOS start scheduler\n");

	vTaskStartScheduler();

    return 0;
}

void user_task0(void *param)
{
    static int cnt = 0;
    BaseType_t res;
    while (1)
    {
        LOGI("cnt:%d\n", cnt ++);

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


void vi_task(void *param)
{
#if 1
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

    vi_err_t err = VI_OK;
    while(1)
    {
        portENTER_CRITICAL();
        err = vi.loop();
        if (err != VI_OK)   continue;
        portEXIT_CRITICAL();

        err = vi.snap(0, &snap_img);
        if (err == VI_OK)
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
#endif
    vTaskDelete(NULL);
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
