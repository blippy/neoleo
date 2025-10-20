dnl 25/10 added
dnl I decided that forms will likely work, so I currently want them
dnl enabled by default. You can optionally disable them
AC_DEFUN([AX_FORM], [

AC_ARG_WITH([form],
  AS_HELP_STRING([--without-form], 
    [Disable ncurses forms (in case of trouble on some systems)]),
    [],
    [with_form=yes])
 
AC_MSG_NOTICE([checking if forms required ... $with_form])

AS_IF([test "x$with_form" != "xno"], [
  dnl do the stuff to enable forms
  PKG_CHECK_MODULES(FORM, formw)
  AC_SUBST([FORM_LIBS])
  AC_SUBST([FORM_CXXFLAGS], ["-DUSE_FORM"])
  dnl AC_SUBST([FORM_LIBS], ["-lformw"])
])



])dnl AX_FORM
