#ifndef __MF_UARTP_BIN_H
#define __MF_UARTP_BIN_H

#include "mf_uartp.h"

/** 串口协议帧总长度 */
#define UARTP_FRAME_SIZE    (256)

/** 串口协议中缓冲帧数 */
#define UARTP_FRAME_BUFFNUM (1)         // Only set to 1,TODO:Support more buff

/**
 *  @brief 串口协议帧格式
 *  @details |帧头(2B)|CRC校验值(2B)|帧长度(2B)|命令码(1B)|帧数据载荷(nB)|
 */
typedef struct
{
    uint16_t head;
    uint16_t len;
    uint16_t crc;
    uint8_t cmd;
    uint8_t data[UARTP_FRAME_SIZE - 7];
}__attribute__((packed)) uartp_bin_frame_t;

typedef enum
{
    HEXCMD_PING         = 0x00,
    HEXCMD_ABORT        = 0x01,
    HEXCMD_INFO         = 0x02,
    HEXCMD_BAUD         = 0x03,
    HEXCMD_RECORD       = 0x04,
    HEXCMD_CONFIRM      = 0x05,
    HEXCMD_DEL          = 0x06,
    HEXCMD_FR_RUN       = 0x07,
    HEXCMD_FR_RES       = 0x08,             // 这个命令用来接收外部响应
    HEXCMD_FR_GATE      = 0x09,
    HEXCMD_LED          = 0x0A,
    HEXCMD_RELAY        = 0x0B,
    HEXCMD_RSTCFG       = 0x0C,
    HEXCMD_REBOOT       = 0x0D,

    HEXCMD_IMPORT       = 0x10,
    HEXCMD_FCNT         = 0x11,
    HEXCMD_FLIST        = 0x12,
    HEXCMD_FTR          = 0x13,
    HEXCMD_FTRALL       = 0x14,
    HEXCMD_FACEPOS      = 0x15,
    HEXCMD_GET_PICFTR   = 0x16,

    HEXCMD_PICADD       = 0x20,
    HEXCMD_STRADD       = 0x21,
    HEXCMD_DISDEL       = 0x22,

    HEXCMD_QRSCAN       = 0x30,
    HEXCMD_QRRES        = 0x31,
    HEXCMD_PIC_SNAP     = 0x32,
    HEXCMD_PLAYBACK     = 0x33,
    HEXCMD_INVALID      = 0xFF,
}uartp_bin_cmd_t;

/**
 * @brief 命令码与回调函数
*/
typedef struct
{
    uartp_bin_cmd_t cmd;
    uartp_cmd_cb_t cb;
}mf_uartp_bin_item_t;

mf_uartp_t *get_uartp_bin(void);

#endif /* __MF_UARTP_BIN_H */