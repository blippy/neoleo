%module Ploppy

//https://stackoverflow.com/questions/7223437/how-to-create-a-tcl-function-with-optional-arguments-using-swig
//%rename("twice") tickle_twice;
%rename ("set-exit") set_exit;

%inline %{
//extern double twicely(double d);
//extern int Plop_Init(Tcl_Interp *interp0);
#include "tickle.h"
%}


%include "tickle.h"
