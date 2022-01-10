#ifndef __MO_H
#define __MO_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "common.h"

typedef enum
{
    MO_TYPE_MAX
}mo_type_t;

typedef enum
{
    MO_STA_IDEL = 0,
    MO_STA_BUSY,
    MO_STA_ERROR
}mo_status_t;

typedef enum
{
    MO_OK = 0,
    MO_OVER,
    MO_CONTINUE,
    MO_ERR_PARAM,
    MO_ERR_MEM,
    MO_ERR_NORMAL,
    MO_ERR_REINIT,
    MO_ERR_UNINIT,
    MO_ERR_UNKNOWN,
    MO_ERR_DB,
    MO_ERR_TODO,
    MO_ERR_INIT,
    MO_ERR_MAX
}mo_err_t;

typedef struct
{
    uint8_t is_init : 1;
    mo_status_t status;
    void *private;
    mo_err_t (*init)(void);
    mo_err_t (*deinit)(void);
    mo_err_t (*lock)(void);
    mo_err_t (*unlock)(void);
    mo_err_t (*loop)(void);
    mo_err_t (*control)(int cmd, ...);
    mo_err_t (*playback)(int type);
}mo_t;

mo_t *get_mo_normal_handle(void);

#endif /* __MO_H */