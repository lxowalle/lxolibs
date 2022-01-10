#ifndef __VI_H
#define __VI_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "common.h"
typedef enum
{
    VI_TYPE_USB_CAM = 0,
    VI_TYPE_MAX
}vi_type_t;

typedef enum
{
    VI_STA_IDEL = 0,
    VI_STA_BUSY,
    VI_STA_ERROR
}vi_status_t;

typedef enum
{
    VI_OK = 0,
    VI_OVER,
    VI_CONTINUE,
    VI_ERR_PARAM,
    VI_ERR_MEM,
    VI_ERR_NORMAL,
    VI_ERR_REINIT,
    VI_ERR_UNINIT,
    VI_ERR_UNKNOWN,
    VI_ERR_DB,
    VI_ERR_TODO,
    VI_ERR_INIT,
    VI_ERR_MAX
}vi_err_t;

typedef struct
{
    uint8_t is_init : 1;
    vi_status_t status;
    void *private;
    vi_err_t (*init)(void);
    vi_err_t (*deinit)(void);
    vi_err_t (*lock)(void);
    vi_err_t (*unlock)(void);
    vi_err_t (*loop)(void);
    vi_err_t (*control)(int cmd, ...);
    vi_err_t (*snap)(int type, image_t **image);
}vi_t;

vi_t *get_vi_usb_cam_handle(void);

#endif /* __VI_H */