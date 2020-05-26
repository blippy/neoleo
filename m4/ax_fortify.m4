AC_DEFUN([AX_FORTIFY], [

AC_ARG_WITH([fortify],
  [AS_HELP_STRING([--with-fortify], [Enable Arch Linux-like Fortification (default is no)])],
  [with_fortify=$withval],
  [with_fortify=no])

AM_CONDITIONAL([WITH_FORTIFY], test "x$with_fortify" = "xyes")

AS_IF([test "x$with_fortify" = "xyes"], [
	AC_SUBST([FORTIFY_CXXFLAGS], ["$CXXFLAGS -O -D_FORTIFY_SOURCE=2"])
	AC_SUBST([FORTIFY_LIBS], [])],
	AC_SUBST([FORTIFY_CXXFLAGS], [])
	AC_SUBST([FORTIFY_LIBS], []) )

])dnl AX_ASAN
