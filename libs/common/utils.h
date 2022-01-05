#ifndef __UTILS_H
#define __UTILS_H

#include "stdint.h"
#include "stdio.h"

/** 
 * @brief crc16 modbus校验 
 */
uint16_t crc16_xmodem(const uint8_t *buffer, uint32_t buffer_length);

#endif /* __UTILS_H */