#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>

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

#define LOGHEX(desc, buff, len) do{\
    printf(TERMINAL_COLOR_GREEN "[%s]:" TERMINAL_COLOR_END, desc);\
    for (int i = 0; i < len; i ++)\
    {\
        printf("%.2X ", buff[i]);\
    }\
    printf("\n");\
}while(0)

#endif // __LOG_H
