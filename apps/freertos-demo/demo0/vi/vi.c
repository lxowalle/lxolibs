#include "vi.h"

/**
 * @brief Private data
*/
typedef struct
{
    image_t image_buffer[2];
    uint8_t buff_lock0 : 1;
    uint8_t buff_lock1 : 1;
}vi_private_t;

static vi_t vi_cam;

/**
 * @brief Init
 * @return
*/
static vi_err_t _vi_cam_init(void)
{
    vi_err_t err = VI_OK;
    vi_t *vi = (vi_t *)&vi_cam;

    if (vi->is_init) return VI_ERR_REINIT;
    /* Init lock */
    // ...

    /* Lock */
    if (vi->lock)
        vi->lock();

    /* Add private param */
    static vi_private_t private;
    vi->private = &private;
    
    /* Reset uartp param */
    vi->status = VI_STA_IDEL;

    /* Init over */ 
    vi->is_init = 1;

    /* Unlock */
    if (vi->unlock)
        vi->unlock();

    return err;
}

/**
 * @brief Deinit
 * @return
*/
static vi_err_t _vi_cam_deinit(void)
{
    vi_err_t err = VI_OK;
    vi_t *vi = (vi_t *)&vi_cam;
    if (!vi->is_init) return VI_ERR_UNINIT;

    /* Lock */
    if (vi->lock)
        vi->lock();

    /* Deinit */

    /* Deinit over */ 
    vi->is_init = 0;

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
static vi_err_t _vi_cam_lock(void)
{
    // Lock
    vi_err_t err = VI_OK;

    return err;
}

/**
 * @brief Unlock
 * @return
*/
static vi_err_t _vi_cam_unlock(void)
{
    // Unlock
    vi_err_t err = VI_OK;

    return err;
}

/**
 * @brief Loop
 * @return
*/
static vi_err_t _vi_cam_loop(void)
{
    vi_err_t err = VI_OK;
    vi_t *vi = (vi_t *)&vi_cam;
    if (!vi->is_init) return VI_ERR_UNINIT;

    /* Lock */
    if (vi->lock)
        vi->lock();

    /* Handler */

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
static vi_err_t _vi_cam_control(int cmd, ...)
{
    vi_err_t err = VI_OK;
    vi_t *vi = (vi_t *)&vi_cam;
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
static vi_err_t _vi_cam_snap(int type, image_t **image)
{
    vi_err_t err = VI_OK;
    vi_t *vi = (vi_t *)&vi_cam;
    vi_private_t *private = (vi_private_t *)vi->private;
    if (!vi->is_init) return VI_ERR_UNINIT;

    /* Lock */
    if (vi->lock)
        vi->lock();

    /* Snap */
    if (private->buff_lock0)
    {
        *image = &private->image_buffer[0];
    }
    else if (private->buff_lock1)
    {
        *image = &private->image_buffer[1];
    }
    else
    {
        *image = NULL;
        LOGW("Can't cam snap image, because cam buffer is locked!\n");
    }

    /* Unlock */
    if (vi->unlock)
        vi->unlock();

    return err;
}

static vi_t vi_cam = 
{
    .is_init = 0,
    .status = VI_STA_IDEL,
    .private = NULL,
    .init = _vi_cam_init,
    .deinit = _vi_cam_deinit,
    .lock = _vi_cam_lock,
    .unlock = _vi_cam_unlock,
    .loop = _vi_cam_loop,
    .control = _vi_cam_control,
    .snap = _vi_cam_snap
};

/**
 * @brief Get vi handle
 * @return Return a point of vi handle
*/
vi_t *get_vi_cam_handle(void)
{
    return &vi_cam;
}
