dnl Arch Linux has an uncanny ability to keep breaking the build process.
dnl This macro is an attempt to simulate its environment in non-Arch distros.
dnl It is likely to  need periodic updating as Arch invents new ways to
dnl break your builds.
dnl
dnl Arch specifies extra compilation flags in: 
dnl 	/etc/makepkg.conf.
dnl A quick way to identify them might be at:
dnl 	https://wiki.parabola.nu/Makepkg
dnl
AC_DEFUN([AX_FORTIFY], [

AC_ARG_WITH([fortify],
  [AS_HELP_STRING([--with-fortify], [Enable Arch Linux-like Fortification (default is no)])],
  [with_fortify=$withval],
  [with_fortify=no])

AM_CONDITIONAL([WITH_FORTIFY], test "x$with_fortify" = "xyes")

AS_IF([test "x$with_fortify" = "xyes"], [
	AC_SUBST([FORTIFY_CXXFLAGS], ["$CXXFLAGS -mtune=generic -pipe -O2 -fstack-protector --param=ssp-buffer-size=4  -D_FORTIFY_SOURCE=2"])
	AC_SUBST([FORTIFY_LIBS], [])],
	AC_SUBST([FORTIFY_CXXFLAGS], [])
	AC_SUBST([FORTIFY_LIBS], []) )

])dnl AX_FORTIFY
