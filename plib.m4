# Configure paths for plib
# Modified by Bernie Bright <bbright@users.sourceforge.net>
# based partly on alsa.m4

dnl XX_PATH_PLIB([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND ]]])
dnl Test for plib and define PLIB_CXXFLAGS and PLIB_LDFLAGS.
dnl
AC_DEFUN([XX_PATH_PLIB],
[dnl
plib_save_CPPFLAGS="$CPPFLAGS"
plib_save_LDFLAGS="$LDFLAGS"
plib_found=yes

dnl
dnl Get the cxxflags and libraries for plib
dnl
AC_ARG_WITH(plib-prefix,[  --with-plib=PFX         Prefix where PLIB is installed (/usr)],
	plib_prefix="$withval", plib_prefix="")
AC_ARG_WITH(plib-libraries,[  --with-plib-libraries=DIR  Directory where plib libraries are installed (/usr/lib)], plib_libraries="$withval", plib_libraries="")
AC_ARG_WITH(plib-includes,[  --with-plib-includes=DIR  Directory where plib header files are installed (/usr/include)], plib_includes="$withval", plib_includes="")
AC_ARG_ENABLE(plibtest, [  --disable-plibtest       Do not try to compile and run a test PLIB program],
	, enable_plibtest=yes)

dnl Add any special include directories
AC_MSG_CHECKING(for PLIB include path)
if test "$plib_includes" != "" ; then
  PLIB_CPPFLAGS="$PLIB_CPPFLAGS -I$plib_includes"
elif test "x$plib_prefix" != "x" ; then
  PLIB_CPPFLAGS="$PLIB_CPPFLAGS -I$plib_prefix/include"
elif test "x$prefix" != "xNONE" ; then
  PLIB_CPPFLAGS="$PLIB_CPPFLAGS -I$prefix/include"
fi
AC_MSG_RESULT($PLIB_CPPFLAGS)
CPPFLAGS="$CPPFLAGS $PLIB_CPPFLAGS"

dnl add any special lib dirs
AC_MSG_CHECKING(for PLIB LDFLAGS)
if test "x$plib_libraries" != "x" ; then
  PLIB_LDFLAGS="-L$plib_libraries"
elif test "x$plib_prefix" != "x" ; then
  PLIB_LDFLAGS="-L$plib_prefix/lib"
elif test "x$prefix" != "xNONE" ; then
  PLIB_LDFLAGS="-L$prefix/lib"
fi
AC_MSG_RESULT($PLIB_LDFLAGS)
LDFLAGS="$LDFLAGS $PLIB_LDFLAGS"

min_plib_version=ifelse([$1], ,1.8.0,$1)
AC_MSG_CHECKING(for PLIB - version >= $min_plib_version)
no_plib=""
plib_min_major_version=`echo $min_plib_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
plib_min_minor_version=`echo $min_plib_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
plib_min_micro_version=`echo $min_plib_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

AC_LANG_PUSH(C++)
AC_TRY_COMPILE([
#include <plib/ul.h>
], [
# if(PLIB_MAJOR_VERSION > $plib_min_major_version)
  exit(0);
# else
#   if(PLIB_MAJOR_VERSION < $plib_min_major_version)
#      error not present
#   endif
#   if(PLIB_MINOR_VERSION > $plib_min_minor_version)
  exit(0);
#   else
#     if(PLIB_MINOR_VERSION < $plib_min_minor_version)
#      error not present
#     endif

#     if(PLIB_TINY_VERSION < $plib_min_micro_version)
#      error not present
#     endif
#   endif
# endif
exit(0);
],
  [AC_MSG_RESULT(found.)],
  [AC_MSG_RESULT(not present.)
   ifelse([$3],,[AC_MSG_ERROR(Sufficiently new version of plib not found.)])
   plib_found=no]
)
AC_LANG_POP

if test "x$plib_found" = "xyes" ; then
  ifelse([$2], , :, [$2])
fi
if test "x$plib_found" = "xno" ; then
  ifelse([$3], , :, [$3])
  CPPFLAGS="$plib_save_CPPFLAGS"
  LDFLAGS="plib_save_LDFLAGS"
  PLIB_CPPFLAGS=""
  PLIB_LDFLAGS=""
fi
])
