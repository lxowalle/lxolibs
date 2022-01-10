#include "vi_dual_cam.h"

/**
 * @brief Buffer status
*/
typedef enum
{
    VI_BUFFER_IDLE,
    VI_BUFFER_BUSY,
    VI_BUFFER_FULL
}vi_buffer_status_t;

/**
 * @brief Private data
*/
typedef struct
{
    image_t image_buffer[2];
    uint32_t image_size;
    vi_buffer_status_t  image_buffer_status[2];
    int buffer_index_in_use;    // -1,no index in use
}vi_private_t;

static vi_t vi_dual_cam;

/**
 * @brief Init
 * 
 * @param   [in]    type    Camera image format
 * @param   [in]    w       Camera image width
 * @param   [in]    h       Camera image height
 * @return
*/
static vi_err_t _vi_dual_cam_init(vi_format_t format, uint16_t w, uint16_t h)
{
    vi_err_t err = VI_OK;
    vi_t *vi = (vi_t *)&vi_dual_cam;

    if (vi->is_init) return VI_ERR_REINIT;
    /* Init lock */
    // ...

    /* Lock */
    if (vi->lock)
        vi->lock();

    /* Add private param */
    static vi_private_t private;
    vi->private = &private;
    private.buffer_index_in_use = -1;

    if (format == VI_FORMAT_YUV420)
    {
        private.image_size = w * h * 2;
        for (int i = 0; i < 2; i ++)
        {
            private.image_buffer[i].w = w;
            private.image_buffer[i].w = h;
            private.image_buffer[i].pixel = 2;
        }
    }
    else
    {
        err = VI_ERR_PARAM;
        goto _exit;
    }
    
    /* Malloc */
    private.image_buffer[0].addr = (uint8_t *)malloc(private.image_size);   
    if (!private.image_buffer[0].addr)  
    {
        err = VI_ERR_MEM;
        goto _exit;
    }

    private.image_buffer[1].addr = (uint8_t *)malloc(private.image_size);
    if (!private.image_buffer[1].addr)  
    {
        err = VI_ERR_MEM;
        free(private.image_buffer[0].addr);
        private.image_buffer[0].addr = NULL;
        goto _exit;
    }

    /* Camera device init */
    err = camera_init(w, h);
    if (err != VI_OK)   goto _exit;

    /* Reset vi param */

    /* Init over */ 
    vi->is_init = 1;

_exit:
    /* Unlock */
    if (vi->unlock)
        vi->unlock();

    return err;
}

/**
 * @brief Deinit
 * @return
*/
static vi_err_t _vi_dual_cam_deinit(void)
{
    vi_err_t err = VI_OK;
    vi_t *vi = (vi_t *)&vi_dual_cam;
    vi_private_t *private = (vi_private_t *)vi->private;
    if (!vi->is_init) return VI_ERR_UNINIT;

    /* Lock */
    if (vi->lock)
        vi->lock();

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
    if (err != VI_OK)   goto _exit;

    /* Deinit over */ 
    vi->is_init = 0;

_exit:
    /* Unlock */
    if (vi->unlock)
        vi->unlock();

    /* Deinit lock */
    // ...

    return err;
}

/**
 * @brief Lock
 * @return
*/
static vi_err_t _vi_dual_cam_lock(void)
{
    // Lock
    vi_err_t err = VI_OK;

    return err;
}

/**
 * @brief Unlock
 * @return
*/
static vi_err_t _vi_dual_cam_unlock(void)
{
    // Unlock
    vi_err_t err = VI_OK;

    return err;
}

/**
 * @brief Loop
 * @return
*/
static vi_err_t _vi_dual_cam_loop(void)
{
    vi_err_t err = VI_OK;
    vi_t *vi = (vi_t *)&vi_dual_cam;
    vi_private_t *private = (vi_private_t *)vi->private;
    if (!vi->is_init) return VI_ERR_UNINIT;

    /* Lock */
    if (vi->lock)
        vi->lock();

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
    if (private->image_buffer_status[0] != VI_BUFFER_BUSY
        && private->image_buffer_status[1] != VI_BUFFER_IDLE)
    {
        snap_idx = 0;
    }
    else
    {
        snap_idx = 1;
    }

    if (private->image_buffer_status[0] ==  VI_BUFFER_BUSY 
        && private->image_buffer_status[1] == VI_BUFFER_BUSY)
    {
        err = VI_ERR_UNKNOWN;
        goto _exit;
    }

    // Snap
    vi_buffer_status_t curr_sta = private->image_buffer_status[snap_idx];
    private->image_buffer_status[snap_idx] = VI_BUFFER_BUSY;
    err = camera_snap(private->image_buffer[snap_idx].addr, private->image_size);
    if (err == VI_OK)
    {
        private->image_buffer_status[snap_idx] = VI_BUFFER_FULL;
    }
    else
    {
        private->image_buffer_status[snap_idx] = curr_sta;
    }

_exit:
    /* Unlock */
    if (vi->unlock)
        vi->unlock();

    return err;
}

/**
 * @brief Control
 * @param [in]  command
 * @param [in]  argN
 * @return
*/
static vi_err_t _vi_dual_cam_control(int cmd, ...)
{
    vi_err_t err = VI_OK;
    vi_t *vi = (vi_t *)&vi_dual_cam;
    if (!vi->is_init) return VI_ERR_UNINIT;

    /* Lock */
    if (vi->lock)
        vi->lock();

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
    if (vi->unlock)
        vi->unlock();

    return err;
}

/**
 * @brief Snap
 * @param [in]  type
 * @return
*/
static vi_err_t _vi_dual_cam_snap(int type, image_t *image)
{
    vi_err_t err = VI_OK;
    vi_t *vi = (vi_t *)&vi_dual_cam;
    vi_private_t *private = (vi_private_t *)vi->private;
    if (!vi->is_init) return VI_ERR_UNINIT;

    /* Lock */
    if (vi->lock)
        vi->lock();

    /* Snap */
    int buffer_idx = -1;
    if (private->image_buffer_status[0] == VI_BUFFER_FULL)
    {
        buffer_idx = 0;
    }
    else if (private->image_buffer_status[1] == VI_BUFFER_FULL)
    {
        buffer_idx = 1;
    }
    else
    {
        private->buffer_index_in_use = -1;
        err = VI_ERR_TODO;
        goto _exit;
    }

    if (image == NULL || image->addr == NULL)
    {
        err = VI_ERR_PARAM;
        goto _exit;
    }
    private->image_buffer_status[buffer_idx] = VI_BUFFER_BUSY;
    private->buffer_index_in_use = buffer_idx;
    memcpy(image->addr, &private->image_buffer[buffer_idx], image->h * image->w * image->pixel);

_exit:
    /* Unlock */
    if (vi->unlock)
        vi->unlock();

    return err;
}

static vi_t vi_dual_cam = 
{
    .is_init = 0,
    .private = NULL,
    .init = _vi_dual_cam_init,
    .deinit = _vi_dual_cam_deinit,
    .lock = _vi_dual_cam_lock,
    .unlock = _vi_dual_cam_unlock,
    .loop = _vi_dual_cam_loop,
    .control = _vi_dual_cam_control,
    .snap = _vi_dual_cam_snap
};

/**
 * @brief Get vi handle
 * @return Return a point of vi handle
*/
vi_t *get_vi_dual_cam_handle(void)
{
    return &vi_dual_cam;
}

vi_err_t __attribute__((weak)) camera_init(uint16_t w, uint16_t h)
{
    vi_err_t err = VI_OK;

    LOGW("Camera init is not define!\n");

    return err;
}

vi_err_t __attribute__((weak)) camera_deinit(void)
{
    vi_err_t err = VI_OK;

    LOGW("Camera deinit is not define!\n");

    return err;
}

vi_err_t __attribute__((weak)) camera_snap(uint8_t *data, uint32_t size)
{
    vi_err_t err = VI_OK;

    LOGW("Camera snap is not define!\n");

    return err;
}