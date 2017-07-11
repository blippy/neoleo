AC_PREREQ([2.69])
AC_INIT([neoleo], [7.0.0])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_SRCDIR([src/main.cc])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIRS([m4])


AX_ASAN
AX_DEBUG
AX_GPROF

AC_PROG_CXX
AC_PROG_YACC

AC_CONFIG_FILES([Makefile doc/Makefile examples/Makefile src/Makefile])
AC_OUTPUT
