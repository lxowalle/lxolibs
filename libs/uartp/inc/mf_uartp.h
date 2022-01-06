#ifndef __MF_UARTP_H
#define __MF_UARTP_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

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

typedef enum
{
    MF_OK = 0,
    MF_ERR_PARAM,
    MF_ERR_MEM,
    MF_ERR_NORMAL,
    MF_ERR_REDEFINE,
    MF_ERR_UNDEFINE,
    MF_ERR_UNKNOWN,
    MF_ERR_TODO,
    MF_ERR_INIT,
    MF_ERR_MAX
}mf_err_t;

typedef mf_err_t (*uartp_cmd_cb_t)(mf_uartp_t* uartp, ...);

struct mf_uartp_t
{
    uint8_t is_init : 1;
    uartp_status_t status;
    void *private;
    mf_err_t (*init)(void);
    mf_err_t (*deinit)(void);
    mf_err_t (*send)(uint8_t *data, int len, int *real_len);
    mf_err_t (*recv)(uint8_t *data, int len, int *real_len);
    mf_err_t (*lock)(void);
    mf_err_t (*unlock)(void);
    mf_err_t (*loop)(void);
    mf_err_t (*control)(int cmd, ...);
};

/**
 * @brief 选择串口协议
 * @details 当前可选的类型
 *          type=UARTP_TYPE_BIN    使用BIN格式串口协议
 * @return 
*/
mf_err_t mf_uartp_choose(uartp_type_t type);

extern mf_err_t uartp_device_init(void);
extern mf_err_t uartp_device_deinit(void);
extern int uartp_device_send(uint8_t *data, int len);
extern int uartp_device_recv(uint8_t *data, int len);
extern mf_uartp_t mf_uartp;

#endif /* __MF_UARTP_H */