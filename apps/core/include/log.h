#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>

#define LOG_COLOR_RED           "\033[31m"
#define LOG_COLOR_YELLOW	    "\033[33m"
#define LOG_COLOR_GREEN		    "\033[32m"
#define LOG_COLOR_BULE		    "\033[34m"
#define LOG_COLOR_END		    "\033[0m"

#define LOGI(format, ...)   do {\
    printf(LOG_COLOR_GREEN __FILE__" (%d): " format LOG_COLOR_END, __LINE__, ##__VA_ARGS__);\
}while(0)

#define LOGW(format, ...)   do {\
    printf(LOG_COLOR_YELLOW __FILE__" (%d): " format LOG_COLOR_END, __LINE__, ##__VA_ARGS__);\
}while(0)

#define LOGE(format, ...)   do {\
    printf(LOG_COLOR_RED __FILE__" (%d): " format LOG_COLOR_END, __LINE__, ##__VA_ARGS__);\
}while(0)

#define LOGD(format, ...)   do {\
    printf(LOG_COLOR_BULE __TIME__ " " __FILE__ " (%d): " format LOG_COLOR_END, __LINE__, ##__VA_ARGS__);\
}while(0)

#define LOGN()   LOGD("none\n")

#endif // __LOG_H
