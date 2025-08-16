# 2019-01-20 mcarter created

AC_DEFUN([AX_COVERAGE], [

AC_ARG_WITH([coverage],
  [AS_HELP_STRING([--with-coverage], [Enable code coverage (default is no)])],
  [with_coverage=$withval],
  [with_coverage=no])

AM_CONDITIONAL([WITH_COVERAGE], test "x$with_coverage" = "xyes")

AS_IF([test "x$with_coverage" = "xyes"], [
	AC_SUBST([COVERAGE_CXXFLAGS], ["--coverage"])
	AC_SUBST([COVERAGE_LDADD], ["--coverage"])],
	AC_SUBST([COVERAGE_CXXFLAGS], [])
	AC_SUBST([COVERAGE_LDADD], []) )

])dnl AX_COVERAGE
