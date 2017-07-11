AC_DEFUN([AX_GPROF], [


AC_ARG_WITH([gprof],
  [AS_HELP_STRING([--with-gprof], 
    [Enable GNU profiler gprof (default is no)
    You probably want to add option --with-debug])],
  [with_gprof=$withval],
  [with_gprof=no])


AM_CONDITIONAL([WITH_GPROF], [test "x$with_gprof" = "xyes"])

AS_IF([test "x$with_gprof" = "xyes"], [
  dnl AC_SUBST([GPROF_CXXFLAGS], [-pg])
  AC_SUBST([CXXFLAGS], ["$CXXFLAGS -pg"])
])

AC_MSG_NOTICE([checking if gprof profiling required ... $with_gprof])

])dnl AX_GPROF
