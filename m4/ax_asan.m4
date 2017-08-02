AC_DEFUN([AX_ASAN], [

AC_ARG_WITH([asan],
  [AS_HELP_STRING([--with-asan], [Enable address sanitizer (default is no)])],
  [with_asan=$withval],
  [with_asan=no])

AM_CONDITIONAL([WITH_ASAN], test "x$with_asan" = "xyes")

])dnl AX_ASAN
