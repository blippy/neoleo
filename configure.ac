AC_PREREQ([2.69])
AC_INIT([neoleo], [3.0], [alt.mcarter@gmail.com])
AC_CONFIG_SRCDIR([src/pcl.c])
AM_INIT_AUTOMAKE([foreign])

AC_CHECK_HEADERS([Xbae/Matrix.h])
AM_CONDITIONAL([HAVE_XBAE], [test "x$ac_cv_header_Xbae_Matrix_h" = "xyes"])
#AM_CONDITIONAL([HAVE_MOTIF], [test "x$ac_cv_header_Xbae_Matrix_h" = "xyes"])

AC_PROG_RANLIB
AC_PROG_CC
AC_PROG_YACC



AC_CONFIG_FILES([Makefile lib/Makefile src/Makefile])
AC_OUTPUT
