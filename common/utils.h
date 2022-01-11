#ifndef __UTILS_H
#define __UTILS_H

#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "stddef.h"
#include "stdlib.h"

typedef struct
{
    uint8_t *addr;
    uint16_t w;
    uint16_t h;
    uint16_t pixel;
}image_t;

/** 
 * @brief crc16 modbus校验 
 */
uint16_t crc16_xmodem(const uint8_t *buffer, uint32_t buffer_length);

#endif /* __UTILS_H */