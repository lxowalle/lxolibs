#include "../inc/mf_uartp_bin.h"
#include "../inc/linux_uart.h"

#define S_LOGHEX(desc,buf,len)\
do{\
    char *buff = (char *)buf;\
    printf("\e[32m[%s](%d):\e[0m", desc, (int)len);\
    for (int i = 0;i < len; ++i)\
    {\
        printf("0x%.2X ", buff[i] & 0xff);\
    }\
    printf("\r\n");\
} while (0);

#define S_LOGI(format, ...)   do {\
    printf(__FILE__" (%d): " format, __LINE__, ##__VA_ARGS__);\
}while(0)

#define FACE_FTR_LEN    (196)

static mf_uartp_t mf_uartp_bin;

/**
 * @brief ping命令
 * @details 
 * @return
*/
static mf_err_t cb_cmd_ping(mf_uartp_t *uartp, uint8_t *data, int len)
{
    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};
    mf_err_t err = MF_OK;

    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    int pdu_len = frame->len - 7;
    rsp.err_code = UARTPERR_NONE;
    S_LOGHEX("ping", data, len);

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief abort命令
 * @details 
 * @return
*/
static mf_err_t cb_cmd_abort(mf_uartp_t *uartp, uint8_t *data, int len)
{
    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};
    mf_err_t err = MF_OK;

    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    int pdu_len = frame->len - 7;
    rsp.err_code = UARTPERR_NONE;

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief info命令
 * @details 
 * 
 *  示例:
 * 主机发送version命令，然后从机返回版本号，以0x00结束。最大15字节(包括结尾的0x00)
 * 主机发送：0x24 0x24 0x07 0x00 0xFF 0xFF 0x01
 * 模块返回：0x40 0x40 0x11 0x00 0x32 0x6B 0x81 0x00 0x62 0x69 0x6E 0x20 0x76 0x30 0x2E 0x30 0x00
 * @return
*/
static mf_err_t cb_cmd_info(mf_uartp_t *uartp, uint8_t *data, int len)
{
    struct
    {
        uint8_t err_code;
        uint8_t version[15];
    }__attribute__((packed)) rsp = {0};
    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    uint8_t version[15] = "bin v0.0";   // TODO: get version
    snprintf(rsp.version, sizeof(rsp.version), "%s", version);

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp) - sizeof(rsp.version) + strlen(rsp.version) + 1, NULL);
    return err;
}

/**
 * @brief baud命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_baud(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint32_t baud;
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    if (pdu_len == sizeof(req_t))
    {
        S_LOGI("Set baud:%d\n", req->baud);
        // TODO:Now, Set baud
    }
    else
    {
        rsp.err_code = UARTPERR_PARAM;
    }

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief record命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_record(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint16_t id;
        uint8_t timeout_s;
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    if (pdu_len == sizeof(req_t))
    {
        S_LOGI("recface id:%d, timeout:%d s\n", req->id, req->timeout_s);
        // TODO:Now, start record face until timeout
    }
    else
    {
        rsp.err_code = UARTPERR_PARAM;
    }

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief confirm命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_confirm(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint8_t confirm;
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    if (pdu_len == sizeof(req_t))
    {
        S_LOGI("confirm :%d\n", req->confirm);
        // TODO:Now, confirm
    }
    else
    {
        rsp.err_code = UARTPERR_PARAM;
    }

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief del命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_del(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint16_t id;
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    if (pdu_len == sizeof(req_t))
    {
        S_LOGI("del id:%d\n", req->id);
        // TODO
        // ...
    }
    else
    {
        rsp.err_code = UARTPERR_PARAM;
    }

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief fr_run命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_fr_run(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint8_t type;
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    if (pdu_len == sizeof(req_t))
    {
        S_LOGI("run type:%d\n", req->type);
        // TODO
        // ...
    }
    else
    {
        rsp.err_code = UARTPERR_PARAM;
    }

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief fr_gate命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_fr_gate(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint8_t fe_gate;
        uint8_t live_gate;
        uint8_t front_gate;
        uint8_t fe_gate_ir;
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    if (pdu_len < sizeof(req_t) - 1)
    {
        rsp.err_code = UARTPERR_PARAM;
        goto _exit;
    }
        
    if (pdu_len == sizeof(req_t))
    {
        S_LOGI("fe_fate_ir:%d\n", req->fe_gate_ir);
        // TODO:Only set fe_fate_ir
        // ...
    }

    S_LOGI("fe_gate:%d live_gate:%d front_gate:%d\n", req->fe_gate, req->live_gate, req->front_gate);
    // TODO:Only set fe_gate,live_gate,front_gate
    // ...

_exit:
    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief led命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_led(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint8_t duty;
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    if (pdu_len == sizeof(req_t))
    {
        S_LOGI("led duty:%d\n", req->duty);
        // TODO
        // ...
    }
    else
    {
        rsp.err_code = UARTPERR_PARAM;
    }

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief relay命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_relay(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint8_t open;
        uint8_t time_100ms;
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    if (pdu_len == sizeof(req_t))
    {
        S_LOGI("relay open:%d, time:%d (unit:100ms)\n", req->open, req->time_100ms);
        // TODO
        // ...
    }
    else
    {
        rsp.err_code = UARTPERR_PARAM;
    }

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief rstcfg命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_rstcfg(mf_uartp_t *uartp, uint8_t *data, int len)
{
    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    S_LOGI("Reset cfg\n");
    // TODO
    // ...

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief import命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_import(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint16_t id;
        uint8_t face_ftr[FACE_FTR_LEN];
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    if (pdu_len == sizeof(req_t))
    {
        S_LOGI("id:%d\n", req->id);
        S_LOGHEX("ftr", req->face_ftr, sizeof(req->face_ftr));
        // TODO
        // ...
    }
    else
    {
        rsp.err_code = UARTPERR_PARAM;
    }

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief fcnt命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_fcnt(mf_uartp_t *uartp, uint8_t *data, int len)
{
    struct
    {
        uint8_t err_code;
        uint16_t id_num;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    S_LOGI("get id_num:%d\n", rsp.id_num);
    // TODO
    // ...

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief flist命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_flist(mf_uartp_t *uartp, uint8_t *data, int len)
{
    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    struct
    {
        uint16_t id;
    }__attribute__((packed)) rsp2 = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    // need by malloc
    // TODO:

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief ftr命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_ftr(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint16_t id;
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
        uint16_t id;
        uint8_t face_ftr[FACE_FTR_LEN];
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    if (pdu_len == sizeof(req_t))
    {
        S_LOGI("id:%d\n", req->id);
        S_LOGHEX("ftr", rsp.face_ftr, sizeof(rsp.face_ftr));
        // TODO
        // ...
    }
    else
    {
        rsp.err_code = UARTPERR_PARAM;
    }

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}


/**
 * @brief ftrall命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_ftrall(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint16_t id;
        uint8_t face_ftr[FACE_FTR_LEN];
    }__attribute__((packed)) ftr_t;

    struct
    {
        uint8_t err_code;
        ftr_t *ftr;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    // TODO
    // ...

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief picadd命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_pic_add(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint8_t pic_id;
        uint8_t alpha;
        uint8_t resize;
        uint32_t addr;
        uint16_t w;
        uint16_t h;
        uint16_t x;
        uint16_t y;
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    if (pdu_len == sizeof(req_t))
    {
        S_LOGI("pic id:%d\n", req->pic_id);
        // TODO
        // ...
    }
    else
    {
        rsp.err_code = UARTPERR_PARAM;
    }

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief stradd命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_str_add(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint16_t str_id;
        uint16_t x;
        uint16_t y;
        uint8_t size;
        uint16_t color;
        uint16_t bg_color;
        uint8_t zhCN;
        uint8_t str[50];
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    if (pdu_len == sizeof(req_t))
    {
        S_LOGI("id:%d\n", req->str_id);
        // TODO
        // ...
    }
    else
    {
        rsp.err_code = UARTPERR_PARAM;
    }

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief itemdel命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_itemdel(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint16_t del_id;
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    if (pdu_len == sizeof(req_t))
    {
        S_LOGI("id:%d\n", req->del_id);
        // TODO
        // ...
    }
    else
    {
        rsp.err_code = UARTPERR_PARAM;
    }

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief reboot命令
 * @details 
 * 
 *  示例:
 * 主机发送reboot命令，从机返回响应码，短暂延时后重启
 * 主机发送：0x24 0x24 0x07 0x00 0xFF 0xFF 0x02
 * 模块返回：0x24 0x24 0x08 0x00 0xFA 0x7D 0x82 0x00
 * @return
*/
static mf_err_t cb_cmd_reboot(mf_uartp_t *uartp, uint8_t *data, int len)
{
    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};
    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    S_LOGHEX("reboot", data, len);

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);

    // TODO:Now, we need reboot
    // ...

    return err;
}

/**
 * @brief soft_cfg命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_soft_cfg(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint16_t str_id;
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    // TODO
    // ...

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief hard_cfg命令
 * @details 
 * 
 * @return
*/
static mf_err_t cb_cmd_hard_cfg(mf_uartp_t *uartp, uint8_t *data, int len)
{
    typedef struct
    {
        uint16_t str_id;
    }__attribute__((packed)) req_t;

    struct
    {
        uint8_t err_code;
    }__attribute__((packed)) rsp = {0};

    mf_err_t err = MF_OK;
    uartp_bin_frame_t *frame = (uartp_bin_frame_t *)data;
    req_t *req = (req_t *)frame;
    int pdu_len = frame->len;
    rsp.err_code = UARTPERR_NONE;

    // TODO
    // ...

    uartp->send_ptl(frame->cmd | 0x80, (uint8_t *)&rsp, sizeof(rsp), NULL);
    return err;
}

/**
 * @brief Command list
 * @details Add command and cb here.the last one item must set cb to NULL
 * 
*/
static mf_uartp_bin_item_t cmd_list[] = 
{
    {BINCMD_PING,           cb_cmd_ping},
    {BINCMD_ABORT,          cb_cmd_abort},
    {BINCMD_INFO,           cb_cmd_info},
    {BINCMD_BAUD,           cb_cmd_baud},
    {BINCMD_RECORD,         cb_cmd_record},
    {BINCMD_CONFIRM,        cb_cmd_confirm},
    {BINCMD_DEL,            cb_cmd_del},
    {BINCMD_FR_RUN,         cb_cmd_fr_run},
    // {BINCMD_FR_RES | 0x80,  cb_cmd_reboot},
    {BINCMD_FR_GATE,        cb_cmd_fr_gate},
    {BINCMD_LED,            cb_cmd_led},
    {BINCMD_RELAY,          cb_cmd_relay},
    {BINCMD_RSTCFG,         cb_cmd_rstcfg},
    {BINCMD_IMPORT,         cb_cmd_import},
    {BINCMD_FCNT,           cb_cmd_fcnt},
    {BINCMD_FLIST,          cb_cmd_flist},
    {BINCMD_FTR,            cb_cmd_ftr},
    {BINCMD_FTRALL,         cb_cmd_ftrall},
    // {BINCMD_FACEPOS | 0x80, cb_cmd_reboot},
    // {BINCMD_GET_PICFTR,     cb_cmd_reboot},
    {BINCMD_PICADD,         cb_cmd_pic_add},
    {BINCMD_STRADD,         cb_cmd_str_add},
    {BINCMD_ITEMDEL,        cb_cmd_itemdel},
    {BINCMD_REBOOT,         cb_cmd_reboot},
    {BINCMD_SOFT_CFG,       cb_cmd_soft_cfg},
    {BINCMD_HARD_CFG,       cb_cmd_hard_cfg},
    // {BINCMD_PIC_CFG,        cb_cmd_reboot},
    {BINCMD_INVALID,        NULL}                    // Must be exist
};

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
    uartp_private_t *private = (uartp_private_t *)uartp->private;

    for (;item->cb != NULL; ++item)
    {
        if (item->cmd == cmd)
        {
            return item->cb(uartp, private->recv_buff, private->recv_cnt);
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
        err = uartp->recv((uint8_t *)recv_ptl + *recv_cnt, 2 - *recv_cnt, &len);
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
        err = uartp->recv((uint8_t *)recv_ptl + *recv_cnt, 6 - *recv_cnt, &len);
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
        err = uartp->recv((uint8_t *)recv_ptl + *recv_cnt, recv_ptl->len - *recv_cnt, &len);
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
            printf("Invalid crc\n");
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
static mf_err_t _uartp_init(void)
{
    mf_err_t err = MF_OK;
    mf_uartp_t *uartp = (mf_uartp_t *)&mf_uartp_bin;

    if (uartp->is_init) return MF_ERR_REINIT;
    /* Init lock */
    // ...

    /* Lock */
    if (uartp->lock)
        uartp->lock();

    /* Add private param */
    err = uartp_device_init();
    if (MF_OK != err) return err;

    /* Reset uartp param */
    static uartp_private_t private;
    uartp->private = &private;
    uartp->status = UARTP_IDEL;

    /* Init over */ 
    uartp->is_init = 1;

    /* Unlock */
    if (uartp->unlock)
        uartp->unlock();

    return err;
}

/**
 * @brief Deinit
*/
static mf_err_t _uartp_deinit(void)
{
    mf_err_t err = MF_OK;
    mf_uartp_t *uartp = (mf_uartp_t *)&mf_uartp_bin;
    if (!uartp->is_init) return MF_ERR_UNINIT;

    /* Lock */
    if (uartp->lock)
        uartp->lock();

    /* Deinit uart device */
    err = uartp_device_deinit();
    if (MF_OK != err) return MF_ERR_UNKNOWN;

    /* Deinit over */ 
    uartp->is_init = 0;

    /* Unlock */
    if (uartp->lock)
        uartp->lock();

    /* Deinit lock */
    // ...

    return err;
}

/**
 * @brief Send msg
 * @details 
 * 
 * @param [in]  arg1    数据指针(uint8_t *)
 * @param [in]  arg2    数据长度(int)
 * @param [out] arg3    实际发送数据的长度(int *),填NULL则不返回
 * 
 * @return 
*/
static mf_err_t _uartp_send(uint8_t *data, int len, int *real_len)
{
    mf_err_t err = MF_OK;
    mf_uartp_t *uartp = (mf_uartp_t *)&mf_uartp_bin;
    if (!uartp->is_init) return MF_ERR_UNINIT;

    /* Lock */
    if (uartp->lock)
        uartp->lock();
    
    /* Send */
    if (!data) return MF_ERR_PARAM;
    int send_len = uartp_device_send(data, len);
    if (real_len)   
        *real_len = send_len;

    /* Unlock */
    if (uartp->unlock)
        uartp->unlock();

    return err;
}

/**
 * @brief Recv msg
 * 
 * @param [in]  arg1    数据指针(uint8_t *)
 * @param [in]  arg2    数据长度(int)
 * @param [out] arg3    实际接收数据的长度(int *),填NULL不返回
 * 
 * @return 
*/
static mf_err_t _uartp_recv(uint8_t *data, int len, int *real_len)
{
    mf_err_t err = MF_OK;
    mf_uartp_t *uartp = (mf_uartp_t *)&mf_uartp_bin;
    if (!uartp->is_init) return MF_ERR_UNINIT;

    /* Lock */
    if (uartp->lock)
        uartp->lock();

    /* Recv */
    if (!data) return MF_ERR_PARAM;
    int recv_len = uartp_device_recv(data, len);
    if (real_len)
        *real_len = recv_len;

    /* Unlock */
    if (uartp->unlock)
        uartp->unlock();

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
static mf_err_t _uartp_send_ptl(int cmd, uint8_t *data, int len, int *real_len)
{
    mf_err_t err = MF_OK;
    mf_uartp_t *uartp = (mf_uartp_t *)&mf_uartp_bin;
    if (!uartp->is_init) return MF_ERR_UNINIT;

    /* Lock */
    if (uartp->lock)
        uartp->lock();

    /* Send protocol data*/
    if (!data) return MF_ERR_PARAM;
    if (len + 7 > UARTP_FRAME_SIZE) return MF_ERR_PARAM;

    uartp_bin_frame_t frame = {0};
    frame.head = 0x2424;
    frame.cmd = cmd;
    frame.len = len + 7;
    memcpy(frame.data, data, len);
    frame.crc = crc16_xmodem((const uint8_t *)&frame + 6, len + 1);
    uartp->send((uint8_t *)&frame, frame.len, real_len);
    S_LOGHEX("send ptl", (uint8_t *)&frame, frame.len);
    /* Unlock */
    if (uartp->unlock)
        uartp->unlock();

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
static mf_err_t _uartp_loop(void)
{
    mf_err_t err = MF_OK;
    mf_uartp_t *uartp = (mf_uartp_t *)&mf_uartp_bin;
    if (!uartp->is_init) return MF_ERR_UNINIT;

    /* Lock */
    if (uartp->lock)
        uartp->lock();

    /* Handler */
    _receive_data_handler(uartp);       // Recv and parse
    _receive_data_handler2(uartp);      // Check and exec

    /* Unlock */
    if (uartp->unlock)
        uartp->unlock();

    return err;
}

/**
 * @brief Change
 * @param [in]  uartp   串口协议句柄
 * @param [in]  argN    arg1=>命令码(int)
*/
mf_err_t _uartp_control(int cmd, ...)
{
    mf_err_t err = MF_OK;
    mf_uartp_t *uartp = (mf_uartp_t *)&mf_uartp_bin;
    if (!uartp->is_init) return MF_ERR_UNINIT;

    /* Lock */
    if (uartp->lock)
        uartp->lock();

    /* Control */
    va_list ap;
    va_start(ap, cmd);

    switch (cmd)
    {
    default:
        break;
    }

    va_end(ap);

    /* Unlock */
    if (uartp->unlock)
        uartp->unlock();

    return err;
}

static mf_uartp_t mf_uartp_bin = 
{
    .is_init = 0,
    .status = UARTP_IDEL,
    .private = NULL,
    .init = _uartp_init,
    .deinit = _uartp_deinit,
    .send = _uartp_send,
    .recv = _uartp_recv,
    .send_ptl = _uartp_send_ptl,
    .lock = _uartp_lock,
    .unlock = _uartp_unlock,
    .loop = _uartp_loop,
};

/**
 * @brief Get uartp bin handle
*/
mf_uartp_t *get_uartp_bin_handle(void)
{
    return &mf_uartp_bin;
}