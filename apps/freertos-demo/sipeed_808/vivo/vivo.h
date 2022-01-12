#ifndef __VIVO_H
#define __VIVO_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "common.h"
typedef enum
{
    VIVO_TYPE_USB_CAM = 0,
    VIVO_TYPE_DUAL_CAM,
    VIVO_TYPE_TOF_CAM,
    VIVO_TYPE_MAX
}vivo_type_t;

typedef enum
{
    VIVO_FORMAT_JPEG,
    VIVO_FORMAT_YUV420
}vivo_format_t;

typedef enum
{
    VIVO_OK = 0,
    VIVO_BUSY,
    VIVO_OVER,
    VIVO_CONTINUE,
    VIVO_ERR_PARAM,
    VIVO_ERR_MEM,
    VIVO_ERR_NORMAL,
    VIVO_ERR_REINIT,
    VIVO_ERR_UNINIT,
    VIVO_ERR_UNKNOWN,
    VIVO_ERR_DB,
    VIVO_ERR_TODO,
    VIVO_ERR_INIT,
    VIVO_ERR_MAX
}vivo_err_t;

typedef struct
{
    uint8_t is_init : 1;
    void *private;
    vivo_err_t (*init)(vivo_format_t format, uint16_t w, uint16_t h);
    vivo_err_t (*deinit)(void);
    vivo_err_t (*lock)(void);
    vivo_err_t (*unlock)(void);
    vivo_err_t (*loop)(void);
    vivo_err_t (*control)(int cmd, ...);
    vivo_err_t (*snap)(int type, image_t *image);
}vivo_t;

vivo_err_t vivo_choose(vivo_type_t type);
extern vivo_err_t camera_init(uint16_t w, uint16_t h);
extern vivo_err_t camera_deinit(void);
extern vivo_err_t camera_get_frame(uint8_t *buffer, uint32_t buffer_max_size);
extern vivo_t vivo;

#endif /* __VIVO_H */