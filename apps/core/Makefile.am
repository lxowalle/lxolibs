noinst_PROGRAMS         = core  
  
core_SOURCES            = main.c
core_SOURCES            += core.c
core_SOURCES            += system/sensory/sensor.c
core_SOURCES            += system/sensory/sensor_touch.c
core_CPPFLAGS           = -Isystem/sensory 

# Object nerve system
core_SOURCES            += tissue/nervious/neuron.c
core_SOURCES            += tissue/nervious/cell.c
core_CPPFLAGS           += -Itissue/nervious

# Link libs
core_CPPFLAGS           += -I$(top_srcdir)/common
core_LDADD              = $(top_srcdir)/common/libcommon.a
