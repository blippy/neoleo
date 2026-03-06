dnl
dnl
dnl AC_FIND_XBAE
dnl
dnl Adapted from a macro by Andreas Zeller.
dnl
AC_DEFUN(AC_FIND_XBAE,
[
AC_REQUIRE([AC_PATH_XTRA])
xbae_includes=
xbae_libraries=
dnl AC_ARG_WITH(Xbae,
dnl [  --without-Xbae         do not use Xbae widgets])
dnl Treat --without-Xbae like
dnl --without-Xbae-includes --without-Xbae-libraries.
if test "$with_Xbae" = "no"
then
xbae_includes=none
xbae_libraries=none
fi
AC_ARG_WITH(Xbae-includes,
[  --with-Xbae-includes=DIR     Xbae include files are in DIR],
xbae_includes="$withval")
AC_ARG_WITH(Xbae-libraries,
[  --with-Xbae-libraries=DIR    Xbae libraries are in DIR],
xbae_libraries="$withval")
AC_MSG_CHECKING(for Xbae)
#
#
# Search the include files.
#
if test "$xbae_includes" = ""; then
AC_CACHE_VAL(ice_cv_xbae_includes,
[
ice_xbae_save_LIBS="$LIBS"
ice_xbae_save_CFLAGS="$CFLAGS"
ice_xbae_save_CPPFLAGS="$CPPFLAGS"
ice_xbae_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_COMPILE([#include <Xbae/Matrix.h>],[int a;],
[
# Xbae/Matrix.h is in the standard search path.
ice_cv_xbae_includes=
],
[
# Xbae/Matrix.h is not in the standard search path.
# Locate it and put its directory in `xbae_includes'
#
# /usr/include/Motif* are used on HP-UX (Motif).
# /usr/include/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
ice_cv_xbae_includes="none"
for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
           /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
           /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
           /usr/dt/include /usr/openwin/include \
           /usr/dt/*/include /opt/*/include /usr/include/Motif* \
	   /home/Xbae/include /usr/Xbae/include /opt/Xbae/include \
	   /home/Xbae*/include /usr/Xbae*/include /opt/Xbae*/include \
           "${prefix}"/*/include /usr/*/include /usr/local/*/include \
           "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
if test -f "$dir/Xbae/Matrix.h"; then
ice_cv_xbae_includes="$dir"
break
fi
done
])
#
LIBS="$ice_xbae_save_LIBS"
CFLAGS="$ice_xbae_save_CFLAGS"
CPPFLAGS="$ice_xbae_save_CPPFLAGS"
LDFLAGS="$ice_xbae_save_LDFLAGS"
])
xbae_includes="$ice_cv_xbae_includes"
fi
#
#
# Now for the libraries.
#
if test "$xbae_libraries" = ""; then
AC_CACHE_VAL(ice_cv_xbae_libraries,
[
ice_xbae_save_LIBS="$LIBS"
ice_xbae_save_CFLAGS="$CFLAGS"
ice_xbae_save_CPPFLAGS="$CPPFLAGS"
ice_xbae_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXbae -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_LINK([#include <Xbae/Matrix.h>],[Widget w; XbaeMatrixRefresh(w);],
[
# libXm.a is in the standard search path.
ice_cv_xbae_libraries=
],
[
# libXm.a is not in the standard search path.
# Locate it and put its directory in `xbae_libraries'
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/lesstif is used on Linux (Lesstif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
ice_cv_xbae_libraries="none"
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
           /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
           /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
           /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
           /usr/lesstif*/lib /usr/lib/Lesstif* \
	   /home/Xbae/lib /usr/Xbae/lib /opt/Xbae/lib \
	   /home/Xbae*/lib /usr/Xbae*/lib /opt/Xbae*/lib \
           "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
           "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libXbae.* 2> /dev/null`" != ""; then
ice_cv_xbae_libraries="$dir"
break
fi
done
])
#
LIBS="$ice_xbae_save_LIBS"
CFLAGS="$ice_xbae_save_CFLAGS"
CPPFLAGS="$ice_xbae_save_CPPFLAGS"
LDFLAGS="$ice_xbae_save_LDFLAGS"
])
#
xbae_libraries="$ice_cv_xbae_libraries"
fi
#
# Provide an easier way to link
#
# Okay
#
# Let's start by making sure that we completely abandon everything related
# to Xbae installation if either the library or the includes have not been
# located, OR if there was a problem locating the Motif libraries, which are
# required for the use of Xbae.  The opening three conditions, if true, will
# bypass all Xbae config operations; that is to say, if any of these
# conditions is true, we call with_xbae "no", and that's the end of the
# game.
#
if test "$with_motif" = "no" ; then
	with_xbae="no"
elif test "$xbae_includes" = "none" ; then
        with_xbae="no"
elif test "$xbae_libraries" = "none"; then
        with_xbae="no"
else
#
# We now have established that we want to use Xbae. It's time to set up the
# basic environment, and do some discrete tests to set up the environment.
#
# First, let's set with_xbae to "yes" (don't know of this is really
# necessary, but we'll be conservative here).  We also send HAVE_XBAE
# to config.h and the cache file.
#
	AC_DEFINE(HAVE_XBAE)
        with_xbae="yes"
#
# Then let's see if the includes were NOT in the default path (if they were,
# we won't be needing an -I to point at the headers, because the compiler
# will find them by itself).  We've already eliminated the possibility of
# "none", so anything other than "" will definitely be a path.
#
        if test "$xbae_includes" != ""; then
                XBAE_CFLAGS="-I$xbae_includes"
        fi
#
# Now that that's out of the way, let's deal with libraries.  Here,
# we check again to see if the variable (xbae_libraries this time)
# is an empty string, but this time we have work to do whether the
# test is true or false.  We start with the case of an empty
# string, which means we want to link with Xbae, but don't need
# a path to the library.
#
        if test "$xbae_libraries" = ""; then
                XBAE_LIBS="-lXbae"
        else
                XBAE_LIBS="-L$xbae_libraries -lXbae"
        fi
#
# We now close the enclosing conditional.
#
fi
#
AC_SUBST(XBAE_CFLAGS)
AC_SUBST(XBAE_LIBS)
#
#
#
xbae_libraries_result="$xbae_libraries"
xbae_includes_result="$xbae_includes"
test "$xbae_libraries_result" = "" &&
  xbae_libraries_result="in default path"
test "$xbae_includes_result" = "" &&
  xbae_includes_result="in default path"
test "$xbae_libraries_result" = "none" &&
  xbae_libraries_result="(none)"
test "$xbae_includes_result" = "none" &&
  xbae_includes_result="(none)"
AC_MSG_RESULT(
  [libraries $xbae_libraries_result, headers $xbae_includes_result])
])dnl
