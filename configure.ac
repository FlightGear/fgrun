dnl (Process this file with autoconf to produce a configure script.)
AC_REVISION($Revision$)dnl
AC_PREREQ(2.53)

AC_INIT([fgrun], [0.1], [bbright@bigpond.net.au])
AM_CONFIG_HEADER([src/config.h])
AC_CONFIG_SRCDIR([src/main.cxx])
AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)
#AM_MAINTAINER_MODE

PACKAGE=$PACKAGE_NAME
VERSION=$PACKAGE_VERSION
AC_SUBST(PACKAGE)
AC_SUBST(VERSION)
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of this package])
AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version of this package])

AC_CANONICAL_HOST

dnl
dnl Checks for programs
dnl
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)

AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_LN_S
#AM_PROG_LIBTOOL

dnl
dnl Checks for compiler features
dnl

dnl
dnl Checks for libraries
dnl

AC_PATH_XTRA
if test "$x_libraries" = "/usr/lib"; then
        echo "Ignoring X library directory \"$x_libraries\" requested by configure."
        x_libraries="NONE"
fi
if test ! "$x_libraries" = "NONE" -a ! "$x_libraries" = ""; then
        LDFLAGS="$LDFLAGS -L$x_libraries"
fi
if test "$x_includes" = "/usr/include"; then
        echo "Ignoring X include directory \"$x_includes\" requested by configure."
        x_includes="NONE"
fi
if test ! "$x_includes" = "NONE" -a ! "$x_includes" = ""; then
        CPPFLAGS="$CPPFLAGS -I$x_includes"
fi

dnl
dnl Checks for header files.
dnl

AC_HEADER_STDC
AC_CHECK_HEADERS(signal.h unistd.h string.h)

dnl
dnl Checks for typedefs, structures, and compiler characteristics.
dnl

AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

dnl
dnl Checks for library functions.
dnl

AC_TYPE_SIGNAL
AC_FUNC_STRFTIME
AC_FUNC_FORK
AC_CHECK_FUNCS(socket strdup strcasecmp atexit)

dnl Check for fltk

FLUID="fluid"

AC_ARG_WITH(fltk, dnl
            [  --with-fltk=DIR      Path to FLTK v1.1],
            FLTK="$withval",)

CPPFLAGS="$CPPFLAGS -I$FLTK/include"
LDFLAGS="$LDFLAGS -L$FLTK/lib"

AC_LANG_PUSH(C++)

AC_CHECK_HEADER(FL/Fl.h)

save_LIBS=$LIBS
LIBS="$LIBS -lfltk -lX11"
AC_MSG_CHECKING([for Fl::run in -lfltk])
AC_TRY_LINK(dnl
  [#include <FL/Fl.h>],
  [Fl::run()],
  AC_MSG_RESULT(yes),
  AC_MSG_RESULT(no)
  LIBS="$save_LIBS"
)

AC_LANG_POP

FLUID_COMPILE="$FLUID -c"
AC_SUBST(FLUID_COMPILE)

AC_CONFIG_FILES([
Makefile
src/Makefile
])

AC_OUTPUT
