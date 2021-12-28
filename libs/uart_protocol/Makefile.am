## 
#   这是一个Makefile.am的模板，只在开发时作为参考使用
##
# noinst_PROGRAMS       = uart_protocol              #   编译为可执行文件(不安装)
# bin_PROGRAMS          =                       #   编译为可执行文件(安装到bin目录)

noinst_LIBRARIES        = libuart_protocol.a                      #   编译为库文件(不安装)
# template_LIBRARIES    =                       #   编译为库文件(安装目录与可执行文件相同)
# template_LTLIBRARIES  =                       #   编译为库文件(libtool)(安装目录与可执行文件相同)

libuart_protocol_a_SOURCES      = src/mf_uartp.c src/mf_uartp_bin.c
noinst_HEADERS          = inc/mf_uartp.h inc/mf_err.h inc/mf_uartp_bin.h inc/mf_log.h 
# template_HEADERS      =                       #   目标文件依赖的所有头文件(安装目录与可执行文件相同)

# template_LIBADD       =                       #   编译时需要加载的其他的库
# template_LDADD        =                       #   链接时需要的所有库文件

libuart_protocol_a_CPPFLAGS     = -Iinc         #   c预处理参数
# template_CFLAGS       =                       #   c编译选项
# template_CXXFLAGS     =                       #   c++编译选项

# template_LDFLAGS      =                       #   链接库标志位

# templatedir           =                       #   数据文件的安装目录
# template_DATA         =                       #   数据文件

# EXTRA_DIST            =                       #   需要打包的所有文件
# SUBDIR                =                       #   需要递归处理的目录(会找到Makefile.am并执行，注意先后顺序)

AUTOMAKE_OPTIONS      = subdir-objects        #   AUTOMAKE编译选项,详情见(https://www.gnu.org/software/automake/manual/html_node/List-of-Automake-options.html)

# 编译动态库的方式
# projectlibdir=$(libdir)           //新建一个目录，就是该目录就是lib目录
# projectlib_PROGRAMS=project.so
# project_so_SOURCES=xxx.C
# project_so_LDFLAGS=-shared -fpic //GCC编译动态库的选项