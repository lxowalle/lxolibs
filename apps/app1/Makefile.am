# executable file last created
noinst_PROGRAMS= app1
app1_SOURCES= main.c

# Link libcommon
SUBDIRS = ../../libs/common
app1_CPPFLAGS = -I$(top_srcdir)/libs/common
app1_LDADD = $(top_srcdir)/libs/common/libcommon.a

# Link libSDL2
app1_CPPFLAGS += -I/usr/include/SDL2
LIBS    = -lSDL2

# Link libdb
SUBDIRS += ../../libs/db
app1_CPPFLAGS += -I(top_srcdir)/libs/db
app1_LDADD = $(top_srcdir)/libs/db/libdb.a

# flags of libs
# app1_LDFLAGS= -D_GNU_SOURCE
