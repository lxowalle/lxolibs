#ifndef __MF_UARTP_BIN_H
#define __MF_UARTP_BIN_H

#include "mf_uartp.h"

/** 串口协议帧总长度 */
#define UARTP_FRAME_SIZE    (256)

/** 串口协议中缓冲帧数 */
#define UARTP_FRAME_BUFFNUM (1)         // Only set to 1,TODO:Support more buff

/**
 *  @brief 串口协议帧格式
 *  @details 
 *  (主机)发送时帧格式：
 *      |帧头(2B)|帧总长度(2B)|CRC校验值(2B)|命令码(1B)|帧数据载荷(nB)| 
 *      (crc校验内容:命令码+帧数据载荷)            
 *  (主机)接收时帧格式：
 *      |帧头(2B)|帧总长度(2B)|CRC校验值(2B)|命令码(1B)|错误码(1B)|帧数据载荷(nB)|    
 *      (crc校验内容:命令码+错误码+帧数据载荷)
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
    BINCMD_PING         = 0x00,     /* 向模块发送ping并等待响应。用来检查是否与模块连接正常 */
    BINCMD_ABORT        = 0x01,     /* 中断模块的一些操作 */
    BINCMD_INFO         = 0x02,     /* 获取模块的版本信息 */
    BINCMD_BAUD         = 0x03,     /* 检查模块是否初始化完成 */
    BINCMD_RECORD       = 0x04,     /* 录入人脸 */
    BINCMD_CONFIRM      = 0x05,     /* 录入人脸确认，在录入多张人脸时使用 */
    BINCMD_DEL          = 0x06,     /* 删除录入的人脸 */
    BINCMD_FR_RUN       = 0x07,     /* 开始/停止人脸识别 */
    BINCMD_FR_RES       = 0x08,     /* (模块主动发送)人脸识别结果 */
    BINCMD_FR_GATE      = 0x09,
    BINCMD_LED          = 0x0A,
    BINCMD_RELAY        = 0x0B,
    BINCMD_RSTCFG       = 0x0C,

    BINCMD_IMPORT       = 0x10, 
    BINCMD_FCNT         = 0x11,
    BINCMD_FLIST        = 0x12,
    BINCMD_FTR          = 0x13,
    BINCMD_FTRALL       = 0x14,
    BINCMD_FACEPOS      = 0x17,
    BINCMD_GET_PICFTR   = 0x16,

	BINCMD_PICADD  		= 0x20,
	BINCMD_STRADD  		= 0x21,
	BINCMD_ITEMDEL 		= 0x22,
	BINCMD_REBOOT  		= 0x23,
	BINCMD_SOFT_CFG 	= 0x24,
	BINCMD_HARD_CFG 	= 0x25,
	BINCMD_PIC_CFG  	= 0x26,
    BINCMD_INVALID      = 0xFF,
}uartp_bin_cmd_t;

typedef enum
{
    UARTPERR_NONE           = 0,
    UARTPERR_PARAM          = 1,
}uartp_bin_err_t;

/**
 * @brief 命令码与回调函数
*/
typedef struct
{
    uartp_bin_cmd_t cmd;
    uartp_cmd_cb_t cb;
}mf_uartp_bin_item_t;

mf_uartp_t *get_uartp_bin_handle(void);

#endif /* __MF_UARTP_BIN_H */