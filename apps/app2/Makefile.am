# executable file last created

noinst_PROGRAMS= app2

# main entry file

app2_SOURCES= main.c

# object file that needed

OBJECTS= $(top_srcdir)/libs/lib_a/src/lib_a.o\
         $(top_srcdir)/libs/lib_b/src/lib_b.o\
         $(top_srcdir)/libs/lib_c/src/lib_c.o

# load object file when linking

app2_LDADD= $(OBJECTS)

# flags of libs

app2_LDFLAGS= -D_GNU_SOURCE

DEFS += -D_GNU_SOURCE

#LIBS= -lpthread