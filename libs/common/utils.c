#include "utils.h"


/** 
 * @brief crc16 modbus校验 
 */
uint16_t crc16_xmodem(const uint8_t *buffer, uint32_t buffer_length)
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
