dnl 25/10 added
dnl set up stuff necessary to create texingfos
AC_DEFUN([AX_INFO], [


dnl AC_ARG_WITH([debug],
dnl   [AS_HELP_STRING([--with-debug], [Enable debug (default is no)])],
dnl   [with_debug=$withval],
dnl   [with_debug=no])


dnl AM_CONDITIONAL([WITH_DEBUG], [test "x$with_debug" = "xyes"])

dnl AS_IF([test "x$with_debug" = "xyes"], [
dnl   dnl AC_SUBST([DEBUG_CXXFLAGS], ["-O0 -g"])
dnl   AC_SUBST([CXXFLAGS], ["$CXXFLAGS -O0 -g"])
dnl ])

dnl AC_MSG_NOTICE([checking if debugging required ... $with_debug])

dnl AC_CHECK_PROG(HAVE_TEXI2HTML, texi2html, yes)
dnl AC_MSG_CHECKING([for texi2html])
dnl if test -z "$HAVE_TEXI2HTML"; then
dnl 	AC_MSG_RESULT([none found])
dnl 	AC_MSG_ERROR([Could not find texi2html. May apt install texi2html])
dnl else
dnl 	AC_MSG_RESULT([$HAVE_TEXI2HTML])
dnl fi
dnl AM_CONDITIONAL(HaveTexi2Html, test x$HAVE_TEXI2HTML = xyes)


AC_CHECK_PROG(HAVE_MAKEINFO, makeinfo, yes)
dnl AC_MSG_CHECKING([for makeinfo])
if test -z "$HAVE_MAKEINFO"; then
dnl	AC_MSG_RESULT([none found])
	AC_MSG_ERROR([Could not find makeinfo. May apt install texinfo])
dnl else
dnl	AC_MSG_RESULT([$HAVE_MAKEINFO])
fi

AC_CHECK_PROG(HAVE_TEX, tex, yes)
dnl AC_MSG_CHECKING([for tex])
if test -z "$HAVE_TEX"; then
dnl	AC_MSG_RESULT([none found])
	AC_MSG_ERROR([Could not find tex. Arch users should install texlive-bin])
dnl else
dnl	AC_MSG_RESULT([$HAVE_TEXI_TO_ANY])
fi

AC_CHECK_PROG(HAVE_TEXI_TO_ANY, texi2any, yes)
dnl AC_MSG_CHECKING([for texi2any])
if test -z "$HAVE_TEXI_TO_ANY"; then
dnl	AC_MSG_RESULT([none found])
	AC_MSG_ERROR([Could not find texi2any. Should probably be installed alongside makeinfo])
dnl else
dnl	AC_MSG_RESULT([$HAVE_TEXI_TO_ANY])
fi

dnl AM_CONDITIONAL(HaveMakeinfo, test x$HAVE_MAKEINFO = xyes)


])dnl AX_DEBUG
