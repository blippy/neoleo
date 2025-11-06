%module Ploppy

//https://stackoverflow.com/questions/7223437/how-to-create-a-tcl-function-with-optional-arguments-using-swig
//%rename("twice") tickle_twice;
%rename("get-cell") ploppy_get_cell;
%newobject ploppy_get_cell; // because a char* is returned
%rename ("set-exit") set_exit;
%rename ("hi-ploppy") ploppy_hi;
%rename ("max-col") ploppy_max_col;
%rename ("max-row") ploppy_max_row;
%rename ("ploppy-life") ploppy_life;
%rename ("load-oleo") ploppy_load_oleo;
%rename ("set-cell") ploppy_set_cell;

/*
%{
//#include "neotypes.h"
using CELLREF = uint16_t;
void set_exit (int code);
void ploppy_hi( );
int ploppy_life();
CELLREF max_col();
CELLREF max_row();
%}
*/

%inline %{
#include "tickle.h"
%}



%include "tickle.h"

/* 
void set_exit (int code);
void ploppy_hi( );
int ploppy_life();
CELLREF max_col();
CELLREF max_row();
*/