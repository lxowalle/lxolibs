# executable file last created
noinst_PROGRAMS= app1
app1_SOURCES= main.c

# Link libcommon
SUBDIRS = ../../libs/common
app1_CPPFLAGS = -I$(top_srcdir)/libs/common
app1_LDADD = $(top_srcdir)/libs/common/libcommon.a

# flags of libs
app1_LDFLAGS= -D_GNU_SOURCE
