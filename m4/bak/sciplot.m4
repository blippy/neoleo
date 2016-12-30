dnl
dnl
dnl ICE_FIND_SciPlot
dnl
dnl Adapted from a macro by Andreas Zeller.
dnl
AC_DEFUN(ICE_FIND_SciPlot,
[
AC_REQUIRE([AC_PATH_XTRA])
sciplot_includes=
sciplot_libraries=
AC_ARG_WITH(SciPlot,
[  --without-SciPlot            do not use SciPlot widgets])
dnl Treat --without-SciPlot like
dnl --without-SciPlot-includes --without-SciPlot-libraries.
if test "$with_SciPlot" = "no"
then
sciplot_includes=none
sciplot_libraries=none
fi
AC_ARG_WITH(SciPlot-includes,
[  --with-SciPlot-includes=DIR  SciPlot include files are in DIR],
sciplot_includes="$withval")
AC_ARG_WITH(SciPlot-libraries,
[  --with-SciPlot-libraries=DIR SciPlot libraries are in DIR],
sciplot_libraries="$withval")
AC_MSG_CHECKING(for SciPlot)
#
#
# Search the include files.
#
if test "$sciplot_includes" = ""; then
AC_CACHE_VAL(ice_cv_sciplot_includes,
[
ice_sciplot_save_LIBS="$LIBS"
ice_sciplot_save_CFLAGS="$CFLAGS"
ice_sciplot_save_CPPFLAGS="$CPPFLAGS"
ice_sciplot_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_COMPILE([#include <SciPlot/SciPlot.h>],[int a;],
[
# SciPlot/SciPlot.h is in the standard search path.
ice_cv_sciplot_includes=
],
[
# SciPlot/SciPlot.h is not in the standard search path.
# Locate it and put its directory in `sciplot_includes'
#
# /usr/include/Motif* are used on HP-UX (Motif).
# /usr/include/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
ice_cv_sciplot_includes="none"
for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
           /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
           /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
           /usr/dt/include /usr/openwin/include \
           /usr/dt/*/include /opt/*/include /usr/include/Motif* \
	   /home/SciPlot/include /usr/SciPlot/include /opt/SciPlot/include \
	   /home/SciPlot*/include /usr/SciPlot*/include /opt/SciPlot*/include \
           "${prefix}"/*/include /usr/*/include /usr/local/*/include \
           "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
if test -f "$dir/SciPlot/SciPlot.h"; then
ice_cv_sciplot_includes="$dir"
break
fi
done
])
#
LIBS="$ice_sciplot_save_LIBS"
CFLAGS="$ice_sciplot_save_CFLAGS"
CPPFLAGS="$ice_sciplot_save_CPPFLAGS"
LDFLAGS="$ice_sciplot_save_LDFLAGS"
])
sciplot_includes="$ice_cv_sciplot_includes"
fi
#
#
# Now for the libraries.
#
if test "$sciplot_libraries" = ""; then
AC_CACHE_VAL(ice_cv_sciplot_libraries,
[
ice_sciplot_save_LIBS="$LIBS"
ice_sciplot_save_CFLAGS="$CFLAGS"
ice_sciplot_save_CPPFLAGS="$CPPFLAGS"
ice_sciplot_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lSciPlot -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_LINK([#include <SciPlot/SciPlot.h>],[SciPlotQuickUpdate();],
[
# libXm.a is in the standard search path.
ice_cv_sciplot_libraries=
],
[
# libXm.a is not in the standard search path.
# Locate it and put its directory in `sciplot_libraries'
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/lesstif is used on Linux (Lesstif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
ice_cv_sciplot_libraries="none"
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
           /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
           /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
           /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
           /usr/lesstif*/lib /usr/lib/Lesstif* \
	   /home/SciPlot/lib /usr/SciPlot/lib /opt/SciPlot/lib \
	   /home/SciPlot*/lib /usr/SciPlot*/lib /opt/SciPlot*/lib \
           "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
           "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libSciPlot.* 2> /dev/null`" != ""; then
ice_cv_sciplot_libraries="$dir"
break
fi
done
])
#
LIBS="$ice_sciplot_save_LIBS"
CFLAGS="$ice_sciplot_save_CFLAGS"
CPPFLAGS="$ice_sciplot_save_CPPFLAGS"
LDFLAGS="$ice_sciplot_save_LDFLAGS"
])
#
sciplot_libraries="$ice_cv_sciplot_libraries"
fi
#
#
# Provide an easier way to link
#
if test "$sciplot_includes" != "" && test "$sciplot_includes" != "$x_includes" && test "$sciplot_includes" != "none"; then
	link_sciplot="-L$sciplot_libraries -lSciPlot"
	include_sciplot="-I$sciplot_includes"
	AC_DEFINE(HAVE_SciPlot_H)
fi
#
AC_SUBST(include_sciplot)
AC_SUBST(link_sciplot)
#
#
#
sciplot_libraries_result="$sciplot_libraries"
sciplot_includes_result="$sciplot_includes"
test "$sciplot_libraries_result" = "" && sciplot_libraries_result="in default path"
test "$sciplot_includes_result" = "" && sciplot_includes_result="in default path"
test "$sciplot_libraries_result" = "none" && sciplot_libraries_result="(none)"
test "$sciplot_includes_result" = "none" && sciplot_includes_result="(none)"
AC_MSG_RESULT(
  [libraries $sciplot_libraries_result, headers $sciplot_includes_result])
])dnl
