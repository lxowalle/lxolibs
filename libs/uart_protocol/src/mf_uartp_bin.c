#include "mf_uartp_bin.h"
#include "linux_uart.h"
int linux_uart_fd = 0;
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
 * @brief Init uart device
 * @return 
*/
static mf_err_t _uart_device_init(void)
{
    mf_err_t err = MF_OK;

    // TODO
    // ...

    // TEST
    mf_brd_uart_t uart_cfg = 
    {
        .baud = 115200,
        .data_bits = 8,
        .parity = 'N',
        .stop_bits = 1
    };
    linux_uart_fd = linux_uart_init("/dev/ttyUSB0", &uart_cfg);
    printf("Linux uart fd:%d\n", linux_uart_fd);

    return err;
}

/**
 * @brief Deinit uart device
 * @return 
*/
static mf_err_t _uart_device_deinit(void)
{
    mf_err_t err = MF_OK;

    // TODO
    // ...

    // TEST
    linux_uart_deinit(linux_uart_fd);
    return err;
}

/**
 * @brief Uart send data
 * @return
*/
static int _uart_device_send(uint8_t *data, int len)
{
    int real_len = 0;

    // TODO
    // ...

    // TEST
    real_len = linux_uart_write(linux_uart_fd, len, data);
    LOGI("fd:%d Real len:%d\n", linux_uart_fd, real_len);
    return real_len;
}

/**
 * @brief Uart recv data
 * @return
*/
static int _uart_device_recv(uint8_t *data, int len)
{
    int real_len = 0;

    // TODO
    // ...

    // TEST
    real_len = linux_uart_read(linux_uart_fd, len, data);
    LOGI("fd:%d Real len:%d\n", linux_uart_fd, real_len);
    return real_len;
}

/**
 * @brief Private data
*/
typedef struct
{
    uint8_t recv_buff[UARTP_FRAME_SIZE * UARTP_FRAME_BUFFNUM];
    int recv_cnt;
}uartp_private_t;

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
 * @brief Check crc16
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
    int *recv_cnt = &private->recv_cnt;
    uartp_bin_frame_t *recv_ptl = (uartp_bin_frame_t *)private->recv_buff;
    int len = 0;

    if (*recv_cnt < 2)
    {
        err = uartp->ops.recv(uartp, recv_ptl + *recv_cnt, 2 - *recv_cnt, &len);
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
        err = uartp->ops.recv(uartp, recv_ptl + *recv_cnt, 6 - *recv_cnt, &len);
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
        err = uartp->ops.recv(uartp, recv_ptl + *recv_cnt, recv_ptl->len - *recv_cnt, &len);
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
    int len = private->recv_cnt;

    if (uartp->status != UARTP_RECV_OVER)   return MF_OK;

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
    err = _uart_device_init();
    if (MF_OK != err) return MF_ERR_UNKNOW;

    /* Reset uartp param */
    uartp_private_t *private = (uartp_private_t *)uartp->private;
    if (!private) 
    {
        LOGE("Uartp have not private data\n");
        return MF_ERR_TODO;
    }

    memset(private->recv_buff, 0, UARTP_FRAME_BUFFNUM * UARTP_FRAME_SIZE);
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
    err = _uart_device_deinit();
    if (MF_OK != err) return MF_ERR_UNKNOW;

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
 * @details 
 * 
 * @param [in]  uartp   句柄
 * @param [in]  arg1    数据指针(uint8_t *)
 * @param [in]  arg2    数据长度(int)
 * @param [out] arg3    实际发送数据的长度(int *)
 * 
 * @return 
*/
static mf_err_t _uartp_send(mf_uartp_t *uartp, ...)
{
    mf_err_t err = MF_OK;
    if (!uartp)  return MF_ERR_PARAM;
    if (!uartp->is_init) return MF_ERR_UNDEFINE;

    /* Lock */
    if (uartp->ops.lock)
        uartp->ops.lock();

    /* Send */
    va_list ap;
    va_start(ap, uartp);

    uint8_t *data = (uint8_t *)va_arg(ap, void *);
    uint8_t len = (int)va_arg(ap, int);
    int *real_len = (int *)va_arg(ap, void *);

    if (!data || !real_len) return MF_ERR_PARAM;
    *real_len = _uart_device_send(data, len);

    va_end(ap);

    /* Unlock */
    if (uartp->ops.unlock)
        uartp->ops.unlock();

    return err;
}

/**
 * @brief Recv msg
 * 
 * @param [in]  uartp   句柄
 * @param [in]  arg1    数据指针(uint8_t *)
 * @param [in]  arg2    数据长度(int)
 * @param [out] arg3    实际接收数据的长度(int *)
 * 
 * @return 
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
    va_list ap;
    va_start(ap, uartp);

    uint8_t *data = (uint8_t *)va_arg(ap, void *);
    uint8_t len = (int)va_arg(ap, int);
    int *real_len = (int *)va_arg(ap, void *);

    if (!data || !real_len) return MF_ERR_PARAM;
    *real_len = _uart_device_recv(data, len);
    // LOGA("123", data, *real_len);
    va_end(ap);

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
 * @param [in]  arg1    命令码(uartp_bin_cmd_t)
 * @param [in]  arg2    数据段指针(uint8_t *)
 * @param [in]  arg3    数据段长度(int)
 * @param [in]  arg4    实际发送的长度
 * 
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

    uartp_bin_cmd_t cmd = va_arg(ap, int);
    uint8_t *data = (uint8_t *)va_arg(ap, void *);
    int len = (int)va_arg(ap, int);
    int *real_len = (int *)va_arg(ap, void *);
    if (!data || !real_len) return MF_ERR_PARAM;
    if (len + 7 > UARTP_FRAME_SIZE) return MF_ERR_PARAM;

    uartp_bin_frame_t frame;
    frame.head = 0x4040;
    frame.cmd = cmd;
    frame.len = len + 7;
    memcpy(frame.data, data, len);
    frame.crc = crc16_xmodem((const uint8_t *)&frame, frame.len);
    *real_len = _uart_device_send((uint8_t *)&frame, frame.len);

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
    // Unlock
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
    _receive_data_handler(uartp);       // Recv and parse
    _receive_data_handler2(uartp);      // Check and exec

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
mf_err_t _uartp_control(mf_uartp_t *uartp, ...)
{
    mf_err_t err = MF_ERR;
    if (!uartp)  return MF_ERR_PARAM;
    if (!uartp->is_init) return MF_ERR_UNDEFINE;

    /* Lock */
    if (uartp->ops.lock)
        uartp->ops.lock();

    /* Control */
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
 * @brief Get uartp bin handle
*/
mf_uartp_t *get_uartp_bin(void)
{
    return &uartp_bin;
}