AC_PREREQ([2.69])
AC_INIT([neoleo], [9.0])
: ${CXXFLAGS=""}dnl https://www.gnu.org/savannah-checkouts/gnu/autoconf/manual/autoconf-2.69/html_node/C_002b_002b-Compiler.html
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_SRCDIR([src/main.cc])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIRS([m4])

AX_ASAN
AX_DEBUG
AX_GPROF
AX_COVERAGE
AC_SUBST(CURSES_LIBS, `pkg-config --libs ncurses`)

AC_PROG_CXX
AC_PROG_YACC

AC_CONFIG_FILES([Makefile examples/Makefile src/Makefile  
		 tests/Makefile tests/out/Makefile tests/verified/Makefile])
AC_OUTPUT
