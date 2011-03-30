dnl Configure script for Ecore
dnl Vincent Torri 2005-01-06
dnl
dnl AM_CHECK_LIBDCA([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for the Ecore library, and define LIBDCA_LIBS and LIBDCA_CFLAGS.
dnl
AC_DEFUN([AM_CHECK_LIBDCA],
   [
    PKG_CONFIG="pkg-config"
    dnl
    dnl Get the prefix where pkg-config is installed.
    dnl
    AC_ARG_WITH(
       [pkg-config],
       AC_HELP_STRING(
          [--with-pkg-config=FILE],
          [pkg-config script to use (eg /usr/bin/pkg-config)]),
       dnl
       dnl Check the given pkg-config script.
       dnl
       [PKG_CONFIG=${withval}
        AC_CHECK_PROG(
           [PKG_CONFIG_IN_PATH],
           [${PKG_CONFIG}],
           [yes],
           [no])
        if test $PKG_CONFIG_IN_PATH = no; then
           echo "ERROR:"
           echo "The pkg-config development script you specified:"
           echo "$PKG_CONFIG"
           echo "was not found.  Please check the path and make sure "
           echo "the script exists and is executable."
           AC_MSG_ERROR(
              [Fatal Error: no pkg-config detected.])
        fi],
       [AC_CHECK_PROG(
           [PKG_CONFIG_IN_PATH],
           [${PKG_CONFIG}],
           [yes],
           [no])
        if test ${PKG_CONFIG_IN_PATH} = no; then
           echo "ERROR:"
           echo "The pkg-config development script was not found in your execute"
           echo "path. This may mean one of several things"
           echo "1. You may not have installed the libdca-devel (or libdca-dev)"
           echo "   packages."
           echo "2. You may have libdca installed somewhere not covered by your path."
           echo ""
           echo "If this is the case make sure you have the packages installed, AND"
           echo "that the pkg-config script is in your execute path (see your"
           echo "shell's manual page on setting the \$PATH environment variable), OR"
           echo "alternatively, specify the script to use with --with-pkg-config."
           AC_MSG_ERROR(
              [Fatal Error: no pkg-config detected.])
        fi])
    LIBDCA_CFLAGS=`$PKG_CONFIG --cflags libdca`
    LIBDCA_LIBS=`$PKG_CONFIG --libs libdca`
    dnl
    dnl Substitution
    dnl
    AC_SUBST([LIBDCA_CFLAGS])
    AC_SUBST([LIBDCA_LIBS])
   ])
dnl End of libdca.m4
