/*
 * tickle.cc
 *
 *  Created on: 28 Oct 2025
 *      Author: pi
 */

// https://github.com/hpaluch-pil/tcl-cpp-example/blob/master/tcl_ex.cpp
// NB You need to add the exported functions to tickle.h

/* NB 26/3 memory cleanup
 * Tcl_DeleteInterp(interp) cleans up the interpreter before exiting.
 * Tcl_Finalize() deletes in-memory caches. You must promise that you will not call more Tcl API functions in the current process.
 */

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using namespace std::literals;
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;


#include <tcl.h>

#include "neotypes.h"
#include "oleofile.h"
#include "sheet.h"
#include "spans.h"
#include "wsht.hh"

#define set_width the_wsht.set_width
#define wsh the_wsht


static Tcl_Interp *interp = nullptr;

//#define Ploppy_Init SWIG_init
extern "C" int Ploppy_Init(Tcl_Interp *interp);
//extern "C" int SWIG_init(Tcl_Interp *interp);
char* ploppy_string(const std::string& s);
std::string  ploppy_get_cell_fmt(int r, int c);
void tickle_eval_expr(const std::string& expr);


void ploppy_set_col_width(int col, int width)
{
	set_width(col, width);
}

void ploppy_set_col_prec(int col, int prec)
{
	extern void set_column_prec(int, int);
	set_column_prec(col, prec);
}


// 26/3
void ploppy_clear_sheet()
{
	extern void clear_spreadsheet();
	clear_spreadsheet();
}

// 26/3 created
void ploppy_definitely_quit()
{
	Global_definitely_quit = true;
}

// 26/3 created
int ploppy_get_col_num()
{
	return cucol;
}


// 26/3 created
int ploppy_get_row_num()
{
	return curow;
}

// 26/3 created
std::string ploppy_invoke_std_form(const std::string& desc, const std::string& text_field)
{
	extern bool invoke_std_form(const std::string& desc, std::string& text_field);
	std::string input{text_field};
	bool ok = invoke_std_form(desc, input);
	std::string res = ok ? "1"s : "0"s;
	res += input;
	return res;
}

// 26/3 created
void ploppy_display_curses()
{
	extern void curses_main();
	curses_main();
}

// 26/3 created
void ploppy_log (char* str)
{
	log(str);
}

// 26/3 created
void ploppy_set_status (const char* status)
{
	set_status(status);
}

// 26/3 created
void ploppy_test_binding()
{
	test_status();
}
// 26/3 created
void ploppy_bind_key (char k, const std::string&  str_to_interpret)
{
	//string s{str_to_interpret};
	bind_char(k, str_to_interpret);
}

// 26/3
// search spreadsheet. Replace first instance of TARGET with formula WITH
// TARGET is assumed to be a formula
// returns 0 if successful, 1 otherwise
int ploppy_replace_first_form (const char* target, const char* with)
{
	for(CELL* cp: the_cells) {
		if(streq(cp->get_formula_text().c_str(), target)) {
			cp->set_and_eval(with);
			return 0;
		}
	}
	return 1;
}

// 25/11
static void set_cell_input_1 (CELLREF r, CELLREF c, const string& formula)
{
	curow = r;
	cucol = c;
	set_and_eval(r, c, formula, true);
}


// 25/11
void ploppy_print_range (int r_lo, int c_lo, int r_hi, int c_hi)
{
	for(auto r = r_lo; r <= r_hi; r++) {
		for(auto c = c_lo; c <= c_hi; c++) {
			cout << ploppy_get_cell_fmt(r, c);
			if(c<c_hi) cout << " ";
		}
		cout << endl;
	}

}

// 25/11
void ploppy_insert_by_row()
{
	string line;
	while(getline(cin, line)) {
		if(line == ";") { curow++; 	cucol=1; continue;}
		if(line == "")  { cucol++; continue;}
		if(line == ".") return;
		if(line.starts_with('#')) continue;
		set_cell_input_1(curow, cucol, line);
		cucol++;
	}
}


// 25/11 Added
void ploppy_insert_row()
{
	insert_row_above(curow);
}

// 25/11 Added
void ploppy_go(int r, int c)
{
	curow = r;
	cucol = c;
}


// 25/11 Added
void ploppy_insert_by_col()
{
	//log_func();
	string line;
	while(getline(cin, line)) {
		if(line == ";") { cucol++; 	curow=1; continue;}
		if(line == "")  { curow++; continue;}
		if(line == ".") return;
		if(line.starts_with('#')) continue;
		set_cell_input_1(curow, cucol, line);
		curow++;
	}
}

std::string  ploppy_get_cell_fmt(int r, int c)
//char*  ploppy_get_cell_fmt(int r, int c)
{
	//return ploppy_string(string_cell_formatted(r,c));
	return string_cell_formatted(r,c);
}


// returns a malloced string for use by Tcl
// Tcl is responsible for deleting
char* ploppy_string(const std::string& s)
{
	//string s{string_cell(r, c)};
	char *return_string = (char*) malloc(s.size()+1);
	if(return_string == 0) return 0; // oops
	strcpy(return_string, s.c_str());
	return return_string;
}

void ploppy_save_oleo_as(char* path)
{
	oleo_write_file_as(wsh, path);
}

int ploppy_load_oleo(char* path)
{
	return oleo_read_file(path);
}

void ploppy_set_cell(int r, int c, const std::string& formula )
{
	set_and_eval (r, c, formula, false);
}


std::string  ploppy_get_cell(int r, int c)
{
	//std:;string str = string_cell(r,c);
	//return ploppy_string(str);
	return string_cell(r,c);
}


int ploppy_max_col()
{
	return max_col();
}
int ploppy_max_row()
{
	return max_row();
}

int ploppy_life()
{
	return 42;
}


// exported as hi-ploppy
void ploppy_hi( )
{
	cout << "Ploppy Tcl says 'hi'" << endl;
}

void set_exit (int code)
{
	exit_value = code;
}


void tickle_run_file(const std::string& path)
{
	Tcl_EvalFile(interp, path.c_str());
}

// a repl from stdin
void tickle_main()
{

#if 0
	log("tickle_main called");
const char *repl = R"(
proc input { buffer ch } {
    if { [gets $ch line] != -1 } {
        append buffer "$line\n"
        if { [info complete $buffer] } {
            if { $buffer ne "\n" } {
                catch {eval uplevel #0 [list $buffer]} res
                if { [string length $res] } {
                    puts $res
                }
            }
            puts -nonewline "% "
            flush stdout
            fileevent stdin readable [list input "" stdin]
        } else {
            fileevent stdin readable [list input $buffer stdin]
        }
    } else {
        set ::forever 1
    }
    
}

puts -nonewline "% "
flush stdout
fileevent stdin readable [list input "" stdin]

vwait forever

exit
)";


	Tcl_Eval(interp, repl);
	return;
#else
	std::string cmd;
	while(std::getline(cin, cmd)) {
		// 26/3 copying a string doesn't seem necessary
		//char *str = ploppy_string(cmd);
		//assert(str);
		//strcpy(str, cmd.c_str());
		//int err = Tcl_Eval(interp, str);
		int err = Tcl_Eval(interp, cmd.c_str());
		if ( err != TCL_OK ){
				fprintf(stderr,"Error calling Tcl_Eval(): %s\n",Tcl_GetStringResult(interp));
		}
	}
#endif
}

// 26/3 created
void tickle_atexit ()
{
	Tcl_DeleteInterp(interp);
	Tcl_Finalize();
	//log("tickle_atexit called");
}

void tickle_init(char* argv0)
{
	//log("tickle_init called");
	int at = std::atexit(tickle_atexit);
	assert(at == 0);
	interp = Tcl_CreateInterp(); // deleted by Tcl_DeleteInterp
	assert(interp);
	Tcl_FindExecutable(argv0); // apparently necessary for paths
	int ok = Ploppy_Init(interp);
	if(ok == TCL_ERROR) {
		puts("couldn't Ploppy_Init");
		return;
	}
	tickle_eval_expr("plog {hello from tickle_init}");
	tickle_eval_expr("lappend auto_path /usr/share/tcltk/tcllib1.21");
}

void tickle_eval_expr(const std::string& expr)
{
	//log("tickle_eval_expr", expr, " with interp ", interp);
	if(!interp) tickle_init(0); // 26/3 can happen if it's a script is being called but not through main(), but via libploppy.so
	Tcl_Eval(interp, expr.c_str());
	//log("tickle_eval_expr exiting");
}

