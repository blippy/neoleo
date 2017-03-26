AC_DEFUN([AX_ASAN], [
AC_ARG_WITH(asan,
     [  --with-asan       Compile using sanitiser],
     [               case "${withval}" in
         y | ye | yes )      useasan=yes ;;
         n | no )            useasan=no ;;
         * )                 useasan=no ;;
         esac],
     [               useasan=no])

AM_CONDITIONAL(UseAsan, test x$useasan = xyes)

])dnl AX_ASAN
