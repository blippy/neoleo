AC_DEFUN([AX_MOTIF_XBAE], [
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

])dnl AX_MOTIF_XBAE
