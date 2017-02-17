AC_PREREQ([2.69])
AC_INIT([neoleo], [4.0.1], [alt.mcarter@gmail.com])
AC_CONFIG_SRCDIR([src/pcl.c])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIRS([m4])


AX_MOTIF_XBAE
AC_PROG_CC
AC_PROG_CXX
AC_PROG_YACC

#AC_ARG_WITH(motif,
#     [  --without-motif              do not use Motif, even if detected],
#     [               case "${withval}" in
#         y | ye | yes )      usemotif=yes ;;
#         n | no )            usemotif=no ;;
#         * )                 usemotif=yes ;;
#         esac],
#     [               usemotif=yes])

#AM_CONDITIONAL(UseMotif, test x$usemotif = xyes)
#if test "$usemotif" = "yes"; then
#   AC_CHECK_HEADERS([Xbae/Matrix.h])
#   AS_IF([test "x$ac_cv_header_Xbae_Matrix_h" != "xyes"],
#         [AC_MSG_ERROR([Xbae not found; you must install it or use "--without-motif"])])
#fi



AC_CONFIG_FILES([Makefile doc/Makefile examples/Makefile src/Makefile])
AC_OUTPUT
