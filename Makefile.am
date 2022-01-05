SUBDIRS =   apps/app1\
            apps/core\
            apps/template\
            libs/kalman/lib\
            libs/kalman/test\
            libs/liblinuxapi/src\
            libs/libfatfs/source\
            libs/uart_protocol\
            libs/uart_protocol/test

# automake need known where it is

CURRENTPATH = ${PWD}

# head files while linking

AM_CPPFLAGS =   -I$(CURRENTPATH)/libs/lib_a/inc\
                -I$(CURRENTPATH)/libs/lib_b/inc\
                -I$(CURRENTPATH)/libs/lib_c/inc
            
export AM_CPPFLAGS