# v4l2 test
bin_PROGRAMS                = test_v4l2
test_v4l2_SOURCES           = test_v4l2.c

# Link libcommon
test_v4l2_CPPFLAGS          = -I$(top_srcdir)/common 
test_v4l2_LDADD             = $(top_srcdir)/common/libcommon.a

# Link libSDL2
LIBS    = -lSDL2 -lSDL2_image
