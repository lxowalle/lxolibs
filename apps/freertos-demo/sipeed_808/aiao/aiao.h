#ifndef __AIAO_H
#define __AIAO_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "common.h"

typedef enum
{
    AIAO_OK = 0,
    AIAO_BUSY,
    AIAO_OVER,
    AIAO_CONTINUE,
    AIAO_ERR_PARAM,
    AIAO_ERR_MEM,
    AIAO_ERR_NORMAL,
    AIAO_ERR_REINIT,
    AIAO_ERR_UNINIT,
    AIAO_ERR_UNKNOWN,
    AIAO_ERR_DB,
    AIAO_ERR_TODO,
    AIAO_ERR_INIT,
    AIAO_ERR_MAX
}aiao_err_t;

typedef enum
{
    AIAO_TYPE_NORMAL,
    AIAO_TYPE_MAX
}aiao_type_t;

typedef enum
{
    AIAO_PLAY_RAM,
    AIAO_PLAY_FLASH   
}aiao_play_type_t;

typedef struct
{
    uint8_t is_init : 1;
    void *private;
    aiao_err_t (*init)(void);
    aiao_err_t (*deinit)(void);
    aiao_err_t (*lock)(void);
    aiao_err_t (*unlock)(void);
    aiao_err_t (*playback)(aiao_play_type_t type, ...);
    aiao_err_t (*loop)(void);
    aiao_err_t (*control)(int cmd, ...);
}aiao_t;

extern aiao_err_t audio_init(void);
extern aiao_err_t audio_deinit(void);
extern aiao_err_t audio_playback(void);
extern aiao_t ao;

#endif /* __AIAO_H */