AC_PREREQ([2.69])
AC_INIT([neoleo], [4.0.1X], [alt.mcarter@gmail.com])
AC_CONFIG_SRCDIR([src/pcl.c])
AM_INIT_AUTOMAKE([foreign])


#PKG_PROG_PKG_CONFIG
#AC_PROG_RANLIB
AC_PROG_CC
AC_PROG_YACC
LT_INIT
#AX_PKG_SWIG([], [], [ AC_MSG_ERROR([SWIG is required to build..]) ])
#AX_PKG_SWIG()

AC_ARG_WITH(motif,
     [  --without-motif              do not use Motif, even if detected],
     [               case "${withval}" in
         y | ye | yes )      usemotif=yes ;;
         n | no )            usemotif=no ;;
         * )                 usemotif=yes ;;
         esac],
     [               usemotif=yes])

AM_CONDITIONAL(UseMotif, test x$usemotif = xyes)
if test "$usemotif" = "yes"; then
   AC_CHECK_HEADERS([Xbae/Matrix.h])
   AS_IF([test "x$ac_cv_header_Xbae_Matrix_h" != "xyes"],
         [AC_MSG_ERROR([Xbae not found; you must install it or use "--without-motif"])])
fi



AC_CONFIG_FILES([Makefile examples/Makefile src/Makefile])
AC_OUTPUT
