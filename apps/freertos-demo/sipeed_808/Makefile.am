bin_PROGRAMS                = sipeed_808
sipeed_808_SOURCES          = main.c 
sipeed_808_CPPFLAGS         = -I.

# Object vi
sipeed_808_SOURCES          += vi/vi.c 
sipeed_808_SOURCES          += vi/vi_dual_cam.c
sipeed_808_SOURCES          += vi/use_sdl2_show_v4l2.c
sipeed_808_CPPFLAGS         += -Ivi

# Object uartp
sipeed_808_SOURCES          += uartp/src/linux_uart.c 
sipeed_808_SOURCES          += uartp/src/mf_uartp_bin.c 
sipeed_808_SOURCES          += uartp/src/mf_uartp.c 
sipeed_808_CPPFLAGS         += -Iuartp/inc

# Object ao
sipeed_808_SOURCES          += aiao/aiao.c
sipeed_808_SOURCES          += aiao/aiao_normal.c
sipeed_808_CPPFLAGS         += -Iaiao

# Object flow
sipeed_808_SOURCES          += flow/flow.c
sipeed_808_SOURCES          += flow/flow_normal.c
sipeed_808_CPPFLAGS         += -Iflow

# Link freertos
SUBDIRS                     = ../../../third_party/FreeRTOS-Kernel
sipeed_808_LDADD            = ../../../third_party/FreeRTOS-Kernel/libfreertos.a
sipeed_808_CPPFLAGS         += -I../../../third_party/FreeRTOS-Kernel
sipeed_808_CPPFLAGS         += -I../../../third_party/FreeRTOS-Kernel/include
sipeed_808_CPPFLAGS         += -I../../../third_party/FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix
sipeed_808_CPPFLAGS         += -I../../../third_party/FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix/utils
LIBS                        = -lpthread

# Link libcommon
sipeed_808_CPPFLAGS         += -I$(top_srcdir)/common
sipeed_808_LDADD            += $(top_srcdir)/common/libcommon.a

# Link SDL2
LIBS                        += -lSDL2 -lSDL2_image