dnl aclocal.m4 generated automatically by aclocal 1.4

dnl Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 1

dnl Usage:
dnl AM_INIT_AUTOMAKE(package,version, [no-define])

AC_DEFUN(AM_INIT_AUTOMAKE,
[AC_REQUIRE([AC_PROG_INSTALL])
PACKAGE=[$1]
AC_SUBST(PACKAGE)
VERSION=[$2]
AC_SUBST(VERSION)
dnl test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" && test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi
ifelse([$3],,
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of package])
AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version number of package]))
AC_REQUIRE([AM_SANITY_CHECK])
AC_REQUIRE([AC_ARG_PROGRAM])
dnl FIXME This is truly gross.
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)
AC_REQUIRE([AC_PROG_MAKE_SET])])

#
# Check to make sure that the build environment is sane.
#

AC_DEFUN(AM_SANITY_CHECK,
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "[$]*" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   if test "[$]*" != "X $srcdir/configure conftestfile" \
      && test "[$]*" != "X conftestfile $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

dnl AM_MISSING_PROG(NAME, PROGRAM, DIRECTORY)
dnl The program must properly implement --version.
AC_DEFUN(AM_MISSING_PROG,
[AC_MSG_CHECKING(for working $2)
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
# Redirect stdin to placate older versions of autoconf.  Sigh.
if ($2 --version) < /dev/null > /dev/null 2>&1; then
   $1=$2
   AC_MSG_RESULT(found)
else
   $1="$3/missing $2"
   AC_MSG_RESULT(missing)
fi
AC_SUBST($1)])

# Add --enable-maintainer-mode option to configure.
# From Jim Meyering

# serial 1

AC_DEFUN(AM_MAINTAINER_MODE,
[AC_MSG_CHECKING([whether to enable maintainer-specific portions of Makefiles])
  dnl maintainer-mode is disabled by default
  AC_ARG_ENABLE(maintainer-mode,
[  --enable-maintainer-mode enable make rules and dependencies not useful
                          (and sometimes confusing) to the casual installer],
      USE_MAINTAINER_MODE=$enableval,
      USE_MAINTAINER_MODE=no)
  AC_MSG_RESULT($USE_MAINTAINER_MODE)
  AM_CONDITIONAL(MAINTAINER_MODE, test $USE_MAINTAINER_MODE = yes)
  MAINT=$MAINTAINER_MODE_TRUE
  AC_SUBST(MAINT)dnl
]
)

# Define a conditional.

AC_DEFUN(AM_CONDITIONAL,
[AC_SUBST($1_TRUE)
AC_SUBST($1_FALSE)
if $2; then
  $1_TRUE=
  $1_FALSE='#'
else
  $1_TRUE='#'
  $1_FALSE=
fi])

# Like AC_CONFIG_HEADER, but automatically create stamp file.

AC_DEFUN(AM_CONFIG_HEADER,
[AC_PREREQ([2.12])
AC_CONFIG_HEADER([$1])
dnl When config.status generates a header, we must update the stamp-h file.
dnl This file resides in the same directory as the config header
dnl that is generated.  We must strip everything past the first ":",
dnl and everything past the last "/".
AC_OUTPUT_COMMANDS(changequote(<<,>>)dnl
ifelse(patsubst(<<$1>>, <<[^ ]>>, <<>>), <<>>,
<<test -z "<<$>>CONFIG_HEADERS" || echo timestamp > patsubst(<<$1>>, <<^\([^:]*/\)?.*>>, <<\1>>)stamp-h<<>>dnl>>,
<<am_indx=1
for am_file in <<$1>>; do
  case " <<$>>CONFIG_HEADERS " in
  *" <<$>>am_file "*<<)>>
    echo timestamp > `echo <<$>>am_file | sed -e 's%:.*%%' -e 's%[^/]*$%%'`stamp-h$am_indx
    ;;
  esac
  am_indx=`expr "<<$>>am_indx" + 1`
done<<>>dnl>>)
changequote([,]))])


# serial 1

AC_DEFUN(AM_WITH_DMALLOC,
[AC_MSG_CHECKING(if malloc debugging is wanted)
AC_ARG_WITH(dmalloc,
[  --with-dmalloc          use dmalloc, as in
                          ftp://ftp.letters.com/src/dmalloc/dmalloc.tar.gz],
[if test "$withval" = yes; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(WITH_DMALLOC,1,
            [Define if using the dmalloc debugging malloc package])
  LIBS="$LIBS -ldmalloc"
  LDFLAGS="$LDFLAGS -g"
else
  AC_MSG_RESULT(no)
fi], [AC_MSG_RESULT(no)])
])


# serial 1

AC_DEFUN(AM_C_PROTOTYPES,
[AC_REQUIRE([AM_PROG_CC_STDC])
AC_REQUIRE([AC_PROG_CPP])
AC_MSG_CHECKING([for function prototypes])
if test "$am_cv_prog_cc_stdc" != no; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(PROTOTYPES,1,[Define if compiler has function prototypes])
  U= ANSI2KNR=
else
  AC_MSG_RESULT(no)
  U=_ ANSI2KNR=./ansi2knr
  # Ensure some checks needed by ansi2knr itself.
  AC_HEADER_STDC
  AC_CHECK_HEADERS(string.h)
fi
AC_SUBST(U)dnl
AC_SUBST(ANSI2KNR)dnl
])


# serial 1

# @defmac AC_PROG_CC_STDC
# @maindex PROG_CC_STDC
# @ovindex CC
# If the C compiler in not in ANSI C mode by default, try to add an option
# to output variable @code{CC} to make it so.  This macro tries various
# options that select ANSI C on some system or another.  It considers the
# compiler to be in ANSI C mode if it handles function prototypes correctly.
#
# If you use this macro, you should check after calling it whether the C
# compiler has been set to accept ANSI C; if not, the shell variable
# @code{am_cv_prog_cc_stdc} is set to @samp{no}.  If you wrote your source
# code in ANSI C, you can make an un-ANSIfied copy of it by using the
# program @code{ansi2knr}, which comes with Ghostscript.
# @end defmac

AC_DEFUN(AM_PROG_CC_STDC,
[AC_REQUIRE([AC_PROG_CC])
AC_BEFORE([$0], [AC_C_INLINE])
AC_BEFORE([$0], [AC_C_CONST])
dnl Force this before AC_PROG_CPP.  Some cpp's, eg on HPUX, require
dnl a magic option to avoid problems with ANSI preprocessor commands
dnl like #elif.
dnl FIXME: can't do this because then AC_AIX won't work due to a
dnl circular dependency.
dnl AC_BEFORE([$0], [AC_PROG_CPP])
AC_MSG_CHECKING(for ${CC-cc} option to accept ANSI C)
AC_CACHE_VAL(am_cv_prog_cc_stdc,
[am_cv_prog_cc_stdc=no
ac_save_CC="$CC"
# Don't try gcc -ansi; that turns off useful extensions and
# breaks some systems' header files.
# AIX			-qlanglvl=ansi
# Ultrix and OSF/1	-std1
# HP-UX			-Aa -D_HPUX_SOURCE
# SVR4			-Xc -D__EXTENSIONS__
for ac_arg in "" -qlanglvl=ansi -std1 "-Aa -D_HPUX_SOURCE" "-Xc -D__EXTENSIONS__"
do
  CC="$ac_save_CC $ac_arg"
  AC_TRY_COMPILE(
[#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
/* Most of the following tests are stolen from RCS 5.7's src/conf.sh.  */
struct buf { int x; };
FILE * (*rcsopen) (struct buf *, struct stat *, int);
static char *e (p, i)
     char **p;
     int i;
{
  return p[i];
}
static char *f (char * (*g) (char **, int), char **p, ...)
{
  char *s;
  va_list v;
  va_start (v,p);
  s = g (p, va_arg (v,int));
  va_end (v);
  return s;
}
int test (int i, double x);
struct s1 {int (*f) (int a);};
struct s2 {int (*f) (double a);};
int pairnames (int, char **, FILE *(*)(struct buf *, struct stat *, int), int, int);
int argc;
char **argv;
], [
return f (e, argv, 0) != argv[0]  ||  f (e, argv, 1) != argv[1];
],
[am_cv_prog_cc_stdc="$ac_arg"; break])
done
CC="$ac_save_CC"
])
if test -z "$am_cv_prog_cc_stdc"; then
  AC_MSG_RESULT([none needed])
else
  AC_MSG_RESULT($am_cv_prog_cc_stdc)
fi
case "x$am_cv_prog_cc_stdc" in
  x|xno) ;;
  *) CC="$CC $am_cv_prog_cc_stdc" ;;
esac
])

dnl
dnl
dnl AC_FIND_MOTIF : find OSF/Motif or LessTif, and provide variables
dnl	to easily use them in a Makefile.
dnl
dnl Adapted from a macro by Andreas Zeller.
dnl
dnl The variables provided are :
dnl	link_motif		(e.g. -L/usr/lesstif/lib -lXm)
dnl	include_motif		(e.g. -I/usr/lesstif/lib)
dnl	motif_libraries		(e.g. /usr/lesstif/lib)
dnl	motif_includes		(e.g. /usr/lesstif/include)
dnl
dnl The link_motif and include_motif variables should be fit to put on
dnl your application's link line in your Makefile.
dnl
dnl Oleo CVS Id: motif.m4,v 1.9 1999/04/09 11:46:49 danny
dnl LessTif CVS $Id: ac_find_motif.m4,v 1.5 1999/09/12 11:13:53 rwscott Exp $
dnl
AC_DEFUN(AC_FIND_MOTIF,
[

AC_REQUIRE([AC_PATH_XTRA])

motif_includes=
motif_libraries=

dnl AC_ARG_WITH(motif,
dnl [  --without-motif         do not use Motif widgets])
dnl Treat --without-motif like
dnl --without-motif-includes --without-motif-libraries.
dnl if test "$with_motif" = "no"
dnl then
dnl   motif_includes=none
dnl   motif_libraries=none
dnl fi

AC_ARG_WITH(motif-includes,
[  --with-motif-includes=DIR    Motif include files are in DIR],
motif_includes="$withval")

AC_ARG_WITH(motif-libraries,
[  --with-motif-libraries=DIR   Motif libraries are in DIR],
motif_libraries="$withval")

AC_MSG_CHECKING(for Motif)

#
#
# Search the include files.
#
if test "$motif_includes" = ""; then
AC_CACHE_VAL(ac_cv_motif_includes,
[
ac_motif_save_LIBS="$LIBS"
ac_motif_save_INCLUDES="$INCLUDES"
ac_motif_save_CPPFLAGS="$CPPFLAGS"
ac_motif_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
INCLUDES="$X_CFLAGS $INCLUDES"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
ac_cv_motif_includes="none"
AC_TRY_COMPILE([#include <Xm/Xm.h>],[int a;],
[
# Xm/Xm.h is in the standard search path.
ac_cv_motif_includes=
],
[
# Xm/Xm.h is not in the standard search path.
# Locate it and put its directory in `motif_includes'
#
# /usr/include/Motif* are used on HP-UX (Motif).
# /usr/include/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
           /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
           /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
           /usr/dt/include /usr/openwin/include \
           /usr/dt/*/include /opt/*/include /usr/include/Motif* \
           "${prefix}"/*/include /usr/*/include /usr/local/*/include \
           "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
if test -f "$dir/Xm/Xm.h"; then
ac_cv_motif_includes="$dir"
break
fi
done
])
#
LIBS="$ac_motif_save_LIBS"
INCLUDES="$ac_motif_save_INCLUDES"
CPPFLAGS="$ac_motif_save_CPPFLAGS"
LDFLAGS="$ac_motif_save_LDFLAGS"
])
motif_includes="$ac_cv_motif_includes"
fi
#
#
# Now for the libraries.
#
if test "$motif_libraries" = ""; then
AC_CACHE_VAL(ac_cv_motif_libraries,
[
ac_motif_save_LIBS="$LIBS"
ac_motif_save_INCLUDES="$INCLUDES"
ac_motif_save_CPPFLAGS="$CPPFLAGS"
ac_motif_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
INCLUDES="$X_CFLAGS $INCLUDES"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
ac_cv_motif_libraries="none"
AC_TRY_LINK([#include <Xm/Xm.h>],[XtToolkitInitialize();],
[
# libXm.a is in the standard search path.
ac_cv_motif_libraries=
],
[
# libXm.a is not in the standard search path.
# Locate it and put its directory in `motif_libraries'
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/lesstif is used on Linux (Lesstif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
           /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
           /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
           /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
           /usr/lesstif*/lib /usr/lib/Lesstif* \
           "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
           "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libXm.* 2> /dev/null`" != ""; then
ac_cv_motif_libraries="$dir"
break
fi
done
])
#
LIBS="$ac_motif_save_LIBS"
INCLUDES="$ac_motif_save_INCLUDES"
CPPFLAGS="$ac_motif_save_CPPFLAGS"
LDFLAGS="$ac_motif_save_LDFLAGS"
])
#
motif_libraries="$ac_cv_motif_libraries"
fi
#
# Provide an easier way to link
#
if test "$motif_includes" = "none" -o "$motif_libraries" = "none"; then
        with_motif="no"
else
        with_motif="yes"
fi

if test "$with_motif" != "no"; then
        if test "$motif_libraries" = ""; then
                link_motif="-lXm"
                MOTIF_LIBS="-lXm"
        else
                link_motif="-L$motif_libraries -lXm"
                MOTIF_LIBS="-L$motif_libraries -lXm"
        fi
        if test "$motif_includes" != ""; then
                include_motif="-I$motif_includes"
                MOTIF_CFLAGS="-I$motif_includes"
        fi
	AC_DEFINE(HAVE_MOTIF)
else
        with_motif="no"
fi
#
AC_SUBST(link_motif)
AC_SUBST(include_motif)
AC_SUBST(MOTIF_CFLAGS)
AC_SUBST(MOTIF_LIBS)
#
#
#
motif_libraries_result="$motif_libraries"
motif_includes_result="$motif_includes"
test "$motif_libraries_result" = "" && motif_libraries_result="in default path"
test "$motif_includes_result" = "" && motif_includes_result="in default path"
test "$motif_libraries_result" = "none" && motif_libraries_result="(none)"
test "$motif_includes_result" = "none" && motif_includes_result="(none)"
AC_MSG_RESULT(
  [libraries $motif_libraries_result, headers $motif_includes_result])
])dnl

dnl
dnl
dnl AC_FIND_XBAE : find libXbae, and provide variables
dnl	to easily use them in a Makefile.
dnl
dnl Adapted from a macro by Andreas Zeller.
dnl
dnl The variables provided are :
dnl	link_xbae		(e.g. -L/usr/lesstif/lib -lXm)
dnl	include_xbae		(e.g. -I/usr/lesstif/lib)
dnl	xbae_libraries		(e.g. /usr/lesstif/lib)
dnl	xbae_includes		(e.g. /usr/lesstif/include)
dnl
dnl The link_xbae and include_xbae variables should be fit to put on
dnl your application's link line in your Makefile.
dnl
AC_DEFUN(AC_FIND_XBAE,
[
AC_REQUIRE([AC_FIND_MOTIF])
xbae_includes=
xbae_libraries=
AC_ARG_WITH(xbae,
[  --without-xbae         do not use Xbae widgets])
dnl Treat --without-xbae like
dnl --without-xbae-includes --without-xbae-libraries.
if test "$with_xbae" = "no"
then
    xbae_includes=no
    xbae_libraries=no
fi
AC_ARG_WITH(xbae-includes,
    [  --with-xbae-includes=DIR    Xbae include files are in DIR], xbae_includes="$withval")
AC_ARG_WITH(xbae-libraries,
    [  --with-xbae-libraries=DIR   Xbae libraries are in DIR], xbae_libraries="$withval")
if test "$xbae_includes" = "no" && test "$xbae_libraries" = "no"
then
    with_xbae="no"
fi

AC_MSG_CHECKING([for Xbae])
if test "$with_xbae" != "no"
then
    #
    #
    # Search the include files.
    #
    if test "$xbae_includes" = ""
    then
	AC_CACHE_VAL(ac_cv_xbae_includes,
	[
	ac_xbae_save_CFLAGS="$CFLAGS"
	ac_xbae_save_CPPFLAGS="$CPPFLAGS"
	#
	CFLAGS="$MOTIF_CFLAGS $X_CFLAGS $CFLAGS"
	CPPFLAGS="$MOTIF_CFLAGS $X_CFLAGS $CPPFLAGS"
	#
	AC_TRY_COMPILE([#include <Xbae/Matrix.h>],[int a;],
	[
	# Xbae/Matrix.h is in the standard search path.
	ac_cv_xbae_includes=
	],
	[
	# Xbae/Matrix.h is not in the standard search path.
	# Locate it and put its directory in `xbae_includes'
	#
	# Other directories are just guesses.
	for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
		   /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
		   /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
		   /usr/dt/include /usr/openwin/include \
		   /usr/dt/*/include /opt/*/include /usr/include/Xbae* \
		   "${prefix}"/*/include /usr/*/include /usr/local/*/include \
		   "${prefix}"/include/* /usr/include/* /usr/local/include/*
	do
	    if test -f "$dir/Xbae/Matrix.h"
	    then
		ac_cv_xbae_includes="$dir"
		break
	    fi
	done
	])
	#
	CFLAGS="$ac_xbae_save_CFLAGS"
	CPPFLAGS="$ac_xbae_save_CPPFLAGS"
	])
	xbae_includes="$ac_cv_xbae_includes"
    fi

    if test -z "$xbae_includes"
    then
	xbae_includes_result="default path"
	XBAE_CFLAGS=""
    else
	if test "$xbae_includes" = "no"
	then
	    xbae_includes_result="told not to use them"
	    XBAE_CFLAGS=""
	else
	    xbae_includes_result="$xbae_includes"
	    XBAE_CFLAGS="-I$xbae_includes"
	fi
    fi
    #
    #
    # Now for the libraries.
    #
    if test "$xbae_libraries" = ""
    then
	AC_CACHE_VAL(ac_cv_xbae_libraries,
	[
	ac_xbae_save_LIBS="$LIBS"
	ac_xbae_save_CFLAGS="$CFLAGS"
	ac_xbae_save_CPPFLAGS="$CPPFLAGS"
	#
	LIBS="-lXbae -lm $MOTIF_LIBS $X_LIBS $X_PRE_LIBS -lXt -lX11 $X_EXTRA_LIBS $LIBS"
	CFLAGS="$XBAE_CFLAGS $MOTIF_CFLAGS $X_CFLAGS $CFLAGS"
	CPPFLAGS="$XBAE_CFLAGS $MOTIF_CFLAGS $X_CFLAGS $CPPFLAGS"
	#
	AC_TRY_LINK([#include <Xbae/Matrix.h>],[Widget w; XbaeMatrixDeselectAll(w);],
	[
	# libXbae.a is in the standard search path.
	ac_cv_xbae_libraries=
	],
	[
	# libXbae.a is not in the standard search path.
	# Locate it and put its directory in `xbae_libraries'
	#
	# Other directories are just guesses.
	for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
		   /usr/lib/Xbae \
		   /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
		   /usr/dt/lib /usr/openwin/lib \
		   /usr/dt/*/lib /opt/*/lib /usr/lib/Xbae* \
		   /usr/lesstif*/lib /usr/lib/Lesstif* \
		   "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
		   "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
	    for ext in "sl" "so" "a"; do
		if test -d "$dir" && test -f "$dir/libXbae.$ext"; then
		    ac_cv_xbae_libraries="$dir"
		    break 2
		fi
	    done
	done
	])
	#
	LIBS="$ac_xbae_save_LIBS"
	CFLAGS="$ac_xbae_save_CFLAGS"
	CPPFLAGS="$ac_xbae_save_CPPFLAGS"
	])
	#
	xbae_libraries="$ac_cv_xbae_libraries"
    fi
    if test -z "$xbae_libraries"
    then
	xbae_libraries_result="default path"
	XBAE_LIBS="-lXbae"
    else
	if test "$xbae_libraries" = "no"
	then
	    xbae_libraries_result="told not to use it"
	    XBAE_LIBS=""
	else
	    xbae_libraries_result="$xbae_libraries"
	    XBAE_LIBS="-L$xbae_libraries -lXbae"
	fi
    fi
#
# Make sure, whatever we found out, we can link.
#
    ac_xbae_save_LIBS="$LIBS"
    ac_xbae_save_CFLAGS="$CFLAGS"
    ac_xbae_save_CPPFLAGS="$CPPFLAGS"
    #
    LIBS="$XBAE_LIBS -lm $MOTIF_LIBS $X_LIBS $X_PRE_LIBS -lXt -lX11 $X_EXTRA_LIBS $LIBS"
    CFLAGS="$XBAE_CFLAGS $MOTIF_CFLAGS $X_CFLAGS $CFLAGS"
    CPPFLAGS="$XBAE_CFLAGS $MOTIF_CFLAGS $X_CFLAGS $CPPFLAGS"

    AC_TRY_LINK([#include <Xbae/Matrix.h>],[Widget w; XbaeMatrixDeselectAll(w);],
	[
	#
	# link passed
	#
	AC_DEFINE(HAVE_XBAE)
	],
	[
	#
	# link failed
	#
	xbae_libraries_result="test link failed"
	xbae_includes_result="test link failed"
	with_xbae="no"
	XBAE_CFLAGS=""
	XBAE_LIBS=""
	]) dnl AC_TRY_LINK

    LIBS="$ac_xbae_save_LIBS"
    CFLAGS="$ac_xbae_save_CFLAGS"
    CPPFLAGS="$ac_xbae_save_CPPFLAGS"
else
    xbae_libraries_result="told not to use it"
    xbae_includes_result="told not to use them"
    XBAE_CFLAGS=""
    XBAE_LIBS=""
fi
AC_MSG_RESULT([libraries $xbae_libraries_result, headers $xbae_includes_result])
AC_SUBST(XBAE_CFLAGS)
AC_SUBST(XBAE_LIBS)
])dnl AC_DEFN

dnl
dnl
dnl ICE_FIND_XmHTML
dnl
dnl Adapted from a macro by Andreas Zeller.
dnl
AC_DEFUN(ICE_FIND_XmHTML,
[
AC_REQUIRE([AC_PATH_XTRA])
xmhtml_includes=
xmhtml_libraries=
AC_ARG_WITH(XmHTML,
[  --without-XmHTML             do not use XmHTML widgets])
dnl Treat --without-XmHTML like
dnl --without-XmHTML-includes --without-XmHTML-libraries.
if test "$with_XmHTML" = "no"
then
xmhtml_includes=none
xmhtml_libraries=none
fi
AC_ARG_WITH(xmhtml-includes,
[  --with-xmhtml-includes=DIR   XmHTML include files are in DIR],
xmhtml_includes="$withval")
AC_ARG_WITH(xmhtml-libraries,
[  --with-xmhtml-libraries=DIR  XmHTML libraries are in DIR],
xmhtml_libraries="$withval")
AC_MSG_CHECKING(for XmHTML)
#
#
# Search the include files.
#
if test "$xmhtml_includes" = ""; then
AC_CACHE_VAL(ice_cv_xmhtml_includes,
[
ice_xmhtml_save_LIBS="$LIBS"
ice_xmhtml_save_CFLAGS="$CFLAGS"
ice_xmhtml_save_CPPFLAGS="$CPPFLAGS"
ice_xmhtml_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_COMPILE([#include <XmHTML/XmHTML.h>],[int a;],
[
# XmHTML/XmHTML.h is in the standard search path.
ice_cv_xmhtml_includes=
],
[
# XmHTML/XmHTML.h is not in the standard search path.
# Locate it and put its directory in `xmhtml_includes'
#
# /usr/include/Motif* are used on HP-UX (Motif).
# /usr/include/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
ice_cv_xmhtml_includes="none"
for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
           /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
           /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
           /usr/dt/include /usr/openwin/include \
           /usr/dt/*/include /opt/*/include /usr/include/Motif* \
	   /home/XmHTML/include /usr/XmHTML/include /opt/XmHTML/include \
	   /home/XmHTML*/include /usr/XmHTML*/include /opt/XmHTML*/include \
           "${prefix}"/*/include /usr/*/include /usr/local/*/include \
           "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
if test -f "$dir/XmHTML/XmHTML.h"; then
	ice_cv_xmhtml_includes="$dir"
	break
fi
done
])
#
LIBS="$ice_xmhtml_save_LIBS"
CFLAGS="$ice_xmhtml_save_CFLAGS"
CPPFLAGS="$ice_xmhtml_save_CPPFLAGS"
LDFLAGS="$ice_xmhtml_save_LDFLAGS"
])
xmhtml_includes="$ice_cv_xmhtml_includes"
fi
#
#
# Now for the libraries.
#
if test "$xmhtml_libraries" = ""; then
AC_CACHE_VAL(ice_cv_xmhtml_libraries,
[
ice_xmhtml_save_LIBS="$LIBS"
ice_xmhtml_save_CFLAGS="$CFLAGS"
ice_xmhtml_save_CPPFLAGS="$CPPFLAGS"
ice_xmhtml_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXmHTML -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_LINK([#include <XmHTML/XmHTML.h>],[XmCreateHTML();],
[
# libXm.a is in the standard search path.
ice_cv_xmhtml_libraries=
],
[
# libXm.a is not in the standard search path.
# Locate it and put its directory in `xmhtml_libraries'
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/lesstif is used on Linux (Lesstif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
ice_cv_xmhtml_libraries="none"
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
           /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
           /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
           /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
           /usr/lesstif*/lib /usr/lib/Lesstif* \
	   /home/XmHTML/lib /usr/XmHTML/lib /opt/XmHTML/lib \
	   /home/XmHTML*/lib /usr/XmHTML*/lib /opt/XmHTML*/lib \
           "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
           "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libXmHTML.* 2> /dev/null`" != ""; then
	ice_cv_xmhtml_libraries="$dir"
	break
fi
done
])
#
LIBS="$ice_xmhtml_save_LIBS"
CFLAGS="$ice_xmhtml_save_CFLAGS"
CPPFLAGS="$ice_xmhtml_save_CPPFLAGS"
LDFLAGS="$ice_xmhtml_save_LDFLAGS"
])
#
xmhtml_libraries="$ice_cv_xmhtml_libraries"
fi
#
# Provide an easier way to link
#
# Okay
#
# Let's start by making sure that we completely abandon everything related
# to XmHTML installation if either the library or the includes have not been
# located, OR if there was a problem locating the Motif libraries, which are
# required for the use of XmHTML.  The opening three conditions, if true, will
# bypass all XmHTML config operations; that is to say, if any of these
# conditions is true, we call with_xmhtml "no", and that's the end of the
# game.
#
if test "$with_xmhtml" = "no" ; then
        with_xmhtml="no"
elif test "$xmhtml_includes" = "none" ; then
        with_xmhtml="no"
elif test "$xmhtml_libraries" = "none"; then
        with_xmhtml="no"
else
#
# We now have established that we want to use XmHTML. It's time to set up the
# basic environment, and do some discrete tests to set up the environment.
#
# First, let's set with_xmhtml to "yes" (don't know of this is really
# necessary, but we'll be conservative here).  We also send HAVE_XmHTML
# to config.h and the cache file.
#
        AC_DEFINE(HAVE_XmHTML_H)
        with_xmhtml="yes"
#
# Then let's see if the includes were NOT in the default path (if they were,
# we won't be needing an -I to point at the headers, because the compiler
# will find them by itself).  We've already eliminated the possibility of
# "none", so anything other than "" will definitely be a path.
#
#
        if test "$xmhtml_includes" != ""; then
                include_xmhtml="-I$xmhtml_includes"
        fi
#
# Now that that's out of the way, let's deal with libraries.  Here,
# we check again to see if the variable (xmhtml_libraries this time)
# is an empty string, but this time we have work to do whether the
# test is true or false.  We start with the case of an empty
# string, which means we want to link with XmHTML, but don't need
# a path to the library.
#
# This isn't quite happy yet.  A test for the location of the jpeg
# and Xext libraries should be added.
#
        if test "$xmhtml_libraries" = ""; then
                link_xmhtml="-lXmHTML -lXext -ljpeg -lpng -lz"
        else
                link_xmhtml="-L$xmhtml_libraries -lXmHTML -lXext -ljpeg -lpng -lz"
        fi
#
# We now close the enclosing conditional.
#
fi
#
AC_SUBST(include_xmhtml)
AC_SUBST(link_xmhtml)
#
#
#
xmhtml_libraries_result="$xmhtml_libraries"
xmhtml_includes_result="$xmhtml_includes"
test "$xmhtml_libraries_result" = "" && xmhtml_libraries_result="in default path"
test "$xmhtml_includes_result" = "" && xmhtml_includes_result="in default path"
test "$xmhtml_libraries_result" = "none" && xmhtml_libraries_result="(none)"
test "$xmhtml_includes_result" = "none" && xmhtml_includes_result="(none)"
AC_MSG_RESULT(
  [libraries $xmhtml_libraries_result, headers $xmhtml_includes_result])
])dnl

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
AC_DEFUN(AC_FIND_XLT,
[
AC_REQUIRE([AC_FIND_MOTIF])
AC_REQUIRE([AC_FIND_XPM])
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
		   /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
		   /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
		   /usr/dt/include /usr/openwin/include \
		   /usr/dt/*/include /opt/*/include /usr/include/Xlt* \
		   "${prefix}"/*/include /usr/*/include /usr/local/*/include \
		   "${prefix}"/include/* /usr/include/* /usr/local/include/*
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
		   /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
		   /usr/dt/lib /usr/openwin/lib \
		   /usr/dt/*/lib /opt/*/lib /usr/lib/Xlt* \
		   /usr/lesstif*/lib /usr/lib/Lesstif* \
		   "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
		   "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
	    for ext in "sl" "so" "a"; do
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
	AC_DEFINE(HAVE_XLT)
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

dnl
dnl
dnl AC_FIND_XPM : find libXpm, and provide variables
dnl	to easily use them in a Makefile.
dnl
dnl Adapted from a macro by Andreas Zeller.
dnl
dnl The variables provided are :
dnl	link_xpm		(e.g. -L/usr/lesstif/lib -lXm)
dnl	include_xpm		(e.g. -I/usr/lesstif/lib)
dnl	xpm_libraries		(e.g. /usr/lesstif/lib)
dnl	xpm_includes		(e.g. /usr/lesstif/include)
dnl
dnl The link_xpm and include_xpm variables should be fit to put on
dnl your application's link line in your Makefile.
dnl
AC_DEFUN(AC_FIND_XPM,
[
AC_REQUIRE([AC_PATH_XTRA])
xpm_includes=
xpm_libraries=
AC_ARG_WITH(xpm,
[  --without-xpm         do not use Xpm])
dnl Treat --without-xpm like
dnl --without-xpm-includes --without-xpm-libraries.
if test "$with_xpm" = "no"
then
    xpm_includes=no
    xpm_libraries=no
fi
AC_ARG_WITH(xpm-includes,
    [  --with-xpm-includes=DIR    Xpm include files are in DIR], xpm_includes="$withval")
AC_ARG_WITH(xpm-libraries,
    [  --with-xpm-libraries=DIR   Xpm libraries are in DIR], xpm_libraries="$withval")
if test "$xpm_includes" = "no" && test "$xpm_libraries" = "no"
then
    with_xpm="no"
fi

AC_MSG_CHECKING([for Xpm])
if test "$with_xpm" != "no"
then
    #
    #
    # Search the include files.
    #
    if test "$xpm_includes" = ""
    then
	AC_CACHE_VAL(ac_cv_xpm_includes,
	[
	ac_xpm_save_CFLAGS="$CFLAGS"
	ac_xpm_save_CPPFLAGS="$CPPFLAGS"
	#
	CFLAGS="$X_CFLAGS $CFLAGS"
	CPPFLAGS="$X_CFLAGS $CPPFLAGS"
	#
	AC_TRY_COMPILE([#include <X11/xpm.h>],[int a;],
	[
	# X11/xpm.h is in the standard search path.
	ac_cv_xpm_includes=
	],
	[
	# X11/xpm.h is not in the standard search path.
	# Locate it and put its directory in `xpm_includes'
	#
	# Other directories are just guesses.
	for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
		   /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
		   /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
		   /usr/dt/include /usr/openwin/include \
		   /usr/dt/*/include /opt/*/include /usr/include/Xpm* \
		   "${prefix}"/*/include /usr/*/include /usr/local/*/include \
		   "${prefix}"/include/* /usr/include/* /usr/local/include/*
	do
	    if test -f "$dir/X11/xpm.h"
	    then
		ac_cv_xpm_includes="$dir"
		break
	    fi
	done
	])
	#
	CFLAGS="$ac_xpm_save_CFLAGS"
	CPPFLAGS="$ac_xpm_save_CPPFLAGS"
	])
	xpm_includes="$ac_cv_xpm_includes"
    fi

    if test -z "$xpm_includes"
    then
	xpm_includes_result="default path"
	XPM_CFLAGS=""
    else
	if test "$xpm_includes" = "no"
	then
	    xpm_includes_result="told not to use them"
	    XPM_CFLAGS=""
	else
	    xpm_includes_result="$xpm_includes"
	    XPM_CFLAGS="-I$xpm_includes"
	fi
    fi
    #
    #
    # Now for the libraries.
    #
    if test "$xpm_libraries" = ""
    then
	AC_CACHE_VAL(ac_cv_xpm_libraries,
	[
	ac_xpm_save_LIBS="$LIBS"
	ac_xpm_save_CFLAGS="$CFLAGS"
	ac_xpm_save_CPPFLAGS="$CPPFLAGS"
	#
	LIBS="-lXpm $X_LIBS -lX11 $X_EXTRA_LIBS $LIBS"
	CFLAGS="$XPM_CFLAGS $X_CFLAGS $CFLAGS"
	CPPFLAGS="$XPM_CFLAGS $X_CFLAGS $CPPFLAGS"
	#
	AC_TRY_LINK([#include <X11/xpm.h>],[XpmAttributesSize();],
	[
	# libXpm.a is in the standard search path.
	ac_cv_xpm_libraries=
	],
	[
	# libXpm.a is not in the standard search path.
	# Locate it and put its directory in `xpm_libraries'
	#
	# Other directories are just guesses.
	for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
		   /usr/lib/Xlt2.0 /usr/lib/Xlt1.2 /usr/lib/Xlt1.1 \
		   /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
		   /usr/dt/lib /usr/openwin/lib \
		   /usr/dt/*/lib /opt/*/lib /usr/lib/Xpm* \
		   /usr/lesstif*/lib /usr/lib/Lesstif* \
		   "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
		   "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
	    for ext in "sl" "so" "a"; do
		if test -d "$dir" && test -f "$dir/libXpm.$ext"; then
		    ac_cv_xpm_libraries="$dir"
		    break 2
		fi
	    done
	done
	])
	#
	LIBS="$ac_xpm_save_LIBS"
	CFLAGS="$ac_xpm_save_CFLAGS"
	CPPFLAGS="$ac_xpm_save_CPPFLAGS"
	])
	#
	xpm_libraries="$ac_cv_xpm_libraries"
    fi
    if test -z "$xpm_libraries"
    then
	xpm_libraries_result="default path"
	XPM_LIBS="-lXpm"
    else
	if test "$xpm_libraries" = "no"
	then
	    xpm_libraries_result="told not to use it"
	    XPM_LIBS=""
	else
	    xpm_libraries_result="$xpm_libraries"
	    XPM_LIBS="-L$xpm_libraries -lXpm"
	fi
    fi
#
# Make sure, whatever we found out, we can link.
#
    ac_xpm_save_LIBS="$LIBS"
    ac_xpm_save_CFLAGS="$CFLAGS"
    ac_xpm_save_CPPFLAGS="$CPPFLAGS"
    #
    LIBS="$XPM_LIBS -lXpm $X_LIBS -lX11 $X_EXTRA_LIBS $LIBS"
    CFLAGS="$XPM_CFLAGS $X_CFLAGS $CFLAGS"
    CPPFLAGS="$XPM_CFLAGS $X_CFLAGS $CPPFLAGS"

    AC_TRY_LINK([#include <X11/xpm.h>],[XpmAttributesSize();],
	[
	#
	# link passed
	#
	AC_DEFINE(HAVE_XPM)
	],
	[
	#
	# link failed
	#
	xpm_libraries_result="test link failed"
	xpm_includes_result="test link failed"
	with_xpm="no"
	XPM_CFLAGS=""
	XPM_LIBS=""
	]) dnl AC_TRY_LINK

    LIBS="$ac_xpm_save_LIBS"
    CFLAGS="$ac_xpm_save_CFLAGS"
    CPPFLAGS="$ac_xpm_save_CPPFLAGS"
else
    xpm_libraries_result="told not to use it"
    xpm_includes_result="told not to use them"
    XPM_CFLAGS=""
    XPM_LIBS=""
fi
AC_MSG_RESULT([libraries $xpm_libraries_result, headers $xpm_includes_result])
AC_SUBST(XPM_CFLAGS)
AC_SUBST(XPM_LIBS)
])dnl AC_DEFN
dnl
dnl Add macros
dnl	AC_PATH_MOTIF_DIRECT
dnl	AC_PATH_MOTIF
dnl

AC_DEFUN(AC_PATH_MOTIF_DIRECT,
[test -z "$motif_direct_test_library" && motif_direct_test_library=Xm
test -z "$motif_direct_test_function" && motif_direct_test_function=XmCreatePushButton
test -z "$motif_direct_test_include" && motif_direct_test_include=Xm/Xm.h
  for ac_dir in               \
    /usr/include/Motif1.2     \
    /usr/Motif1.2/include     \
                              \
    /usr/motif/include        \
                              \
    /usr/X11R6/include        \
    /usr/X11R5/include        \
                              \
    /usr/include/X11R6        \
    /usr/include/X11R5        \
                              \
    /usr/local/X11R6/include  \
    /usr/local/X11R5/include  \
                              \
    /usr/local/include/X11R6  \
    /usr/local/include/X11R5  \
                              \
    /usr/X11/include          \
    /usr/include/X11          \
    /usr/local/X11/include    \
    /usr/local/include/X11    \
                              \
    /usr/X386/include         \
    /usr/x386/include         \
    /usr/XFree86/include/X11  \
                              \
    /usr/dt/include           \
    /usr/openwin/include      \
    /opt/openwin/include      \
                              \
    /usr/include              \
    /usr/local/include        \
    /usr/unsupported/include  \
    /usr/athena/include       \
    /usr/local/x11r5/include  \
    /usr/lpp/Xamples/include  \
    $PWD/../../include/Motif-1.2   \
    ; \
  do
    if test -r "$ac_dir/$motif_direct_test_include"; then
      no_motif= ac_motif_includes=$ac_dir
      break
    fi
  done

# Check for the libraries.
# See if we find them without any special options.
# Don't add to $LIBS permanently.
ac_save_LIBS="$LIBS"
LIBS="-l$motif_direct_test_library $LIBS"
# First see if replacing the include by lib works.
for ac_dir in `echo "$ac_motif_includes" | sed s/include/lib/` \
    /usr/lib/Motif1.2     \
    /usr/Motif1.2/lib     \
                          \
    /usr/motif/lib        \
                          \
    /usr/X11R6/lib        \
    /usr/X11R5/lib        \
                          \
    /usr/lib/X11R6        \
    /usr/lib/X11R5        \
                          \
    /usr/local/X11R6/lib  \
    /usr/local/X11R5/lib  \
                          \
    /usr/local/lib/X11R6  \
    /usr/local/lib/X11R5  \
                          \
    /usr/X11/lib          \
    /usr/lib/X11          \
    /usr/local/X11/lib    \
                          \
    /usr/X386/lib         \
    /usr/x386/lib         \
    /usr/XFree86/lib/X11  \
                          \
    /usr/dt/lib           \
    /usr/openwin/lib      \
    /opt/openwin/lib      \
                          \
    /usr/lib              \
    /usr/local/lib        \
    /usr/unsupported/lib  \
    /usr/athena/lib       \
    /usr/local/x11r5/lib  \
    /usr/lpp/Xamples/lib  \
    $PWD/../Xm/.libs      \
    ; \
do
  for ac_extension in a so sl; do
    if test -r $ac_dir/lib${motif_direct_test_library}.$ac_extension; then
      no_motif= ac_motif_libraries=$ac_dir
      break 2
    fi
  done
done
LIBS=$ac_save_LIBS])

AC_DEFUN(AC_PATH_MOTIF,
[AC_REQUIRE_CPP()dnl

AC_ARG_WITH(motif-includes, [  --with-motif-includes=DIR     Motif include files are in DIR])
if test -z "$with_motif_includes"; then
  motif_includes=NONE
else
  motif_includes=$with_motif_includes
fi
AC_ARG_WITH(motif-libraries, [  --with-motif-libraries=DIR    Motif library files are in DIR])
if test -z "$with_motif_libraries"; then
  motif_libraries=NONE
else
  motif_libraries=$with_motif_libraries
fi

AC_MSG_CHECKING(for Motif)
AC_ARG_WITH(motif, [  --with-motif            enable Motif tests])
if test "x$with_motif" = xno; then
  no_motif=yes
else
  if test "x$motif_includes" != xNONE && test "x$motif_libraries" != xNONE; then
    no_motif=
  else
AC_CACHE_VAL(ac_cv_path_motif,
[# One or both of these vars are not set, and there is no cached value.
no_motif=yes
AC_PATH_MOTIF_DIRECT

if test "$no_motif" = yes; then
  ac_cv_path_motif="no_motif=yes"
else
  ac_cv_path_motif="no_motif= ac_motif_includes=$ac_motif_includes ac_motif_libraries=$ac_motif_libraries"
fi])dnl
  fi
  eval "$ac_cv_path_motif"
fi # with_motif != no

if test "$no_motif" = yes; then
  AC_MSG_RESULT(no)
else
  test "x$motif_includes" = xNONE && motif_includes=$ac_motif_includes
  test "x$motif_libraries" = xNONE && motif_libraries=$ac_motif_libraries
  ac_cv_path_motif="no_motif= ac_motif_includes=$motif_includes ac_motif_libraries=$motif_libraries"
  AC_MSG_RESULT([libraries $motif_libraries, headers $motif_includes])
fi
test "x$motif_libraries" != "xNONE" && test "x$motif_libraries" != "x" && MOTIF_LIBS=-L$motif_libraries
test "x$motif_includes" != "xNONE" && test "x$motif_includes" != "x" && MOTIF_CFLAGS=-I$motif_includes
AC_SUBST(MOTIF_LIBS)
AC_SUBST(MOTIF_CFLAGS)
])

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

dnl
dnl
dnl AC_LINK_X : find X11 (minimal file to OPTIONALLY link Oleo
dnl	with X11 libraries.
dnl
dnl Adapted from a macro by Andreas Zeller.
dnl
dnl The variable provided is :
dnl	link_x			(e.g. -L/usr/X11R6/lib -lX11 -lXt)
dnl
dnl The link_x variable should be fit to put on the application's
dnl link line in the Makefile.
dnl
dnl Oleo CVS $Id: x11.m4,v 1.2 1999/02/23 21:24:40 danny Exp $
dnl
AC_DEFUN(AC_LINK_X,
[if test "$with_x" = "yes"; then
  X_LIBS="${X_LIBS} -lXt -lX11"
fi])


#serial 4

dnl From Jim Meyering.
dnl FIXME: this should migrate into libit.

AC_DEFUN(AM_FUNC_MKTIME,
[AC_REQUIRE([AC_HEADER_TIME])dnl
 AC_CHECK_HEADERS(sys/time.h unistd.h)
 AC_CHECK_FUNCS(alarm)
 AC_CACHE_CHECK([for working mktime], am_cv_func_working_mktime,
  [AC_TRY_RUN(
changequote(<<, >>)dnl
<</* Test program from Paul Eggert (eggert@twinsun.com)
   and Tony Leneis (tony@plaza.ds.adp.com).  */
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if !HAVE_ALARM
# define alarm(X) /* empty */
#endif

/* Work around redefinition to rpl_putenv by other config tests.  */
#undef putenv

static time_t time_t_max;

/* Values we'll use to set the TZ environment variable.  */
static const char *const tz_strings[] = {
  (const char *) 0, "TZ=GMT0", "TZ=JST-9",
  "TZ=EST+3EDT+2,M10.1.0/00:00:00,M2.3.0/00:00:00"
};
#define N_STRINGS (sizeof (tz_strings) / sizeof (tz_strings[0]))

/* Fail if mktime fails to convert a date in the spring-forward gap.
   Based on a problem report from Andreas Jaeger.  */
static void
spring_forward_gap ()
{
  /* glibc (up to about 1998-10-07) failed this test) */
  struct tm tm;

  /* Use the portable POSIX.1 specification "TZ=PST8PDT,M4.1.0,M10.5.0"
     instead of "TZ=America/Vancouver" in order to detect the bug even
     on systems that don't support the Olson extension, or don't have the
     full zoneinfo tables installed.  */
  putenv ("TZ=PST8PDT,M4.1.0,M10.5.0");

  tm.tm_year = 98;
  tm.tm_mon = 3;
  tm.tm_mday = 5;
  tm.tm_hour = 2;
  tm.tm_min = 0;
  tm.tm_sec = 0;
  tm.tm_isdst = -1;
  if (mktime (&tm) == (time_t)-1)
    exit (1);
}

static void
mktime_test (now)
     time_t now;
{
  struct tm *lt;
  if ((lt = localtime (&now)) && mktime (lt) != now)
    exit (1);
  now = time_t_max - now;
  if ((lt = localtime (&now)) && mktime (lt) != now)
    exit (1);
}

static void
irix_6_4_bug ()
{
  /* Based on code from Ariel Faigon.  */
  struct tm tm;
  tm.tm_year = 96;
  tm.tm_mon = 3;
  tm.tm_mday = 0;
  tm.tm_hour = 0;
  tm.tm_min = 0;
  tm.tm_sec = 0;
  tm.tm_isdst = -1;
  mktime (&tm);
  if (tm.tm_mon != 2 || tm.tm_mday != 31)
    exit (1);
}

static void
bigtime_test (j)
     int j;
{
  struct tm tm;
  time_t now;
  tm.tm_year = tm.tm_mon = tm.tm_mday = tm.tm_hour = tm.tm_min = tm.tm_sec = j;
  /* This test makes some buggy mktime implementations loop.
     Give up after 10 seconds.  */
  alarm (10);
  now = mktime (&tm);
  alarm (0);
  if (now != (time_t) -1)
    {
      struct tm *lt = localtime (&now);
      if (! (lt
	     && lt->tm_year == tm.tm_year
	     && lt->tm_mon == tm.tm_mon
	     && lt->tm_mday == tm.tm_mday
	     && lt->tm_hour == tm.tm_hour
	     && lt->tm_min == tm.tm_min
	     && lt->tm_sec == tm.tm_sec
	     && lt->tm_yday == tm.tm_yday
	     && lt->tm_wday == tm.tm_wday
	     && ((lt->tm_isdst < 0 ? -1 : 0 < lt->tm_isdst)
		  == (tm.tm_isdst < 0 ? -1 : 0 < tm.tm_isdst))))
	exit (1);
    }
}

int
main ()
{
  time_t t, delta;
  int i, j;

  for (time_t_max = 1; 0 < time_t_max; time_t_max *= 2)
    continue;
  time_t_max--;
  delta = time_t_max / 997; /* a suitable prime number */
  for (i = 0; i < N_STRINGS; i++)
    {
      if (tz_strings[i])
	putenv (tz_strings[i]);

      for (t = 0; t <= time_t_max - delta; t += delta)
	mktime_test (t);
      mktime_test ((time_t) 60 * 60);
      mktime_test ((time_t) 60 * 60 * 24);

      for (j = 1; 0 < j; j *= 2)
        bigtime_test (j);
      bigtime_test (j - 1);
    }
  irix_6_4_bug ();
  spring_forward_gap ();
  exit (0);
}
	      >>,
changequote([, ])dnl
	     am_cv_func_working_mktime=yes, am_cv_func_working_mktime=no,
	     dnl When crosscompiling, assume mktime is missing or broken.
	     am_cv_func_working_mktime=no)
  ])
  if test $am_cv_func_working_mktime = no; then
    LIBOBJS="$LIBOBJS mktime.o"
  fi
])

dnl From Jim Meyering.
dnl FIXME: migrate into libit.

AC_DEFUN(AM_FUNC_OBSTACK,
[AC_CACHE_CHECK([for obstacks], am_cv_func_obstack,
 [AC_TRY_LINK([#include "obstack.h"],
	      [struct obstack *mem;obstack_free(mem,(char *) 0)],
	      am_cv_func_obstack=yes,
	      am_cv_func_obstack=no)])
 if test $am_cv_func_obstack = yes; then
   AC_DEFINE(HAVE_OBSTACK,1,[Define if libc includes obstacks])
 else
   LIBOBJS="$LIBOBJS obstack.o"
 fi
])

dnl
dnl non-fatal check for c++ compiler
dnl
AC_DEFUN(AC_PROG_CXX_NONFATAL,
[AC_BEFORE([$0], [AC_PROG_CXXCPP])dnl
AC_CHECK_PROGS(CXX, $CCC c++ g++ gcc CC cxx cc++ cl, gcc)

dnl
dnl Not doing the "works" test is the only difference.
dnl AC_PROG_CXX_WORKS
dnl
AC_PROG_CXX_WORKS_NONFATAL

if test $CXX != "no"; then

AC_PROG_CXX_GNU

if test $ac_cv_prog_gxx = yes; then
  GXX=yes
else 
  GXX=
fi
dnl Check whether -g works, even if CXXFLAGS is set, in case the package
dnl plays around with CXXFLAGS (such as to build both debugging and
dnl normal versions of a library), tasteless as that idea is.
ac_test_CXXFLAGS="${CXXFLAGS+set}"
ac_save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS=
AC_PROG_CXX_G
if test "$ac_test_CXXFLAGS" = set; then
  CXXFLAGS="$ac_save_CXXFLAGS"
elif test $ac_cv_prog_cxx_g = yes; then
  if test "$GXX" = yes; then
    CXXFLAGS="-g -O2"
  else
    CXXFLAGS="-g"
  fi
else
  if test "$GXX" = yes; then
    CXXFLAGS="-O2"
  else
    CXXFLAGS=
  fi
fi
fi
])

AC_DEFUN(AC_PROG_CXX_WORKS_NONFATAL,
[AC_MSG_CHECKING([whether the C++ compiler ($CXX $CXXFLAGS $LDFLAGS) works])
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILER([int main(){return(0);}], ac_cv_prog_cxx_works, ac_cv_prog_cxx_cross)
AC_LANG_RESTORE
AC_MSG_RESULT($ac_cv_prog_cxx_works)
if test $ac_cv_prog_cxx_works = no; then
  AC_MSG_WARN([installation or configuration problem: C++ compiler cannot create executables.])
  CXX="no"
else
  AC_MSG_CHECKING([whether the C++ compiler ($CXX $CXXFLAGS $LDFLAGS) is a cross-compiler])
  AC_MSG_RESULT($ac_cv_prog_cxx_cross)
  cross_compiling=$ac_cv_prog_cxx_cross
fi
])

# Macro to add for using GNU gettext.
# Ulrich Drepper <drepper@cygnus.com>, 1995.
#
# This file can be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.

# serial 5

AC_DEFUN(AM_WITH_NLS,
  [AC_MSG_CHECKING([whether NLS is requested])
    dnl Default is enabled NLS
    AC_ARG_ENABLE(nls,
      [  --disable-nls           do not use Native Language Support],
      USE_NLS=$enableval, USE_NLS=yes)
    AC_MSG_RESULT($USE_NLS)
    AC_SUBST(USE_NLS)

    USE_INCLUDED_LIBINTL=no

    dnl If we use NLS figure out what method
    if test "$USE_NLS" = "yes"; then
      AC_DEFINE(ENABLE_NLS)
      AC_MSG_CHECKING([whether included gettext is requested])
      AC_ARG_WITH(included-gettext,
        [  --with-included-gettext use the GNU gettext library included here],
        nls_cv_force_use_gnu_gettext=$withval,
        nls_cv_force_use_gnu_gettext=no)
      AC_MSG_RESULT($nls_cv_force_use_gnu_gettext)

      nls_cv_use_gnu_gettext="$nls_cv_force_use_gnu_gettext"
      if test "$nls_cv_force_use_gnu_gettext" != "yes"; then
        dnl User does not insist on using GNU NLS library.  Figure out what
        dnl to use.  If gettext or catgets are available (in this order) we
        dnl use this.  Else we have to fall back to GNU NLS library.
	dnl catgets is only used if permitted by option --with-catgets.
	nls_cv_header_intl=
	nls_cv_header_libgt=
	CATOBJEXT=NONE

	AC_CHECK_HEADER(libintl.h,
	  [AC_CACHE_CHECK([for gettext in libc], gt_cv_func_gettext_libc,
	    [AC_TRY_LINK([#include <libintl.h>], [return (int) gettext ("")],
	       gt_cv_func_gettext_libc=yes, gt_cv_func_gettext_libc=no)])

	   if test "$gt_cv_func_gettext_libc" != "yes"; then
	     AC_CHECK_LIB(intl, bindtextdomain,
	       [AC_CACHE_CHECK([for gettext in libintl],
		 gt_cv_func_gettext_libintl,
		 [AC_CHECK_LIB(intl, gettext,
		  gt_cv_func_gettext_libintl=yes,
		  gt_cv_func_gettext_libintl=no)],
		 gt_cv_func_gettext_libintl=no)])
	   fi

	   if test "$gt_cv_func_gettext_libc" = "yes" \
	      || test "$gt_cv_func_gettext_libintl" = "yes"; then
	      AC_DEFINE(HAVE_GETTEXT)
	      AM_PATH_PROG_WITH_TEST(MSGFMT, msgfmt,
		[test -z "`$ac_dir/$ac_word -h 2>&1 | grep 'dv '`"], no)dnl
	      if test "$MSGFMT" != "no"; then
		AC_CHECK_FUNCS(dcgettext)
		AC_PATH_PROG(GMSGFMT, gmsgfmt, $MSGFMT)
		AM_PATH_PROG_WITH_TEST(XGETTEXT, xgettext,
		  [test -z "`$ac_dir/$ac_word -h 2>&1 | grep '(HELP)'`"], :)
		AC_TRY_LINK(, [extern int _nl_msg_cat_cntr;
			       return _nl_msg_cat_cntr],
		  [CATOBJEXT=.gmo
		   DATADIRNAME=share],
		  [CATOBJEXT=.mo
		   DATADIRNAME=lib])
		INSTOBJEXT=.mo
	      fi
	    fi
	])

        if test "$CATOBJEXT" = "NONE"; then
	  AC_MSG_CHECKING([whether catgets can be used])
	  AC_ARG_WITH(catgets,
	    [  --with-catgets          use catgets functions if available],
	    nls_cv_use_catgets=$withval, nls_cv_use_catgets=no)
	  AC_MSG_RESULT($nls_cv_use_catgets)

	  if test "$nls_cv_use_catgets" = "yes"; then
	    dnl No gettext in C library.  Try catgets next.
	    AC_CHECK_LIB(i, main)
	    AC_CHECK_FUNC(catgets,
	      [AC_DEFINE(HAVE_CATGETS)
	       INTLOBJS="\$(CATOBJS)"
	       AC_PATH_PROG(GENCAT, gencat, no)dnl
	       if test "$GENCAT" != "no"; then
		 AC_PATH_PROG(GMSGFMT, gmsgfmt, no)
		 if test "$GMSGFMT" = "no"; then
		   AM_PATH_PROG_WITH_TEST(GMSGFMT, msgfmt,
		    [test -z "`$ac_dir/$ac_word -h 2>&1 | grep 'dv '`"], no)
		 fi
		 AM_PATH_PROG_WITH_TEST(XGETTEXT, xgettext,
		   [test -z "`$ac_dir/$ac_word -h 2>&1 | grep '(HELP)'`"], :)
		 USE_INCLUDED_LIBINTL=yes
		 CATOBJEXT=.cat
		 INSTOBJEXT=.cat
		 DATADIRNAME=lib
		 INTLDEPS='$(top_builddir)/intl/libintl.a'
		 INTLLIBS=$INTLDEPS
		 LIBS=`echo $LIBS | sed -e 's/-lintl//'`
		 nls_cv_header_intl=intl/libintl.h
		 nls_cv_header_libgt=intl/libgettext.h
	       fi])
	  fi
        fi

        if test "$CATOBJEXT" = "NONE"; then
	  dnl Neither gettext nor catgets in included in the C library.
	  dnl Fall back on GNU gettext library.
	  nls_cv_use_gnu_gettext=yes
        fi
      fi

      if test "$nls_cv_use_gnu_gettext" = "yes"; then
        dnl Mark actions used to generate GNU NLS library.
        INTLOBJS="\$(GETTOBJS)"
        AM_PATH_PROG_WITH_TEST(MSGFMT, msgfmt,
	  [test -z "`$ac_dir/$ac_word -h 2>&1 | grep 'dv '`"], msgfmt)
        AC_PATH_PROG(GMSGFMT, gmsgfmt, $MSGFMT)
        AM_PATH_PROG_WITH_TEST(XGETTEXT, xgettext,
	  [test -z "`$ac_dir/$ac_word -h 2>&1 | grep '(HELP)'`"], :)
        AC_SUBST(MSGFMT)
	USE_INCLUDED_LIBINTL=yes
        CATOBJEXT=.gmo
        INSTOBJEXT=.mo
        DATADIRNAME=share
	INTLDEPS='$(top_builddir)/intl/libintl.a'
	INTLLIBS=$INTLDEPS
	LIBS=`echo $LIBS | sed -e 's/-lintl//'`
        nls_cv_header_intl=intl/libintl.h
        nls_cv_header_libgt=intl/libgettext.h
      fi

      dnl Test whether we really found GNU xgettext.
      if test "$XGETTEXT" != ":"; then
	dnl If it is no GNU xgettext we define it as : so that the
	dnl Makefiles still can work.
	if $XGETTEXT --omit-header /dev/null 2> /dev/null; then
	  : ;
	else
	  AC_MSG_RESULT(
	    [found xgettext program is not GNU xgettext; ignore it])
	  XGETTEXT=":"
	fi
      fi

      # We need to process the po/ directory.
      POSUB=po
    else
      DATADIRNAME=share
      nls_cv_header_intl=intl/libintl.h
      nls_cv_header_libgt=intl/libgettext.h
    fi
    AC_LINK_FILES($nls_cv_header_libgt, $nls_cv_header_intl)
    AC_OUTPUT_COMMANDS(
     [case "$CONFIG_FILES" in *po/Makefile.in*)
        sed -e "/POTFILES =/r po/POTFILES" po/Makefile.in > po/Makefile
      esac])


    # If this is used in GNU gettext we have to set USE_NLS to `yes'
    # because some of the sources are only built for this goal.
    if test "$PACKAGE" = gettext; then
      USE_NLS=yes
      USE_INCLUDED_LIBINTL=yes
    fi

    dnl These rules are solely for the distribution goal.  While doing this
    dnl we only have to keep exactly one list of the available catalogs
    dnl in configure.in.
    for lang in $ALL_LINGUAS; do
      GMOFILES="$GMOFILES $lang.gmo"
      POFILES="$POFILES $lang.po"
    done

    dnl Make all variables we use known to autoconf.
    AC_SUBST(USE_INCLUDED_LIBINTL)
    AC_SUBST(CATALOGS)
    AC_SUBST(CATOBJEXT)
    AC_SUBST(DATADIRNAME)
    AC_SUBST(GMOFILES)
    AC_SUBST(INSTOBJEXT)
    AC_SUBST(INTLDEPS)
    AC_SUBST(INTLLIBS)
    AC_SUBST(INTLOBJS)
    AC_SUBST(POFILES)
    AC_SUBST(POSUB)
  ])

AC_DEFUN(AM_GNU_GETTEXT,
  [AC_REQUIRE([AC_PROG_MAKE_SET])dnl
   AC_REQUIRE([AC_PROG_CC])dnl
   AC_REQUIRE([AC_PROG_RANLIB])dnl
   AC_REQUIRE([AC_ISC_POSIX])dnl
   AC_REQUIRE([AC_HEADER_STDC])dnl
   AC_REQUIRE([AC_C_CONST])dnl
   AC_REQUIRE([AC_C_INLINE])dnl
   AC_REQUIRE([AC_TYPE_OFF_T])dnl
   AC_REQUIRE([AC_TYPE_SIZE_T])dnl
   AC_REQUIRE([AC_FUNC_ALLOCA])dnl
   AC_REQUIRE([AC_FUNC_MMAP])dnl

   AC_CHECK_HEADERS([argz.h limits.h locale.h nl_types.h malloc.h string.h \
unistd.h sys/param.h])
   AC_CHECK_FUNCS([getcwd munmap putenv setenv setlocale strchr strcasecmp \
strdup __argz_count __argz_stringify __argz_next])

   if test "${ac_cv_func_stpcpy+set}" != "set"; then
     AC_CHECK_FUNCS(stpcpy)
   fi
   if test "${ac_cv_func_stpcpy}" = "yes"; then
     AC_DEFINE(HAVE_STPCPY)
   fi

   AM_LC_MESSAGES
   AM_WITH_NLS

   if test "x$CATOBJEXT" != "x"; then
     if test "x$ALL_LINGUAS" = "x"; then
       LINGUAS=
     else
       AC_MSG_CHECKING(for catalogs to be installed)
       NEW_LINGUAS=
       for lang in ${LINGUAS=$ALL_LINGUAS}; do
         case "$ALL_LINGUAS" in
          *$lang*) NEW_LINGUAS="$NEW_LINGUAS $lang" ;;
         esac
       done
       LINGUAS=$NEW_LINGUAS
       AC_MSG_RESULT($LINGUAS)
     fi

     dnl Construct list of names of catalog files to be constructed.
     if test -n "$LINGUAS"; then
       for lang in $LINGUAS; do CATALOGS="$CATALOGS $lang$CATOBJEXT"; done
     fi
   fi

   dnl The reference to <locale.h> in the installed <libintl.h> file
   dnl must be resolved because we cannot expect the users of this
   dnl to define HAVE_LOCALE_H.
   if test $ac_cv_header_locale_h = yes; then
     INCLUDE_LOCALE_H="#include <locale.h>"
   else
     INCLUDE_LOCALE_H="\
/* The system does not provide the header <locale.h>.  Take care yourself.  */"
   fi
   AC_SUBST(INCLUDE_LOCALE_H)

   dnl Determine which catalog format we have (if any is needed)
   dnl For now we know about two different formats:
   dnl   Linux libc-5 and the normal X/Open format
   test -d intl || mkdir intl
   if test "$CATOBJEXT" = ".cat"; then
     AC_CHECK_HEADER(linux/version.h, msgformat=linux, msgformat=xopen)

     dnl Transform the SED scripts while copying because some dumb SEDs
     dnl cannot handle comments.
     sed -e '/^#/d' $srcdir/intl/$msgformat-msg.sed > intl/po2msg.sed
   fi
   dnl po2tbl.sed is always needed.
   sed -e '/^#.*[^\\]$/d' -e '/^#$/d' \
     $srcdir/intl/po2tbl.sed.in > intl/po2tbl.sed

   dnl In the intl/Makefile.in we have a special dependency which makes
   dnl only sense for gettext.  We comment this out for non-gettext
   dnl packages.
   if test "$PACKAGE" = "gettext"; then
     GT_NO="#NO#"
     GT_YES=
   else
     GT_NO=
     GT_YES="#YES#"
   fi
   AC_SUBST(GT_NO)
   AC_SUBST(GT_YES)

   dnl If the AC_CONFIG_AUX_DIR macro for autoconf is used we possibly
   dnl find the mkinstalldirs script in another subdir but ($top_srcdir).
   dnl Try to locate is.
   MKINSTALLDIRS=
   if test -n "$ac_aux_dir"; then
     MKINSTALLDIRS="$ac_aux_dir/mkinstalldirs"
   fi
   if test -z "$MKINSTALLDIRS"; then
     MKINSTALLDIRS="\$(top_srcdir)/mkinstalldirs"
   fi
   AC_SUBST(MKINSTALLDIRS)

   dnl *** For now the libtool support in intl/Makefile is not for real.
   l=
   AC_SUBST(l)

   dnl Generate list of files to be processed by xgettext which will
   dnl be included in po/Makefile.
   test -d po || mkdir po
   if test "x$srcdir" != "x."; then
     if test "x`echo $srcdir | sed 's@/.*@@'`" = "x"; then
       posrcprefix="$srcdir/"
     else
       posrcprefix="../$srcdir/"
     fi
   else
     posrcprefix="../"
   fi
   rm -f po/POTFILES
   sed -e "/^#/d" -e "/^\$/d" -e "s,.*,	$posrcprefix& \\\\," -e "\$s/\(.*\) \\\\/\1/" \
	< $srcdir/po/POTFILES.in > po/POTFILES
  ])

# Search path for a program which passes the given test.
# Ulrich Drepper <drepper@cygnus.com>, 1996.
#
# This file can be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.

# serial 1

dnl AM_PATH_PROG_WITH_TEST(VARIABLE, PROG-TO-CHECK-FOR,
dnl   TEST-PERFORMED-ON-FOUND_PROGRAM [, VALUE-IF-NOT-FOUND [, PATH]])
AC_DEFUN(AM_PATH_PROG_WITH_TEST,
[# Extract the first word of "$2", so it can be a program name with args.
set dummy $2; ac_word=[$]2
AC_MSG_CHECKING([for $ac_word])
AC_CACHE_VAL(ac_cv_path_$1,
[case "[$]$1" in
  /*)
  ac_cv_path_$1="[$]$1" # Let the user override the test with a path.
  ;;
  *)
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:"
  for ac_dir in ifelse([$5], , $PATH, [$5]); do
    test -z "$ac_dir" && ac_dir=.
    if test -f $ac_dir/$ac_word; then
      if [$3]; then
	ac_cv_path_$1="$ac_dir/$ac_word"
	break
      fi
    fi
  done
  IFS="$ac_save_ifs"
dnl If no 4th arg is given, leave the cache variable unset,
dnl so AC_PATH_PROGS will keep looking.
ifelse([$4], , , [  test -z "[$]ac_cv_path_$1" && ac_cv_path_$1="$4"
])dnl
  ;;
esac])dnl
$1="$ac_cv_path_$1"
if test -n "[$]$1"; then
  AC_MSG_RESULT([$]$1)
else
  AC_MSG_RESULT(no)
fi
AC_SUBST($1)dnl
])

# Check whether LC_MESSAGES is available in <locale.h>.
# Ulrich Drepper <drepper@cygnus.com>, 1995.
#
# This file can be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.

# serial 1

AC_DEFUN(AM_LC_MESSAGES,
  [if test $ac_cv_header_locale_h = yes; then
    AC_CACHE_CHECK([for LC_MESSAGES], am_cv_val_LC_MESSAGES,
      [AC_TRY_LINK([#include <locale.h>], [return LC_MESSAGES],
       am_cv_val_LC_MESSAGES=yes, am_cv_val_LC_MESSAGES=no)])
    if test $am_cv_val_LC_MESSAGES = yes; then
      AC_DEFINE(HAVE_LC_MESSAGES)
    fi
  fi])

