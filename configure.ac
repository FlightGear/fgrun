dnl (Process this file with autoconf to produce a configure script.)
AC_REVISION($Revision$)dnl
AC_PREREQ(2.54)

AC_INIT([fgrun], [0.4.9], [fredb@users.sourceforge.net])
AC_CONFIG_HEADER(src/config.h)
AC_CONFIG_SRCDIR(src/main.cxx)

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE
#AM_MAINTAINER_MODE

PACKAGE=$PACKAGE_NAME
VERSION=$PACKAGE_VERSION
AC_SUBST(PACKAGE)
AC_SUBST(VERSION)
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of this package])
AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version of this package])


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
AC_PROG_CXXCPP
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_LN_S

dnl
dnl Checks for compiler features
dnl

LIBM=
case "${target_os}" in
mingw32)
  CPPFLAGS="-DWIN32 $CPPFLAGS"
  CXXFLAGS="-mwindows -mno-cygwin $CXXFLAGS"
  LDFLAGS="-mwindows $LDFLAGS"
  LIBS="-lglu32 -lopengl32 -lcomctl32 -luuid -lwsock32 -lole32 -lwinmm -lgdi32 $LIBS"
  ;;
cygwin)
  LIBS="-lglu32 -lopengl32 -lcomctl32 -luuid -lwsock32 -lole32 -lwinmm -lgdi32 $LIBS"
  ;;
*)
  AC_CHECK_LIB(m, cos, LIBM="-lm")
  ;;
esac

LIBS="$LIBS $LIBM"

AC_C_CONST

dnl
dnl Checks for libraries
dnl

AC_PATH_XTRA

if test "x$have_x" = "xyes"; then
    CPPFLAGS="$CPPFLAGS $X_CFLAGS"
    LDFLAGS="$LDFLAGS $X_LIBS"

    AC_CHECK_LIB(X11, XCreateWindow)
    AC_CHECK_LIB(Xext, XShmCreateImage)
    AC_CHECK_LIB(Xi, XGetExtensionVersion)
    AC_CHECK_LIB(ICE, IceOpenConnection)
    AC_CHECK_LIB(SM, SmcOpenConnection)
    AC_CHECK_LIB(Xt, XtMalloc)
    AC_CHECK_LIB(Xmu, XmuLookupStandardColormap)

    AC_SEARCH_LIBS(glNewList, [ GL GLcore MesaGL ])
    if test "x$ac_cv_search_glNewList" = "x-lGLcore"; then
        dnl if GLcore found, then also check for GL
        AC_SEARCH_LIBS(glXCreateContext, GL)
    fi
fi

dnl
dnl Checks for header files.
dnl

AC_HEADER_STDC
AC_HEADER_TIME
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(fcntl.h libutil.h pty.h termios.h utmp.h util.h windows.h)

if test "x$ac_cv_header_windows_h" = "xyes"; then
  AC_DEFINE([WIN32], 1, [Define if building on a Win32 platform])
  AC_DEFINE([NOMINMAX], 1, [Define for Win32 platforms])
fi

AM_CONDITIONAL(WIN32, test "x$ac_cv_header_windows_h" = "xyes")

AC_CHECK_HEADER(zlib.h,
  AC_CHECK_LIB(z, gzopen,
    AC_DEFINE([HAVE_LIBZ], 1, [Define if you have the <zlib.h> header file.])
    LIBS="$LIBS -lz"))

XX_PATH_PLIB(1.8.0)

AC_LANG_PUSH(C++)

AC_ARG_WITH(simgear, [  --with-simgear=DIR      Path to simgear (/usr/local)],
            simgear_prefix="$withval", simgear_prefix="")

if test x$simgear_prefix != x ; then
  CPPFLAGS="-I$simgear_prefix/include $CPPFLAGS"
  LDFLAGS="-L$simgear_prefix/lib $LDFLAGS"
fi

AC_CHECK_HEADER(simgear/props/props_io.hxx)
if test "x$ac_cv_header_simgear_props_props_io_hxx" != "xyes"; then
  echo "SimGear not found.  Aborting."
  exit
fi

AC_LANG_POP

dnl
dnl Checks for typedefs, structures, and compiler characteristics.
dnl

AC_TYPE_SIZE_T
AC_TYPE_PID_T
AC_TYPE_SIGNAL

dnl
dnl Checks for library functions.
dnl

AC_CHECK_FUNCS(strdup strlcat strlcpy)

AC_FUNC_FORK
AC_CHECK_FUNCS(waitpid)

if test "x$ac_cv_header_windows_h" = "xno"; then
  AC_CHECK_FUNCS(openpty,
    AC_DEFINE([HAVE_OPENPTY],1,[Define to 1 if you have the `openpty' function.]),
    AC_CHECK_LIB(util, openpty,
      [
        AC_DEFINE([HAVE_OPENPTY],1,[Define to 1 if you have the `openpty' function.])
        LIBS="$LIBS -lutil"
      ])
  )

  AC_CHECK_FUNCS(forkpty login_tty)
  AC_CHECK_FUNCS(_getpty grantpt unlockpt)

  dnl Workaround for a Autoconf 2.4 bug:
  if test x$ac_cv_func_grantpt = xyes; then
    :
  else
    AC_CHECK_LIB(pt, grantpt)
  fi

  AC_MSG_CHECKING([for /dev/ptmx])
  if test -e /dev/ptmx
  then
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_DEV_PTMX, 1, [Define if we have /dev/ptmx])
  else
    AC_MSG_RESULT(no)
  fi

  AC_MSG_CHECKING([for /dev/ptc])
  if test -e /dev/ptc
  then
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_DEV_PTC, 1, [Define if we have /dev/ptc])
  else
    AC_MSG_RESULT(no)
  fi
fi

dnl Check for fltk
FLUID="fluid"

AC_ARG_WITH(fltk,    [  --with-fltk=DIR         Path to FLTK v1.1],
	fltk_prefix="$withval", fltk_prefix="")

if test x$fltk_prefix != x ; then
  FLUID="$fltk_prefix/bin/fluid"
  CPPFLAGS="-I$fltk_prefix/include $CPPFLAGS"
  LDFLAGS="-L$fltk_prefix/lib $LDFLAGS"
fi

AC_SUBST(FLUID)

AC_LANG_PUSH(C++)

dnl Check for FLTK header and library.
AC_CHECK_HEADER(FL/Fl.H)

dnl ----------------------------------------------------------------------
dnl check if fltk has been compiled with Xft (and add lib if required)
dnl if the check is not done before the other checks, linking will fail
dnl on platforms with Xft enabled fltk builds
dnl ----------------------------------------------------------------------

AC_CHECK_LIB(fltk,XftFontOpen,LIBS=" -lfltk -lXft $LIBS", AC_CHECK_LIB(fltk, fl_snprintf) , -lXft)
dnl save_LIBS="$LIBS"

dnl the next line could be used instead of the following chunk of code
dnl AC_CHECK_LIB(fltk_gl,gl_start)

AC_CHECK_LIB(pthread,pthread_create)

save_LIBS="$LIBS"
LIBS="-lfltk_gl $LIBS"
AC_MSG_CHECKING(for gl_start in -lfltk_gl)
AC_TRY_LINK([
#include <FL/gl.h>
], gl_start(),
  AC_MSG_RESULT(yes),
  LIBS="$save_LIBS"
  AC_MSG_RESULT(no)
)

AC_LANG_POP

AC_CONFIG_FILES([
Makefile
src/Makefile
])

AC_OUTPUT
