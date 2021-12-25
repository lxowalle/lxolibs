noinst_PROGRAMS= test_fatfs           #生成的目标 
# noinst_LIBRARIES= libfatfs.o         #   编译为库文件(不安装)

test_fatfs_SOURCES= test.c ff.c ffsystem.c ffunicode.c diskio.c
test_fatfs_CPPFLAGS = -I.
noinst_HEADERS      = diskio.h ff.h ffconf.h
AUTOMAKE_OPTIONS    = subdir-objects