#ifndef __MF_UARTP_H
#define __MF_UARTP_H

#include "mf_err.h"
#include "mf_log.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct mf_uartp_t mf_uartp_t;

typedef enum
{
    UARTP_TYPE_BIN = 0,
    UARTP_TYPE_MAX
}uartp_type_t;

typedef enum
{
    UARTP_IDEL = 0,
    UARTP_RECV_START,
    UARTP_RECV_OVER
}uartp_status_t;

typedef mf_err_t (*uartp_cmd_cb_t)(mf_uartp_t* uartp, ...);

typedef struct
{
    mf_err_t (*init)(mf_uartp_t *uartp);
    mf_err_t (*deinit)(mf_uartp_t *uartp);
    mf_err_t (*send)(mf_uartp_t *uartp, ...);
    mf_err_t (*recv)(mf_uartp_t *uartp, ...);
    mf_err_t (*lock)(void);
    mf_err_t (*unlock)(void);
    mf_err_t (*loop)(mf_uartp_t *uartp, ...);
    mf_err_t (*control)(mf_uartp_t *uartp, ...);
}uartp_ops_t;

struct mf_uartp_t
{
    uint8_t is_init : 1;
    uartp_status_t status;
    void *private;
    uartp_ops_t ops;
};

/**
 * @brief 选择串口协议
 * @details 当前可选的类型
 *          type=UARTP_TYPE_BIN    使用BIN格式串口协议
 * @return 返回串口协议句柄
*/
mf_uartp_t *mf_uartp_choose(uartp_type_t type);

#endif /* __MF_UARTP_H */