#ifndef __MF_LOG_H
#define __MF_LOG_H

#include <stdio.h>
#include <stdarg.h>

#define TERMINAL_COLOR_RED              "\033[31m"
#define TERMINAL_COLOR_YELLOW	        "\033[33m"
#define TERMINAL_COLOR_GREEN		    "\033[32m"
#define TERMINAL_COLOR_BULE		        "\033[34m"
#define TERMINAL_COLOR_END		        "\033[0m"

#define LOGI(format, ...)   do {\
    printf(TERMINAL_COLOR_GREEN __FILE__" (%d): " format TERMINAL_COLOR_END, __LINE__, ##__VA_ARGS__);\
}while(0)

#define LOGW(format, ...)   do {\
    printf(TERMINAL_COLOR_YELLOW __FILE__" (%d): " format TERMINAL_COLOR_END, __LINE__, ##__VA_ARGS__);\
}while(0)

#define LOGE(format, ...)   do {\
    printf(TERMINAL_COLOR_RED __FILE__" (%d): " format TERMINAL_COLOR_END, __LINE__, ##__VA_ARGS__);\
}while(0)

#define LOGD(format, ...)   do {\
    printf(TERMINAL_COLOR_BULE __TIME__ " " __FILE__ " (%d): " format TERMINAL_COLOR_END, __LINE__, ##__VA_ARGS__);\
}while(0)

#define LOGN()   LOGD("none\n")

#define PRINF_HEX_ARR(str,buf,len)\
do{\
    char *buff = (char *)buf;\
    printf("\e[32m[%s](%d):\e[0m", str, len);\
    for (int i = 0;i < len; ++i)\
    {\
        printf("0x%.2X ", buff[i] & 0xff);\
    }\
    printf("\r\n");\
} while (0);

#define LOGA(format, ...)   do {\
{\
    va_list loga_ap;\
    va_start(loga_ap, format);\
    uint8_t *data = (uint8_t *)va_arg(loga_ap, void *);\
    int len = (int)va_arg(loga_ap, int);\
    printf(TERMINAL_COLOR_GREEN __TIME__ " " __FILE__ " %s(%d): ", format, __LINE__);\
    for (int i = 0; i < len; i ++)\
    {\
        printf("0x%.2X ", data[i] & 0xff);\
    }\
    printf(TERMINAL_COLOR_END);\
    printf("\r\n");\
    va_end(loga_ap);\
}\
}while(0)

#endif // __MF_LOG_H
