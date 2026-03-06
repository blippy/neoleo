dnl
dnl
dnl AC_FIND_XLT : find libXlt, and provide variables
dnl	to easily use them in a Makefile.
dnl
dnl Adapted from a macro by Andreas Zeller.
dnl
dnl The variables provided are :
dnl	link_xlt		(e.g. -L/usr/lesstif/lib -lXm)
dnl	include_xlt		(e.g. -I/usr/lesstif/lib)
dnl	xlt_libraries		(e.g. /usr/lesstif/lib)
dnl	xlt_includes		(e.g. /usr/lesstif/include)
dnl
dnl The link_xlt and include_xlt variables should be fit to put on
dnl your application's link line in your Makefile.
dnl
AC_DEFUN([AC_FIND_XLT],
[
AC_REQUIRE([AC_FIND_MOTIF])
dnl AC_REQUIRE([AC_FIND_XPM])
xlt_includes=
xlt_libraries=
AC_ARG_WITH(xlt,
[  --without-xlt         do not use Xlt widgets])
dnl Treat --without-xlt like
dnl --without-xlt-includes --without-xlt-libraries.
if test "$with_xlt" = "no"
then
    xlt_includes=no
    xlt_libraries=no
fi
AC_ARG_WITH(xlt-includes,
    [  --with-xlt-includes=DIR    Xlt include files are in DIR], xlt_includes="$withval")
AC_ARG_WITH(xlt-libraries,
    [  --with-xlt-libraries=DIR   Xlt libraries are in DIR], xlt_libraries="$withval")
if test "$xlt_includes" = "no" && test "$xlt_libraries" = "no"
then
    with_xlt="no"
fi

AC_MSG_CHECKING([for Xlt])
if test "$with_xlt" != "no"
then
    #
    #
    # Search the include files.
    #
    if test "$xlt_includes" = ""
    then
	AC_CACHE_VAL(ac_cv_xlt_includes,
	[
	ac_xlt_save_CFLAGS="$CFLAGS"
	ac_xlt_save_CPPFLAGS="$CPPFLAGS"
	#
	CFLAGS="$MOTIF_CFLAGS $X_CFLAGS $XPM_CFLAGS $CFLAGS"
	CPPFLAGS="$MOTIF_CFLAGS $X_CFLAGS $XPM_CFLAGS $CPPFLAGS"
	#
	AC_TRY_COMPILE([#include <Xlt/Xlt.h>],[int a;],
	[
	# Xlt/Xlt.h is in the standard search path.
	ac_cv_xlt_includes=
	],
	[
	# Xlt/Xlt.h is not in the standard search path.
	# Locate it and put its directory in `xlt_includes'
	#
	# Other directories are just guesses.
	for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
		    /usr/include/Motif2.1 /usr/include/Motif2.0 /usr/include/Motif1.2 \
		   /usr/include/X11R6 /usr/include/X11R5 \
		   /usr/dt/include /usr/openwin/include \
		   /usr/dt/*/include /opt/*/include /usr/include/Xlt* \
		   "${prefix}"/*/include /usr/*/include /usr/local/*/include \
		   "${prefix}"/include/* /usr/include/* /usr/local/include/* \
		   "${HOME}"/include
	do
	    if test -f "$dir/Xlt/Xlt.h"
	    then
		ac_cv_xlt_includes="$dir"
		break
	    fi
	done
	])
	#
	CFLAGS="$ac_xlt_save_CFLAGS"
	CPPFLAGS="$ac_xlt_save_CPPFLAGS"
	])
	xlt_includes="$ac_cv_xlt_includes"
    fi

    if test -z "$xlt_includes"
    then
	xlt_includes_result="default path"
	XLT_CFLAGS=""
    else
	if test "$xlt_includes" = "no"
	then
	    xlt_includes_result="told not to use them"
	    XLT_CFLAGS=""
	else
	    xlt_includes_result="$xlt_includes"
	    XLT_CFLAGS="-I$xlt_includes"
	fi
    fi
    #
    #
    # Now for the libraries.
    #
    if test "$xlt_libraries" = ""
    then
	AC_CACHE_VAL(ac_cv_xlt_libraries,
	[
	ac_xlt_save_LIBS="$LIBS"
	ac_xlt_save_CFLAGS="$CFLAGS"
	ac_xlt_save_CPPFLAGS="$CPPFLAGS"
	#
	LIBS="-lXlt -lm $MOTIF_LIBS $X_LIBS $X_PRE_LIBS -lXt $XPM_LIBS -lX11 $X_EXTRA_LIBS $LIBS"
	CFLAGS="$XLT_CFLAGS $MOTIF_CFLAGS $X_CFLAGS $XPM_CFLAGS $CFLAGS"
	CPPFLAGS="$XLT_CFLAGS $MOTIF_CFLAGS $X_CFLAGS $XPM_CFLAGS $CPPFLAGS"
	#
	AC_TRY_LINK([#include <Xlt/Xlt.h>],[Widget w; XltRedirectStdErr(w);],
	[
	# libXlt.a is in the standard search path.
	ac_cv_xlt_libraries=
	],
	[
	# libXlt.a is not in the standard search path.
	# Locate it and put its directory in `xlt_libraries'
	#
	# Other directories are just guesses.
	for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
		   /usr/lib/Xlt2.0 /usr/lib/Xlt1.2 /usr/lib/Xlt1.1 \
		   /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11 \
		   /usr/dt/lib /usr/openwin/lib \
		   /usr/dt/*/lib /opt/*/lib /usr/lib/Xlt* \
		   /usr/lesstif*/lib /usr/lib/Lesstif* \
		   "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
		   "${prefix}"/lib/* /usr/lib/* /usr/local/lib/* \
		   "${HOME}"/lib
	do
	    for ext in "sl" "so" "a" "lib" ; do
		if test -d "$dir" && test -f "$dir/libXlt.$ext"; then
		    ac_cv_xlt_libraries="$dir"
		    break 2
		fi
	    done
	done
	])
	#
	LIBS="$ac_xlt_save_LIBS"
	CFLAGS="$ac_xlt_save_CFLAGS"
	CPPFLAGS="$ac_xlt_save_CPPFLAGS"
	])
	#
	xlt_libraries="$ac_cv_xlt_libraries"
    fi
    if test -z "$xlt_libraries"
    then
	xlt_libraries_result="default path"
	XLT_LIBS="-lXlt"
    else
	if test "$xlt_libraries" = "no"
	then
	    xlt_libraries_result="told not to use it"
	    XLT_LIBS=""
	else
	    xlt_libraries_result="$xlt_libraries"
	    XLT_LIBS="-L$xlt_libraries -lXlt"
	fi
    fi
#
# Make sure, whatever we found out, we can link.
#
    ac_xlt_save_LIBS="$LIBS"
    ac_xlt_save_CFLAGS="$CFLAGS"
    ac_xlt_save_CPPFLAGS="$CPPFLAGS"
    #
    LIBS="$XLT_LIBS -lm $MOTIF_LIBS $X_LIBS $X_PRE_LIBS -lXt $XPM_LIBS -lX11 $X_EXTRA_LIBS $LIBS"
    CFLAGS="$XLT_CFLAGS $MOTIF_CFLAGS $X_CFLAGS $XPM_CFLAGS $CFLAGS"
    CPPFLAGS="$XLT_CFLAGS $MOTIF_CFLAGS $X_CFLAGS $XPM_CFLAGS $CPPFLAGS"

    AC_TRY_LINK([#include <Xlt/Xlt.h>],[Widget w; XltRedirectStdErr(w);],
	[
	#
	# link passed
	#
	# Just to be sure there is something here...
	#
	true
	#
	AC_DEFINE(HAVE_XLT, 1, Define if the Xlt library is available)
	],
	[
	#
	# link failed
	#
	xlt_libraries_result="test link failed"
	xlt_includes_result="test link failed"
	with_xlt="no"
	XLT_CFLAGS=""
	XLT_LIBS=""
	]) dnl AC_TRY_LINK

    LIBS="$ac_xlt_save_LIBS"
    CFLAGS="$ac_xlt_save_CFLAGS"
    CPPFLAGS="$ac_xlt_save_CPPFLAGS"
else
    xlt_libraries_result="told not to use it"
    xlt_includes_result="told not to use them"
    XLT_CFLAGS=""
    XLT_LIBS=""
fi
AC_MSG_RESULT([libraries $xlt_libraries_result, headers $xlt_includes_result])
AC_SUBST(XLT_CFLAGS)
AC_SUBST(XLT_LIBS)
])dnl AC_DEFN
