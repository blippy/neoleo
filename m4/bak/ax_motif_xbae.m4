AC_DEFUN([AX_MOTIF_XBAE], [
AC_ARG_WITH(motif,
     [  --with-motif       Compile using Motif],
     [               case "${withval}" in
         y | ye | yes )      usemotif=yes ;;
         n | no )            usemotif=no ;;
         * )                 usemotif=no ;;
         esac],
     [               usemotif=no])

AM_CONDITIONAL(UseMotif, test x$usemotif = xyes)
if test "$usemotif" = "yes"; then
   AC_CHECK_HEADERS([Xbae/Matrix.h])
   AS_IF([test "x$ac_cv_header_Xbae_Matrix_h" != "xyes"],
         [AC_MSG_ERROR([Xbae not found; avoid using "--with-motif"])])
fi

])dnl AX_MOTIF_XBAE
