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

if test "x$with_x" = "xno"; then
  :;
else
    CPPFLAGS="$CPPFLAGS $X_CFLAGS"
    LDFLAGS="$LDFLAGS $X_LIBS"

    AC_CHECK_LIB(X11, XCreateWindow)
    AC_CHECK_LIB(Xext, XShmCreateImage)
    AC_CHECK_LIB(Xi, XGetExtensionVersion)
    AC_CHECK_LIB(ICE, IceOpenConnection)
    AC_CHECK_LIB(SM, SmcOpenConnection)
    ##AC_CHECK_LIB(Xt, XtMalloc)
    AC_CHECK_LIB(Xmu, XmuLookupStandardColormap)
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

AC_ARG_WITH(fltk, dnl
            [  --with-fltk=DIR      Path to FLTK v1.1],
            FLTK="$withval",)

CPPFLAGS="$CPPFLAGS -I$FLTK/include"
LDFLAGS="$LDFLAGS -L$FLTK/lib"

AC_LANG_PUSH(C++)

AC_CHECK_HEADER(FL/Fl.h)

AC_CHECK_LIB(fltk, fl_numericsort)

FLUID="fluid"
AC_SUBST(FLUID)

AC_LANG_POP


AC_CONFIG_FILES([
Makefile
src/Makefile
])

AC_OUTPUT
