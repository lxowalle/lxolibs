noinst_PROGRAMS       = core              #   编译为可执行文件(不安装)
core_SOURCES        =   src/main.c src/core.c \
                        src/sensor/sensor.c src/sensor/sensor_touch.c \
                        src/test.c
noinst_HEADERS      =   include/main.h include/core.h \
                        include/sensor.h

core_CPPFLAGS     = -Iinclude -I$(top_srcdir)/libs/kalman/inc -I$(top_srcdir)/libs/liblinuxapi/inc           #   c预处理参数

SUBDIR                =  $(top_srcdir)/libs/kalman $(top_srcdir)/libs/liblinuxapi/src                    #   需要递归处理的目录(会找到Makefile.am并执行，注意先后顺序)
SUBDIRS                 = ../../libs/common
core_CPPFLAGS           += -I$(top_srcdir)/libs/common
core_LDADD              = $(top_srcdir)/libs/common/libcommon.a
