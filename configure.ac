AC_PREREQ([2.69])
AC_INIT([neoleo], [16.0])
AM_INIT_AUTOMAKE([foreign])
# : ${CXXFLAGS=""}dnl https://www.gnu.org/savannah-checkouts/gnu/autoconf/manual/autoconf-2.69/html_node/C_002b_002b-Compiler.html
AC_CONFIG_HEADERS([src/config.h])

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_MACRO_DIRS([m4])

# silent rules just gives you a summary of what is being compiled
# To disable, either do `make V=0' after configuration, or
# ./configure --enable-silent-rules on configuration
# https://stackoverflow.com/questions/17421641/how-to-enable-quiet-build-using-automake
AM_SILENT_RULES([yes])



AX_ASAN
AX_DEBUG
AX_GPROF
AX_COVERAGE
AX_FORTIFY
#AC_SUBST(CURSES_LIBS, `pkg-config --libs ncurses`)
PKG_CHECK_MODULES(NCURSES, ncursesw)

AC_PROG_CXX
#AC_PROG_YACC

AC_CONFIG_FILES([Makefile examples/Makefile src/Makefile  
		 tests/Makefile tests/out/Makefile tests/verified/Makefile])
AC_OUTPUT
