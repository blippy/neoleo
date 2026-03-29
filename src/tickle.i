%module Ploppy

%include "cpointer.i"
%include "typemaps.i"
%include "std_string.i"

//%apply 
//https://stackoverflow.com/questions/7223437/how-to-create-a-tcl-function-with-optional-arguments-using-swig
//%rename("twice") tickle_twice;
//%newobject ploppy_get_cell; // because a char* is returned
//%newobject ploppy_get_cell_fmt; // Tcl must free

//%apply (std::string &IN) { (std::string &ref_string);}
//%apply std::string &INOUT { std::string &ref_inout_str };
//extern std::string	ploppy_test_tcl11(std::string& ref_inout_str); 
//%pointer_functions(char*, charp);
//%pointer_functions(std::string*, stringp);

//%apply char *OUTPUT { char *out_str};
//extern std::string	ploppy_test_tcl11(char* out_str);


//%typemap(out_str) std::string {
//    $result = Tcl_NewStringObj($1.c_str(), $1.length());
//}

//void blah(std::string* result);
//std::string	ploppy_test_tcl11(std::string* out_str);

%rename ("bind-key") ploppy_bind_key;
%rename ("clear-sheet") ploppy_clear_sheet;
%rename ("definitely-quit") ploppy_definitely_quit;
%rename ("display-curses") ploppy_display_curses;
%rename ("get-cell") ploppy_get_cell;
%rename ("get-cell-fmt") ploppy_get_cell_fmt;
%rename ("get-col-num") ploppy_get_col_num;
%rename ("get-row-num") ploppy_get_row_num;
%rename ("go") ploppy_go;
%rename ("hi-ploppy") ploppy_hi;
%rename ("insert-by-col") ploppy_insert_by_col;
%rename ("insert-by-row") ploppy_insert_by_row;
%rename ("insert-row") ploppy_insert_row;
%rename ("invoke-std-form") ploppy_invoke_std_form;
%rename ("load-oleo") ploppy_load_oleo;
%rename ("max-col") ploppy_max_col;
%rename ("max-row") ploppy_max_row;
%rename ("plog") ploppy_log;
%rename ("ploppy-life") ploppy_life;
%rename ("print-range") ploppy_print_range;
%rename ("replace-first-form") ploppy_replace_first_form;
%rename ("save-oleo-as") ploppy_save_oleo_as;
%rename ("set-cell") ploppy_set_cell;
%rename ("set-col-width") ploppy_set_col_width;
%rename ("set-col-precision") ploppy_set_col_prec;
%rename ("set-exit") set_exit;
%rename ("set-status") ploppy_set_status;
%rename ("test-binding") ploppy_test_binding;
%rename ("test-tcl11") ploppy_test_tcl11;

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
