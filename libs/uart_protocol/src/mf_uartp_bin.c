#include "mf_uartp_bin.h"

typedef struct
{
    uint8_t recv_buff[UARTP_FRAME_SIZE * UARTP_FRAME_BUFFNUM];
    size_t recv_cnt;
}uartp_private_t;

/**
 * @brief Command list
 * @details Add command and cb here.the last one item must set cb to NULL
 * 
*/
static mf_uartp_bin_item_t cmd_list[] = 
{
    {HEXCMD_INVALID,    NULL}                    // Must be exist
};

/**
 * @brief Find cmd and exec cb
*/
static mf_err_t _uartp_exec_cmd(mf_uartp_t *uartp, uartp_bin_cmd_t cmd)
{
    mf_uartp_bin_item_t *item = cmd_list;

    for (;item->cb != NULL; ++item)
    {
        if (item->cmd == cmd)
        {
            return item->cb(uartp, cmd);
        }
    }
}

/** 
 * @brief crc校验 
 */
static uint16_t crc16_xmodem(const uint8_t *buffer, uint32_t buffer_length)
{
    uint8_t c, treat, bcrc;
    uint16_t wcrc = 0;

    for(uint32_t i = 0; i < buffer_length; i++)
    {
        c = buffer[i];
        for(uint8_t j = 0; j < 8; j++)
        {
            treat = c & 0x80;
            c <<= 1;
            bcrc = (wcrc >> 8) & 0x80;
            wcrc <<= 1;
            if(treat != bcrc)
                wcrc ^= 0x1021;
        }
    }
    return wcrc;
}

/**
 * @brief Receive data handler
*/
static mf_err_t _receive_data_handler(mf_uartp_t *uartp)
{
    mf_err_t err = MF_OK;
    uartp_private_t *private = (uartp_private_t *)uartp->private;
    size_t *recv_cnt = &private->recv_cnt;
    uartp_bin_frame_t *recv_ptl = (uartp_bin_frame_t *)private->recv_buff;
    int len = 0;

    if (*recv_cnt < 2)
    {
        err = uartp->ops.recv(uartp, &len); // TODO: 伪代码
        if (err == MF_OK && len > 0)
        {
            *recv_cnt += len;
            uartp->status = UARTP_RECV_START;

            if (*recv_cnt == 2 && recv_ptl->head != 0x2424 && recv_ptl->head != 0x4040)
            {
                if (((recv_ptl->head & 0xff00) == 0x2400))
                {
                    *recv_cnt = 1;
                    recv_ptl->head = 0x24;
                }
                else if ((recv_ptl->head & 0xff00) == 0x4000)
                {
                    *recv_cnt = 1;
                    recv_ptl->head = 0x40;
                }
                else
                {
                    *recv_cnt = 0;
                    uartp->status = UARTP_IDEL;
                }
            }
        }
    }
    else if (*recv_cnt < 6)
    {
        err = uartp->ops.recv(uartp, &len); // TODO: 伪代码
        if (err == MF_OK && len > 0)
        {
            *recv_cnt += len;

            if (*recv_cnt > 4 && (recv_ptl->len <= 6 || recv_ptl->len > UARTP_FRAME_SIZE))
            {
                *recv_cnt = 0;
                uartp->status = UARTP_IDEL;
            }
        }
    }
    else if (*recv_cnt < recv_ptl->len)
    {
        err = uartp->ops.recv(uartp, &len); // TODO: 伪代码
        if (err == MF_OK && len > 0)
        {
            *recv_cnt += len;
            if (*recv_cnt == recv_ptl->len)
            {
                uartp->status = UARTP_RECV_OVER;
            }
        }
    }
    else
    {
        uartp->status = UARTP_IDEL;
        *recv_cnt = 0;
    }

    return err;
}

/**
 * @brief Receive data handler2
*/
static mf_err_t _receive_data_handler2(mf_uartp_t *uartp)
{
    mf_err_t err = MF_OK;

    uartp_private_t *private = (uartp_private_t *)uartp->private;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)private->recv_buff;
    size_t len = private->recv_cnt;

    if (0x4040 == frame->cmd)
    {
        if (frame->crc != crc16_xmodem((const uint8_t *)frame + 6, len - 6))
        {
            LOGW("Invalid crc\n");
            return MF_ERR_TODO;
        }
    }

    err = _uartp_exec_cmd(uartp, frame->cmd);

    uartp->status = UARTP_IDEL;

    return err;
}

/**
 * @brief Init
*/
static mf_err_t _uartp_init(mf_uartp_t *uartp)
{
    mf_err_t err = MF_OK;
    if (uartp == NULL)  return MF_ERR_PARAM;
    if (uartp->is_init) return MF_ERR_REDEFINE;

    /* Init lock */
    // ...

    /* Lock */
    if (uartp->ops.lock)
        uartp->ops.lock();

    /* Init uart device */
    // ...

    /* Reset uartp param */
    uartp_private_t *private = (uartp_private_t *)uartp->private;
    if (!private) 
    {
        LOGE("Uartp have not private data\n");
        return MF_ERR_TODO;
    }

    memset(private->recv_buff, 0, UARTP_FRAME_BUFFNUM * sizeof(uartp_bin_frame_t));
    private->recv_cnt = 0;
    uartp->status = UARTP_IDEL;

    /* Init over */ 
    uartp->is_init = 1;

    /* Unlock */
    if (uartp->ops.unlock)
        uartp->ops.unlock();

    return err;
}

/**
 * @brief Deinit
*/
static mf_err_t _uartp_deinit(mf_uartp_t *uartp)
{
    mf_err_t err = MF_ERR;
    if (uartp == NULL)  return MF_ERR_PARAM;
    if (!uartp->is_init) return MF_ERR_UNDEFINE;

    /* Lock */
    if (uartp->ops.lock)
        uartp->ops.lock();

    /* Deinit uart device */
    // ...

    /* Deinit over */ 
    uartp->is_init = 0;

    /* Unlock */
    if (uartp->ops.lock)
        uartp->ops.lock();

    /* Deinit lock */
    // ...

    return err;
}

/**
 * @brief Send msg
*/
static mf_err_t _uartp_send(mf_uartp_t *uartp, ...)
{
    mf_err_t err = MF_ERR;
    if (!uartp)  return MF_ERR_PARAM;
    if (!uartp->is_init) return MF_ERR_UNDEFINE;

    /* Lock */
    if (uartp->ops.lock)
        uartp->ops.lock();

    /* Send */
    // ...

    /* Unlock */
    if (uartp->ops.unlock)
        uartp->ops.unlock();

    return err;
}

/**
 * @brief Recv msg
*/
static mf_err_t _uartp_recv(mf_uartp_t *uartp, ...)
{
    mf_err_t err = MF_ERR;
    if (!uartp)  return MF_ERR_PARAM;
    if (!uartp->is_init) return MF_ERR_UNDEFINE;

    /* Lock */
    if (uartp->ops.lock)
        uartp->ops.lock();

    /* Recv */
    // ...

    /* Unlock */
    if (uartp->ops.unlock)
        uartp->ops.unlock();

    return err;
}

/**
 * @brief Send protocol msg
 * @details 从串口发送带协议格式的串口消息
 * 
 * @param [in]  uartp   串口协议句柄
 * @param [in]  argN    arg1=>命令码(int)
 * @return 
*/
static mf_err_t _uartp_send_protocol(mf_uartp_t *uartp, ...)
{
    mf_err_t err = MF_ERR;
    if (!uartp)  return MF_ERR_PARAM;
    if (!uartp->is_init) return MF_ERR_UNDEFINE;

    /* Lock */
    if (uartp->ops.lock)
        uartp->ops.lock();

    /* Send protocol */
    va_list ap;
    va_start(ap, uartp);

    int cmd = va_arg(ap, int);

    va_end(ap);

    /* Unlock */
    if (uartp->ops.unlock)
        uartp->ops.unlock();

    return err;
}

/**
 * @brief Lock
*/
static mf_err_t _uartp_lock(void)
{
    // Lock
}

/**
 * @brief Unlock
*/
static mf_err_t _uartp_unlock(void)
{
    // Lock
}

/**
 * @brief Loop
*/
static mf_err_t _uartp_loop(mf_uartp_t *uartp, ...)
{
    mf_err_t err = MF_ERR;
    if (!uartp)  return MF_ERR_PARAM;
    if (!uartp->is_init) return MF_ERR_UNDEFINE;

    /* Lock */
    if (uartp->ops.lock)
        uartp->ops.lock();

    /* Handler */
    _receive_data_handler(uartp);
    if (uartp->status == UARTP_RECV_OVER)
    {
        _receive_data_handler2(uartp);
    }

    /* Unlock */
    if (uartp->ops.unlock)
        uartp->ops.unlock();

    return err;
}

/**
 * @brief Change
 * @param [in]  uartp   串口协议句柄
 * @param [in]  argN    arg1=>命令码(int)
*/
mf_err_t change(mf_uartp_t *uartp, ...)
{
    mf_err_t err = MF_ERR;
    if (!uartp)  return MF_ERR_PARAM;
    if (!uartp->is_init) return MF_ERR_UNDEFINE;

    /* Lock */
    if (uartp->ops.lock)
        uartp->ops.lock();

    /* Change */
    va_list ap;
    va_start(ap, uartp);

    int cmd = va_arg(ap, int);
    switch (cmd)
    {
    default:
        break;
    }

    va_end(ap);

    /* Unlock */
    if (uartp->ops.unlock)
        uartp->ops.unlock();

    return err;
}

static uartp_private_t uartp_private;
static mf_uartp_t uartp_bin = 
{
    .is_init = 0,
    .status = UARTP_IDEL,
    .private = &uartp_private,
    .ops.init = _uartp_init,
    .ops.deinit = _uartp_deinit,
    .ops.send = _uartp_send,
    .ops.recv = _uartp_recv,
    .ops.lock = _uartp_lock,
    .ops.unlock = _uartp_unlock,
    .ops.loop = _uartp_loop,
};

/**
 * @brief 获取本句柄
*/
mf_uartp_t *get_uartp_bin(void)
{
    return &uartp_bin;
}