#include "vivo_dual_cam.h"

/**
 * @brief Buffer status
*/
typedef enum
{
    VIVO_BUFFER_IDLE,
    VIVO_BUFFER_BUSY,
    VIVO_BUFFER_FULL
}vivo_buffer_status_t;

/**
 * @brief Private data
*/
typedef struct
{
    image_t image_buffer[2];
    uint32_t image_size;
    vivo_buffer_status_t  image_buffer_status[2];
    int buffer_index_in_use;    // -1,no index in use
}vivo_private_t;

static vivo_t vivo_dual_cam;

/**
 * @brief Init
 * 
 * @param   [in]    type    Camera image format
 * @param   [in]    w       Camera image width
 * @param   [in]    h       Camera image height
 * @return
*/
static vivo_err_t _vivo_dual_cam_init(vivo_format_t format, uint16_t w, uint16_t h)
{
    vivo_err_t err = VIVO_OK;
    vivo_t *vivo = (vivo_t *)&vivo_dual_cam;

    if (vivo->is_init) return VIVO_ERR_REINIT;
    /* Init lock */
    // ...

    /* Lock */
    if (vivo->lock)
        vivo->lock();

    /* Add private param */
    static vivo_private_t private;
    vivo->private = &private;
    private.buffer_index_in_use = -1;

    if (format == VIVO_FORMAT_YUV420)
    {
        private.image_size = w * h * 2;
        for (int i = 0; i < 2; i ++)
        {
            private.image_buffer[i].w = w;
            private.image_buffer[i].h = h;
            private.image_buffer[i].pixel = 2;
        }
    }
    else if (format == VIVO_FORMAT_JPEG)
    {
        private.image_size = w * h * 3;
        for (int i = 0; i < 2; i ++)            // There has some error setting, just for test
        {
            private.image_buffer[i].w = w;
            private.image_buffer[i].h = h;
            private.image_buffer[i].pixel = 3;
        }
    }
    else
    {
        err = VIVO_ERR_PARAM;
        goto _exit;
    }
    
    /* Malloc */
    private.image_buffer[0].addr = (uint8_t *)malloc(private.image_size);   
    if (!private.image_buffer[0].addr)  
    {
        err = VIVO_ERR_MEM;
        goto _exit;
    }

    private.image_buffer[1].addr = (uint8_t *)malloc(private.image_size);
    if (!private.image_buffer[1].addr)  
    {
        err = VIVO_ERR_MEM;
        free(private.image_buffer[0].addr);
        private.image_buffer[0].addr = NULL;
        goto _exit;
    }

    /* Camera device init */
    err = camera_init(w, h);
    if (err != VIVO_OK)   goto _exit;

    /* Reset vivo param */

    /* Init over */ 
    vivo->is_init = 1;

_exit:
    /* Unlock */
    if (vivo->unlock)
        vivo->unlock();

    return err;
}

/**
 * @brief Deinit
 * @return
*/
static vivo_err_t _vivo_dual_cam_deinit(void)
{
    vivo_err_t err = VIVO_OK;
    vivo_t *vivo = (vivo_t *)&vivo_dual_cam;
    vivo_private_t *private = (vivo_private_t *)vivo->private;
    if (!vivo->is_init) return VIVO_ERR_UNINIT;

    /* Lock */
    if (vivo->lock)
        vivo->lock();

    /* Deinit */
    if (private->image_buffer[0].addr)
    {
        free(private->image_buffer[0].addr);
        private->image_buffer[0].addr = NULL;
    }

    if (private->image_buffer[1].addr)
    {
        free(private->image_buffer[1].addr);
        private->image_buffer[1].addr = NULL;
    }

    err = camera_deinit();
    if (err != VIVO_OK)   goto _exit;

    /* Deinit over */ 
    vivo->is_init = 0;

_exit:
    /* Unlock */
    if (vivo->unlock)
        vivo->unlock();

    /* Deinit lock */
    // ...

    return err;
}

/**
 * @brief Lock
 * @return
*/
static vivo_err_t _vivo_dual_cam_lock(void)
{
    // Lock
    vivo_err_t err = VIVO_OK;

    return err;
}

/**
 * @brief Unlock
 * @return
*/
static vivo_err_t _vivo_dual_cam_unlock(void)
{
    // Unlock
    vivo_err_t err = VIVO_OK;

    return err;
}

/**
 * @brief Loop
 * @return
*/
static vivo_err_t _vivo_dual_cam_loop(void)
{
    vivo_err_t err = VIVO_OK;
    vivo_t *vivo = (vivo_t *)&vivo_dual_cam;
    vivo_private_t *private = (vivo_private_t *)vivo->private;
    if (!vivo->is_init) return VIVO_ERR_UNINIT;

    /* Lock */
    if (vivo->lock)
        vivo->lock();

    /**
     *  Snap camera data andler
     * 
     *  Total two buffer, and three status(IDLE,FULL,BUSY)
     * 
     *  ____________________
     * |  buffer0 | buffer1 |   
     * |__________|_________|          
     * |   IDLE   |  IDLE   |   0 is idle, 1 is idle, snap 0
     * |   FULL   |  IDLE   |   0 is full, 1 is idle, snap 1
     * |   BUSY   |  IDLE   |   0 is busy, 1 is idle, snap 1 
     * |   FULL   |  BUSY   |   0 is full, 1 is busy, snap 0
     * |   FULL   |  FULL   |   0 is full, 1 is full, snap 0 or 1(default 0)
     * |   IDLE   |  BUSY   |   0 is idle, 1 is busy, snap 0
     * |   IDLE   |  FULL   |   0 is idle, 1 is full, snap 0
     * |   BUSY   |  BUSY   |   0 is busy, 1 is busy, error!!
     * |   BUSY   |  FULL   |   0 is busy, 1 is full, snap 1
     * |__________|_________|
     * 
     * (Note:snap will also set the status to BUSY)
    */
    // Choose buffer index
    int snap_idx = -1;
    if (private->image_buffer_status[0] != VIVO_BUFFER_BUSY
        && private->image_buffer_status[1] != VIVO_BUFFER_IDLE)
    {
        snap_idx = 0;
    }
    else
    {
        snap_idx = 1;
    }

    if (private->image_buffer_status[0] ==  VIVO_BUFFER_BUSY 
        && private->image_buffer_status[1] == VIVO_BUFFER_BUSY)
    {
        err = VIVO_ERR_UNKNOWN;
        goto _exit;
    }

    // Snap
    vivo_buffer_status_t curr_sta = private->image_buffer_status[snap_idx];
    private->image_buffer_status[snap_idx] = VIVO_BUFFER_BUSY;
    err = camera_get_frame(private->image_buffer[snap_idx].addr, private->image_size);
    if (err == VIVO_OK)
    {
        private->image_buffer_status[snap_idx] = VIVO_BUFFER_FULL;
    }
    else
    {
        private->image_buffer_status[snap_idx] = curr_sta;
    }
    
_exit:
    /* Unlock */
    if (vivo->unlock)
        vivo->unlock();

    return err;
}

/**
 * @brief Control
 * @param [in]  command
 * @param [in]  argN
 * @return
*/
static vivo_err_t _vivo_dual_cam_control(int cmd, ...)
{
    vivo_err_t err = VIVO_OK;
    vivo_t *vivo = (vivo_t *)&vivo_dual_cam;
    if (!vivo->is_init) return VIVO_ERR_UNINIT;

    /* Lock */
    if (vivo->lock)
        vivo->lock();

    /* Control */
    va_list ap;
    va_start(ap, cmd);
    switch (cmd)
    {
    default:
        break;
    }
    va_end(ap);

    /* Unlock */
    if (vivo->unlock)
        vivo->unlock();

    return err;
}

/**
 * @brief Snap
 * @param [in]  type
 * @return
*/
static vivo_err_t _vivo_dual_cam_snap(int type, image_t *image)
{
    vivo_err_t err = VIVO_OK;
    vivo_t *vivo = (vivo_t *)&vivo_dual_cam;
    vivo_private_t *private = (vivo_private_t *)vivo->private;
    if (!vivo->is_init) return VIVO_ERR_UNINIT;

    /* Lock */
    if (vivo->lock)
        vivo->lock();

    /* Snap */
    int buffer_idx = -1;
    if (private->image_buffer_status[0] == VIVO_BUFFER_FULL)
    {
        buffer_idx = 0;
    }
    else if (private->image_buffer_status[1] == VIVO_BUFFER_FULL)
    {
        buffer_idx = 1;
    }
    else
    {
        private->buffer_index_in_use = -1;
        err = VIVO_ERR_TODO;
        goto _exit;
    }

    if (image == NULL || image->addr == NULL)
    {
        err = VIVO_ERR_PARAM;
        goto _exit;
    }

    if (private->image_buffer[buffer_idx].h * private->image_buffer[buffer_idx].w * private->image_buffer[buffer_idx].pixel
        < image->h * image->w * image->pixel)
    {
        err = VIVO_ERR_PARAM;
        goto _exit;
    }
    private->image_buffer_status[buffer_idx] = VIVO_BUFFER_BUSY;
    private->buffer_index_in_use = buffer_idx;
    memcpy(image->addr, private->image_buffer[buffer_idx].addr, image->h * image->w * image->pixel);
    private->image_buffer_status[buffer_idx] = VIVO_BUFFER_IDLE;
    
_exit:
    /* Unlock */
    if (vivo->unlock)
        vivo->unlock();

    return err;
}

static vivo_t vivo_dual_cam = 
{
    .is_init = 0,
    .private = NULL,
    .init = _vivo_dual_cam_init,
    .deinit = _vivo_dual_cam_deinit,
    .lock = _vivo_dual_cam_lock,
    .unlock = _vivo_dual_cam_unlock,
    .loop = _vivo_dual_cam_loop,
    .control = _vivo_dual_cam_control,
    .snap = _vivo_dual_cam_snap
};

/**
 * @brief Get vivo handle
 * @return Return a point of vivo handle
*/
vivo_t *get_vivo_dual_cam_handle(void)
{
    return &vivo_dual_cam;
}

vivo_err_t __attribute__((weak)) camera_init(uint16_t w, uint16_t h)
{
    vivo_err_t err = VIVO_OK;

    LOGW("Camera init is not define!\n");

    return err;
}

vivo_err_t __attribute__((weak)) camera_deinit(void)
{
    vivo_err_t err = VIVO_OK;

    LOGW("Camera deinit is not define!\n");

    return err;
}

vivo_err_t __attribute__((weak)) camera_get_frame(uint8_t *buffer, uint32_t buffer_max_size)
{
    vivo_err_t err = VIVO_OK;

    LOGW("Camera snap is not define!\n");

    return err;
}