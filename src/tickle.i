%module Ploppy

//https://stackoverflow.com/questions/7223437/how-to-create-a-tcl-function-with-optional-arguments-using-swig
//%rename("twice") tickle_twice;
%rename("get-cell") ploppy_get_cell;
%newobject ploppy_get_cell; // because a char* is returned
%rename ("get-cell-fmt") ploppy_get_cell_fmt;
%newobject ploppy_get_cell_fmt; // Tcl must free

%rename ("set-exit") set_exit;
%rename ("hi-ploppy") ploppy_hi;
%rename ("go") ploppy_go;
%rename ("insert-by-col") ploppy_insert_by_col;
%rename ("insert-by-row") ploppy_insert_by_row;
%rename ("insert-row") ploppy_insert_row;
%rename ("max-col") ploppy_max_col;
%rename ("max-row") ploppy_max_row;
%rename ("ploppy-life") ploppy_life;
%rename ("load-oleo") ploppy_load_oleo;
%rename ("save-oleo-as") ploppy_save_oleo_as;
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