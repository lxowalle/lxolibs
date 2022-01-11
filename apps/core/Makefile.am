noinst_PROGRAMS         = core  
  
core_SOURCES            = main.c
core_SOURCES            += core.c
core_SOURCES            += system/sensory/sensor.c
core_SOURCES            += system/sensory/sensor_touch.c
core_CPPFLAGS           = -Isystem/sensory 

# Link libs
core_CPPFLAGS           += -I$(top_srcdir)/common
core_LDADD              = $(top_srcdir)/common/libcommon.a
