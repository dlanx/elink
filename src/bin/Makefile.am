
CLEANFILES = Makefile.in

bin_PROGRAMS = elink

elink_SOURCES = \
algorithm.c \
log.c \
main.c

elink_CPPFLAGS = @LIBDCA_CFLAGS@ @EINA_CFLAGS@
elink_CFLAGS = -ggdb3 @EINA_CFLAGS@
elink_LDADD = @LIBDCA_LIBS@ @EINA_LIBS@
