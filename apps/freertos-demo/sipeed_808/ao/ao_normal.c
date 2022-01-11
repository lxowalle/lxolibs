#include "ao_normal.h"

typedef struct
{
    char wav[15];
    uint32_t address;
    uint32_t wav_len;
}wav_item_t;

typedef struct
{
    wav_item_t wav_list[10];
}ao_private_t;

static ao_t ao_normal;

/**
 * @brief Init
 * 
 * @return
*/
static ao_err_t _ao_dual_cam_init(void)
{
    ao_err_t err = AO_OK;
    ao_t *ao = (ao_t *)&ao_normal;

    if (ao->is_init) return AO_ERR_REINIT;
    /* Init lock */
    // ...

    /* Lock */
    if (ao->lock)
        ao->lock();

    /* Add private param */
    static ao_private_t private;
    ao->private = &private;

    /* Camera deaoce init */

    /* Reset ao param */

    /* Init over */ 
    ao->is_init = 1;

_exit:
    /* Unlock */
    if (ao->unlock)
        ao->unlock();

    return err;
}

static ao_t ao_normal = 
{
    .is_init = 0,
    .private = NULL,
    .init = NULL,
    .deinit = NULL,
    .lock = NULL,
    .unlock = NULL,
    .playback = NULL,
    .loop = NULL,
    .control = NULL
};

ao_t *get_ao_normal_handle(void)
{
    return &ao_normal;
}
