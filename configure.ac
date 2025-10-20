AC_PREREQ([2.69])
AC_INIT([neoleo], [16.0])
: ${CXXFLAGS=""}dnl https://www.gnu.org/savannah-checkouts/gnu/autoconf/manual/autoconf-2.69/html_node/C_002b_002b-Compiler.html
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_SRCDIR([src/main.cc])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIRS([m4])

AX_ASAN
AX_DEBUG
AX_FORM
AX_GPROF
AX_COVERAGE
AX_FORTIFY
#AC_SUBST(CURSES_LIBS, `pkg-config --libs ncurses`)
PKG_CHECK_MODULES(NCURSES, ncursesw)

#https://stackoverflow.com/questions/130801/how-do-you-create-tests-for-make-check-with-gnu-autotools
#PKG_CHECK_MODULES([CHECK], [check >= 0.9.10])

#export SRCDIR=$(abs_srcdir)
#export BUILDDIR=$(abs_top_builddir)
#export BUILDDIR=$(top_builddir)

AC_CHECK_PROG(HAVE_TEXI2HTML, texi2html, yes)
AC_MSG_CHECKING([for texi2html])
if test -z "$HAVE_TEXI2HTML"; then
	AC_MSG_RESULT([none found])
	AC_MSG_ERROR([Could not find texi2html. May apt install texi2html])
else
	AC_MSG_RESULT([$HAVE_TEXI2HTML])
fi
AM_CONDITIONAL(HaveTexi2Html, test x$HAVE_TEXI2HTML = xyes)

AC_CHECK_PROG(HAVE_MAKEINFO, makeinfo, yes)
AC_MSG_CHECKING([for makeinfo])
if test -z "$HAVE_MAKEINFO"; then
	AC_MSG_RESULT([none found])
	AC_MSG_ERROR([Could not find makeinfo. May apt install texinfo])
else
	AC_MSG_RESULT([$HAVE_MAKEINFO])
fi
AM_CONDITIONAL(HaveMakeinfo, test x$HAVE_MAKEINFO = xyes)



AC_PROG_CXX
#AC_PROG_YACC

AC_CONFIG_FILES([Makefile doc/Makefile examples/Makefile src/Makefile  
		 tests/Makefile tests/out/Makefile tests/verified/Makefile])
AC_OUTPUT
