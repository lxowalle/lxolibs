## 
#   这是一个Makefile.am的模板，只在开发时作为参考使用
##
noinst_PROGRAMS       = core              #   编译为可执行文件(不安装)
# bin_PROGRAMS          =                       #   编译为可执行文件(安装到bin目录)

# noinst_LIBRARIES      =                       #   编译为库文件(不安装)
# core_LIBRARIES    =                       #   编译为库文件(安装目录与可执行文件相同)
# core_LTLIBRARIES  =                       #   编译为库文件(libtool)(安装目录与可执行文件相同)

core_SOURCES        =   src/main.c src/core.c \
                        src/sensor/sensor.c src/sensor/sensor_touch.c

noinst_HEADERS      =   include/main.h include/core.h \
                        include/log.h include/sensor.h \
                        include/err.h
# core_HEADERS      =                       #   目标文件依赖的所有头文件(安装目录与可执行文件相同)

# core_LIBADD       =                       #   编译时需要加载的其他的库
# core_LDADD        = $(top_srcdir)/libs/kalman/libkalman.a                      #   链接时需要的所有库文件

core_CPPFLAGS     = -Iinclude -I$(top_srcdir)/libs/kalman/inc            #   c预处理参数
# core_CFLAGS       =                       #   c编译选项
# core_CXXFLAGS     =                       #   c++编译选项

# core_LDFLAGS      =                       #   链接库标志位

# coredir           =                       #   数据文件的安装目录
# core_DATA         =                       #   数据文件

# EXTRA_DIST            =                       #   需要打包的所有文件
SUBDIR                =  $(top_srcdir)/libs/kalman                     #   需要递归处理的目录(会找到Makefile.am并执行，注意先后顺序)

AUTOMAKE_OPTIONS      = subdir-objects        #   AUTOMAKE编译选项,详情见(https://www.gnu.org/software/automake/manual/html_node/List-of-Automake-options.html)

# 编译动态库的方式
# projectlibdir=$(libdir)           //新建一个目录，就是该目录就是lib目录
# projectlib_PROGRAMS=project.so
# project_so_SOURCES=xxx.C
# project_so_LDFLAGS=-shared -fpic //GCC编译动态库的选项