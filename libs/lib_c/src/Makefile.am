# noinst_PROGRAMS= lib_c              #   编译为可执行文件(不安装)
# bin_PROGRAMS= lib_c                 #   编译为可执行文件(安装到bin目录)

noinst_LIBRARIES=lib_c.a                   #   编译为库文件(不安装)
# lib_c_LIBRARIES=                    #   编译为库文件(安装目录与可执行文件相同)
# lib_c_LTLIBRARIES=                  #   编译为库文件(libtool)(安装目录与可执行文件相同)

lib_c_a_SOURCES= lib_c.c              #   目标文件依赖的所有源文件
# noinst_HEADERS=                     #   目标文件依赖的所有头文件(不安装)
# lib_c_HEADERS=                      #   目标文件依赖的所有头文件(安装目录与可执行文件相同)

# # lib_c_LIBADD=                       #   编译时需要加载的其他的库
# lib_c_LDADD=                        #   链接时需要的所有库文件

# lib_c_CPPFLAGS=                     #   c预处理参数
# lib_c_CFLAGS=                       #   c编译选项
# lib_c_CXXFLAGS=                     #   c++编译选项

# lib_c_LDFLAGS=                      #   链接库标志位

# lib_cdir=                           #   数据文件的安装目录
# lib_c_DATA=                         #   数据文件

# DEFS += -D_GNU_SOURCE

# EXTRA_DIST=                         #   需要打包的所有文件
# SUBDIR=                             #   需要递归处理的目录(会找到Makefile.am并执行，注意先后顺序)

# 编译动态库的方式
# projectlibdir=$(libdir)           //新建一个目录，就是该目录就是lib目录
# projectlib_PROGRAMS=project.so
# project_so_SOURCES=xxx.C
# project_so_LDFLAGS=-shared -fpic //GCC编译动态库的选项