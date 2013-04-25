dnl autoconf initialization
AC_INIT(
   [elink],
   [0.1.0],
   [dennis.yxun@gmail.com])
AC_PREREQ(2.54)

AM_INIT_AUTOMAKE([1.7 gnits dist-bzip2])

AC_CONFIG_MACRO_DIR([m4])

AM_PROG_LIBTOOL

dnl Check preprocessor and compiler
AC_PROG_CPP
AC_PROG_CC
AM_PROG_CC_C_O

PKG_CHECK_MODULES(ELINK,
		  eina
		  elementary)

AC_CONFIG_FILES(
   [Makefile
    src/Makefile
    m4/Makefile
    data/Makefile
    data/images/Makefile
    src/bin/Makefile])
AC_OUTPUT
