
CLEANFILES = Makefile.in

bin_PROGRAMS = elink

elink_SOURCES = \
algorithm.h \
config.h \
elink.h \
log.h \
algorithm.c \
log.c \
main.c

elink_CPPFLAGS = @ELINK_CFLAGS@
elink_CFLAGS = -ggdb3 @ELINK_CFLAGS@
elink_LDADD = @ELINK_LIBS@
