AC_DEFUN([AX_ASAN], [

AC_ARG_WITH([asan],
  [AS_HELP_STRING([--with-asan], [Enable address sanitizer (default is no)])],
  [with_asan=$withval],
  [with_asan=no])

AM_CONDITIONAL([WITH_ASAN], test "x$with_asan" = "xyes")

AS_IF([test "x$with_asan" = "xyes"], [
	AC_SUBST([ASAN_CXXFLAGS], ["-fsanitize=address -fno-omit-frame-pointer"])
	AC_SUBST([ASAN_LIBS], ["-lasan"])],
	AC_SUBST([ASAN_CXXFLAGS], [])
	AC_SUBST([ASAN_LIBS], []) )

])dnl AX_ASAN
