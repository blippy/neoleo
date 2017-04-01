AC_DEFUN([AX_X], [
AC_ARG_WITH(x,
     [  --with-x       Compile using X11],
     [               case "${withval}" in
         y | ye | yes )      UseX=yes ;;
         n | no )            UseX=no ;;
         * )                 UseX=no ;;
         esac],
     [               UseX=no])

AM_CONDITIONAL(UseX, test x$UseX = xyes)

])dnl AX_X
