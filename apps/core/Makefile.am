noinst_PROGRAMS         = core  
  
core_SOURCES            = main.c
core_SOURCES            += src/core.c
core_SOURCES            += src/sensor/sensor.c
core_SOURCES            += src/sensor/sensor_touch.c
core_SOURCES            += src/test.c
                        
noinst_HEADERS          = main.h
noinst_HEADERS          += include/core.h
noinst_HEADERS          += include/sensor.h
                            
core_CPPFLAGS           = -Iinclude       

# Link libs
SUBDIRS                 = ../../libs/common
core_CPPFLAGS           += -I$(top_srcdir)/libs/common
core_LDADD              = $(top_srcdir)/libs/common/libcommon.a
