#include "mo.h"


/**
 * @brief Private data
*/
typedef struct
{

}mo_private_t;

static mo_t mo_normal;

/**
 * @brief Init
 * @return
*/
static mo_err_t _mo_normal_init(void)
{
    mo_err_t err = MO_OK;
    mo_t *mo = (mo_t *)&mo_normal;

    if (mo->is_init) return MO_ERR_REINIT;
    /* Init lock */
    // ...

    /* Lock */
    if (mo->lock)
        mo->lock();

    /* Add private param */
    static mo_private_t private;
    mo->private = &private;
    
    /* Reset uartp param */
    mo->status = MO_STA_IDEL;

    /* Init over */ 
    mo->is_init = 1;

    /* Unlock */
    if (mo->unlock)
        mo->unlock();

    return err;
}

/**
 * @brief Deinit
 * @return
*/
static mo_err_t _mo_normal_deinit(void)
{
    mo_err_t err = MO_OK;
    mo_t *mo = (mo_t *)&mo_normal;
    if (!mo->is_init) return MO_ERR_UNINIT;

    /* Lock */
    if (mo->lock)
        mo->lock();

    /* Deinit */

    /* Deinit over */ 
    mo->is_init = 0;

    /* Unlock */
    if (mo->unlock)
        mo->unlock();

    /* Deinit lock */
    // ...

    return err;
}

/**
 * @brief Lock
 * @return
*/
static mo_err_t _mo_normal_lock(void)
{
    // Lock
    mo_err_t err = MO_OK;

    return err;
}

/**
 * @brief Unlock
 * @return
*/
static mo_err_t _mo_normal_unlock(void)
{
    // Unlock
    mo_err_t err = MO_OK;

    return err;
}

/**
 * @brief Loop
 * @return
*/
static mo_err_t _mo_normal_loop(void)
{
    mo_err_t err = MO_OK;
    mo_t *mo = (mo_t *)&mo_normal;
    if (!mo->is_init) return MO_ERR_UNINIT;

    /* Lock */
    if (mo->lock)
        mo->lock();

    /* Handler */

    /* Unlock */
    if (mo->unlock)
        mo->unlock();

    return err;
}

/**
 * @brief Control
 * @param [in]  command
 * @param [in]  argN
 * @return
*/
static mo_err_t _mo_normal_control(int cmd, ...)
{
    mo_err_t err = MO_OK;
    mo_t *mo = (mo_t *)&mo_normal;
    if (!mo->is_init) return MO_ERR_UNINIT;

    /* Lock */
    if (mo->lock)
        mo->lock();

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
    if (mo->unlock)
        mo->unlock();

    return err;
}

/**
 * @brief Playback
 * @return
*/
static mo_err_t _mo_normal_playback(int type)
{
    mo_err_t err = MO_OK;
    mo_t *mo = (mo_t *)&mo_normal;
    if (!mo->is_init) return MO_ERR_UNINIT;

    /* Lock */
    if (mo->lock)
        mo->lock();

    /* Handler */

    /* Unlock */
    if (mo->unlock)
        mo->unlock();

    return err;
}

static mo_t mo_normal = 
{
    .is_init = 0,
    .status = MO_STA_IDEL,
    .private = NULL,
    .init = _mo_normal_init,
    .deinit = _mo_normal_deinit,
    .lock = _mo_normal_lock,
    .unlock = _mo_normal_unlock,
    .loop = _mo_normal_loop,
    .control = _mo_normal_control,
    .playback = _mo_normal_playback
};

/**
 * @brief Get mo handle
 * @return Return a point of mo handle
*/
mo_t *get_mo_normal_handle(void)
{
    return &mo_normal;
}


