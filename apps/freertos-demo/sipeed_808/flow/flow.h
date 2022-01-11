#ifndef __FLOW_H
#define __FLOW_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "common.h"

typedef enum
{
    FLOW_TYPE_NORMAL,
    FLOW_TYPE_MAX
}flow_type_t;

typedef enum
{
    FLOW_OK = 0,
    FLOW_BUSY,
    FLOW_OVER,
    FLOW_CONTINUE,
    FLOW_ERR_PARAM,
    FLOW_ERR_MEM,
    FLOW_ERR_NORMAL,
    FLOW_ERR_REINIT,
    FLOW_ERR_UNINIT,
    FLOW_ERR_UNKNOWN,
    FLOW_ERR_DB,
    FLOW_ERR_TODO,
    FLOW_ERR_INIT,
    FLOW_ERR_MAX
}flow_err_t;

typedef struct
{
    uint8_t is_init : 1;
    void *private;
    flow_err_t (*init)(void);
    flow_err_t (*deinit)(void);
    flow_err_t (*lock)(void);
    flow_err_t (*unlock)(void);
    flow_err_t (*loop)(void);
    flow_err_t (*control)(int cmd, ...);
}flow_t;

#endif /* __FLOW_H */
