#include "aiao_normal.h"

typedef struct
{
    char wav[15];
    uint32_t address;
    uint32_t wav_len;
}wav_item_t;

typedef struct
{
    wav_item_t wav_list[10];
}aiao_private_t;

static aiao_t aiao_normal;

/**
 * @brief Init
 * 
 * @return
*/
static aiao_err_t _aiao_normal_init(void)
{
    aiao_err_t err = AIAO_OK;
    aiao_t *aiao = (aiao_t *)&aiao_normal;

    if (aiao->is_init) return AIAO_ERR_REINIT;
    /* Init lock */
    // ...

    /* Lock */
    if (aiao->lock)
        aiao->lock();

    /* Add private param */
    static aiao_private_t private;
    aiao->private = &private;

    /* Audio  init */

    /* Reset aiao param */

    /* Init over */ 
    aiao->is_init = 1;

_exit:
    /* Unlock */
    if (aiao->unlock)
        aiao->unlock();

    return err;
}

/**
 * @brief Deinit
 * @return
*/
static aiao_err_t _aiao_normal_deinit(void)
{
    aiao_err_t err = AIAO_OK;
    aiao_t *aiao = (aiao_t *)&aiao_normal;
    if (!aiao->is_init) return AIAO_ERR_UNINIT;

    /* Lock */
    if (aiao->lock)
        aiao->lock();

    /* Deinit */

_exit:
    /* Unlock */
    if (aiao->unlock)
        aiao->unlock();

    /* Deinit lock */
    // ...

    return err;
}

/**
 * @brief Lock
 * @return
*/
static aiao_err_t _aiao_normal_lock(void)
{
    // Lock
    aiao_err_t err = AIAO_OK;

    return err;
}

/**
 * @brief Unlock
 * @return
*/
static aiao_err_t _aiao_normal_unlock(void)
{
    // Unlock
    aiao_err_t err = AIAO_OK;

    return err;
}

/**
 * @brief Loop
 * @return
*/
static aiao_err_t _aiao_normal_loop(void)
{
    aiao_err_t err = AIAO_OK;
    aiao_t *aiao = (aiao_t *)&aiao_normal;
    aiao_private_t *private = (aiao_private_t *)aiao->private;
    if (!aiao->is_init) return AIAO_ERR_UNINIT;

    /* Lock */
    if (aiao->lock)
        aiao->lock();

    /* Loop */

_exit:
    /* Unlock */
    if (aiao->unlock)
        aiao->unlock();

    return err;
}

/**
 * @brief Playback
 * @return
*/
static aiao_err_t _aiao_normal_playback(aiao_play_type_t type, ...)
{
    aiao_err_t err = AIAO_OK;
    aiao_t *aiao = (aiao_t *)&aiao_normal;
    aiao_private_t *private = (aiao_private_t *)aiao->private;
    if (!aiao->is_init) return AIAO_ERR_UNINIT;

    /* Lock */
    if (aiao->lock)
        aiao->lock();

    /* Control */
    va_list ap;
    va_start(ap, type);
    switch (type)
    {
    default:
        break;
    }
    va_end(ap);

_exit:
    /* Unlock */
    if (aiao->unlock)
        aiao->unlock();

    return err;
}

/**
 * @brief Control
 * @return
*/
static aiao_err_t _aiao_normal_control(int cmd, ...)
{
    aiao_err_t err = AIAO_OK;
    aiao_t *aiao = (aiao_t *)&aiao_normal;
    aiao_private_t *private = (aiao_private_t *)aiao->private;
    if (!aiao->is_init) return AIAO_ERR_UNINIT;

    /* Lock */
    if (aiao->lock)
        aiao->lock();

    /* Control */
    va_list ap;
    va_start(ap, cmd);
    switch (cmd)
    {
    default:
        break;
    }
    va_end(ap);

_exit:
    /* Unlock */
    if (aiao->unlock)
        aiao->unlock();

    return err;
}

static aiao_t aiao_normal = 
{
    .is_init = 0,
    .private = NULL,
    .init = _aiao_normal_init,
    .deinit = _aiao_normal_deinit,
    .lock = _aiao_normal_lock,
    .unlock = _aiao_normal_unlock,
    .playback = _aiao_normal_playback,
    .loop = _aiao_normal_loop,
    .control = _aiao_normal_control
};

aiao_t *get_aiao_normal_handle(void)
{
    return &aiao_normal;
}
