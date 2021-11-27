SUBDIRS =   libs/lib_a/src\
            libs/lib_b/src\
            libs/lib_c/src\
            apps/app1\
            apps/app2

# automake need known where it is

CURRENTPATH= ${PWD}

# head files while linking

AM_CPPFLAGS =   -I$(CURRENTPATH)/libs/lib_a/inc\
                -I$(CURRENTPATH)/libs/lib_b/inc\
                -I$(CURRENTPATH)/libs/lib_c/inc
            
export AM_CPPFLAGS