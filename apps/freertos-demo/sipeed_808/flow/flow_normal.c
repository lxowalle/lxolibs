#include "flow_normal.h"

typedef struct
{
    char name[15];
}flow_private_t;

static flow_t flow_normal;

/**
 * @brief Init
 * 
 * @return
*/
static flow_err_t _flow_normal_init(void)
{
    flow_err_t err = FLOW_OK;
    flow_t *flow = (flow_t *)&flow_normal;

    if (flow->is_init) return FLOW_ERR_REINIT;
    /* Init lock */
    // ...

    /* Lock */
    if (flow->lock)
        flow->lock();

    /* Add private param */
    static flow_private_t private;
    flow->private = &private;

    /* Audio  init */

    /* Reset flow param */

    /* Init over */ 
    flow->is_init = 1;

_exit:
    /* Unlock */
    if (flow->unlock)
        flow->unlock();

    return err;
}

/**
 * @brief Deinit
 * @return
*/
static flow_err_t _flow_normal_deinit(void)
{
    flow_err_t err = FLOW_OK;
    flow_t *flow = (flow_t *)&flow_normal;
    if (!flow->is_init) return FLOW_ERR_UNINIT;

    /* Lock */
    if (flow->lock)
        flow->lock();

    /* Deinit */

_exit:
    /* Unlock */
    if (flow->unlock)
        flow->unlock();

    /* Deinit lock */
    // ...

    return err;
}

/**
 * @brief Lock
 * @return
*/
static flow_err_t _flow_normal_lock(void)
{
    // Lock
    flow_err_t err = FLOW_OK;

    return err;
}

/**
 * @brief Unlock
 * @return
*/
static flow_err_t _flow_normal_unlock(void)
{
    // Unlock
    flow_err_t err = FLOW_OK;

    return err;
}

/**
 * @brief Loop
 * @return
*/
static flow_err_t _flow_normal_loop(void)
{
    flow_err_t err = FLOW_OK;
    flow_t *flow = (flow_t *)&flow_normal;
    flow_private_t *private = (flow_private_t *)flow->private;
    if (!flow->is_init) return FLOW_ERR_UNINIT;

    /* Lock */
    if (flow->lock)
        flow->lock();

    /* Loop */

_exit:
    /* Unlock */
    if (flow->unlock)
        flow->unlock();

    return err;
}

/**
 * @brief Control
 * @return
*/
static flow_err_t _flow_normal_control(int cmd, ...)
{
    flow_err_t err = FLOW_OK;
    flow_t *flow = (flow_t *)&flow_normal;
    flow_private_t *private = (flow_private_t *)flow->private;
    if (!flow->is_init) return FLOW_ERR_UNINIT;

    /* Lock */
    if (flow->lock)
        flow->lock();

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
    if (flow->unlock)
        flow->unlock();

    return err;
}

static flow_t flow_normal = 
{
    .is_init = 0,
    .private = NULL,
    .init = _flow_normal_init,
    .deinit = _flow_normal_deinit,
    .lock = _flow_normal_lock,
    .unlock = _flow_normal_unlock,
    .loop = _flow_normal_loop,
    .control = _flow_normal_control
};

flow_t *get_flow_normal_handle(void)
{
    return &flow_normal;
}