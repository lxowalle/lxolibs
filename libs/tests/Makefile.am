############################# v4l2 test #############################
bin_PROGRAMS                = test_v4l2
test_v4l2_SOURCES           = test_v4l2.c

# Link libcommon
test_v4l2_CPPFLAGS          = -I$(top_srcdir)/common 
test_v4l2_LDADD             = $(top_srcdir)/common/libcommon.a

# Link libSDL2
LIBS    = -lSDL2 -lSDL2_image

############################# Freertos test #############################
bin_PROGRAMS                    += test_freertos
test_freertos_SOURCES           = test_freertos.c

# Link freertos
SUBDIRS                         = ../../third_party/FreeRTOS-Kernel
test_freertos_LDADD             = $(top_srcdir)/third_party/FreeRTOS-Kernel/libfreertos.a
test_freertos_CPPFLAGS          = -I$(top_srcdir)/third_party/FreeRTOS-Kernel
test_freertos_CPPFLAGS          += -I$(top_srcdir)/third_party/FreeRTOS-Kernel/include
test_freertos_CPPFLAGS          += -I$(top_srcdir)/third_party/FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix
test_freertos_CPPFLAGS          += -I$(top_srcdir)/third_party/FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix/utils
LIBS                            += -lpthread

# Link libcommon
test_freertos_CPPFLAGS          += -I$(top_srcdir)/common 
test_freertos_LDADD             += $(top_srcdir)/common/libcommon.a