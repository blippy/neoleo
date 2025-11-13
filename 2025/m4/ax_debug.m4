AC_DEFUN([AX_DEBUG], [


AC_ARG_WITH([debug],
  [AS_HELP_STRING([--with-debug], [Enable debug (default is no)])],
  [with_debug=$withval],
  [with_debug=no])


AM_CONDITIONAL([WITH_DEBUG], [test "x$with_debug" = "xyes"])

AS_IF([test "x$with_debug" = "xyes"], [
  dnl AC_SUBST([DEBUG_CXXFLAGS], ["-O0 -g"])
  AC_SUBST([CXXFLAGS], ["$CXXFLAGS -O0 -g"])
])

AC_MSG_NOTICE([checking if debugging required ... $with_debug])

])dnl AX_DEBUG
