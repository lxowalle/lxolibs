# executable file last created
noinst_PROGRAMS= app1
app1_SOURCES= main.c

# Link libcommon
app1_CPPFLAGS = -I$(top_srcdir)/common 
app1_LDADD = $(top_srcdir)/common/libcommon.a

# Link libSDL2
app1_CPPFLAGS += -I/usr/include/SDL2
LIBS    = -lSDL2
