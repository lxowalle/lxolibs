#ifndef __AO_H
#define __AO_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "common.h"

typedef enum
{
    AO_OK = 0,
    AO_BUSY,
    AO_OVER,
    AO_CONTINUE,
    AO_ERR_PARAM,
    AO_ERR_MEM,
    AO_ERR_NORMAL,
    AO_ERR_REINIT,
    AO_ERR_UNINIT,
    AO_ERR_UNKNOWN,
    AO_ERR_DB,
    AO_ERR_TODO,
    AO_ERR_INIT,
    AO_ERR_MAX
}ao_err_t;

typedef enum
{
    AO_TYPE_NORMAL,
    AO_TYPE_MAX
}ao_type_t;

typedef enum
{
    AO_PLAY_RAM,
    AO_PLAY_FLASH   
}ao_play_type_t;

typedef struct
{
    uint8_t is_init : 1;
    void *private;
    ao_err_t (*init)(void);
    ao_err_t (*deinit)(void);
    ao_err_t (*lock)(void);
    ao_err_t (*unlock)(void);
    ao_err_t (*playback)(ao_play_type_t type, ...);
    ao_err_t (*loop)(void);
    ao_err_t (*control)(int cmd, ...);
}ao_t;

extern ao_err_t audio_init(void);
extern ao_err_t audio_deinit(void);
extern ao_err_t audio_playback(void);
extern ao_t ao;

#endif /* __AO_H */