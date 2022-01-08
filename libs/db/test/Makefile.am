bin_PROGRAMS                = test_db
test_db_SOURCES             = test.c
test_db_CPPFLAGS            = -I..
test_db_CPPFLAGS            += -I../inc

test_db_SOURCES             += ../src/db.c
test_db_SOURCES             += ../src/facedb.c

noinst_HEADERS              = ../inc/db.h
noinst_HEADERS              += ../inc/facedb.h

# Link libcommon
test_db_CPPFLAGS            += -I$(top_srcdir)/common 
test_db_LDADD               = $(top_srcdir)/common/libcommon.a
