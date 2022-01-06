bin_PROGRAMS            = demo0
demo0_SOURCES           = main.c
demo0_CPPFLAGS          = -I.

SUBDIRS                 = ../../../third_party/FreeRTOS-Kernel
demo0_LDADD             = ../../../third_party/FreeRTOS-Kernel/libfreertos.a
demo0_CPPFLAGS          += -I../../../third_party/FreeRTOS-Kernel
demo0_CPPFLAGS          += -I../../../third_party/FreeRTOS-Kernel/include
demo0_CPPFLAGS          += -I../../../third_party/FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix
demo0_CPPFLAGS          += -I../../../third_party/FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix/utils
LIBS                    = -lpthread

# Link libcommon
SUBDIRS                 += ../../../libs/common
demo0_CPPFLAGS          += -I../../../libs/common
demo0_LDADD             += ../../../libs/common/libcommon.a

