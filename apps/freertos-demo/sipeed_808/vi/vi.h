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
    VI_TYPE_DUAL_CAM,
    VI_TYPE_TOF_CAM,
    VI_TYPE_MAX
}vi_type_t;

typedef enum
{
    VI_FORMAT_YUV420
}vi_format_t;

typedef enum
{
    VI_OK = 0,
    VI_BUSY,
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
    void *private;
    vi_err_t (*init)(vi_format_t format, uint16_t w, uint16_t h);
    vi_err_t (*deinit)(void);
    vi_err_t (*lock)(void);
    vi_err_t (*unlock)(void);
    vi_err_t (*loop)(void);
    vi_err_t (*control)(int cmd, ...);
    vi_err_t (*snap)(int type, image_t *image);
}vi_t;

vi_err_t vi_choose(vi_type_t type);
extern vi_err_t camera_init(uint16_t w, uint16_t h);
extern vi_err_t camera_deinit(void);
extern vi_err_t camera_snap(uint8_t *data, uint32_t size);
extern vi_t vi;

#endif /* __VI_H */