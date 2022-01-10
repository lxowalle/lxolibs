bin_PROGRAMS            = sipeed_808
sipeed_808_SOURCES           = main.c 
sipeed_808_CPPFLAGS          = -I.

# Object vi
sipeed_808_SOURCES           += vi/vi.c 
sipeed_808_SOURCES           += vi/use_sdl2_show_v4l2.c
sipeed_808_CPPFLAGS          += -Ivi

SUBDIRS                 = ../../../third_party/FreeRTOS-Kernel
sipeed_808_LDADD             = ../../../third_party/FreeRTOS-Kernel/libfreertos.a
sipeed_808_CPPFLAGS          += -I../../../third_party/FreeRTOS-Kernel
sipeed_808_CPPFLAGS          += -I../../../third_party/FreeRTOS-Kernel/include
sipeed_808_CPPFLAGS          += -I../../../third_party/FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix
sipeed_808_CPPFLAGS          += -I../../../third_party/FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix/utils
LIBS                    = -lpthread

# Link libcommon
sipeed_808_CPPFLAGS          += -I$(top_srcdir)/common
sipeed_808_LDADD             += $(top_srcdir)/common/libcommon.a

# Link libuartp
SUBDIRS                 += ../../../libs/uartp
sipeed_808_CPPFLAGS          += -I../../../libs/uartp/inc
sipeed_808_LDADD             += ../../../libs/uartp/libuartp.a

# Link SDL2
LIBS                    += -lSDL2 -lSDL2_image