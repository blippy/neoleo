AC_DEFUN([AX_FORM], [


AC_ARG_WITH([form],
  [AS_HELP_STRING([--with-form], 
    [Build with forms (default is no)
    Displaying forms seems to cause problems on some systems])],
  [with_form=$withval],
  [with_form=no])


AM_CONDITIONAL([WITH_FORM], [test "x$with_form" = "xyes"])

AS_IF([test "x$with_form" = "xyes"], [
  PKG_CHECK_MODULES(FORM, formw)
  AC_SUBST([FORM_LIBS])
  AC_SUBST([FORM_CXXFLAGS], ["-DUSE_FORM"])
  dnl AC_SUBST([FORM_LIBS], ["-lformw"])
])

AC_MSG_NOTICE([checking if forms required ... $with_form])

])dnl AX_FORM
