/*
 * tickle.cc
 *
 *  Created on: 28 Oct 2025
 *      Author: pi
 */

// https://github.com/hpaluch-pil/tcl-cpp-example/blob/master/tcl_ex.cpp
// NB You need to add the exported functions to tickle.h

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

static Tcl_Interp *interp = nullptr;

//#define Ploppy_Init SWIG_init
extern "C" int Ploppy_Init(Tcl_Interp *interp);
//extern "C" int SWIG_init(Tcl_Interp *interp);
char* ploppy_string(const std::string& s);
char*  ploppy_get_cell_fmt(int r, int c);

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

char*  ploppy_get_cell_fmt(int r, int c)
{
	return ploppy_string(string_cell_formatted(r,c));
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
	oleo_write_file_as(path);
}

int ploppy_load_oleo(char* path)
{
	return oleo_read_file(path);
}

void ploppy_set_cell(int r, int c, char* formula)
{
	set_and_eval (r, c, formula, false);
}


char*  ploppy_get_cell(int r, int c)
{
	return ploppy_string(string_cell(r,c));
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

void tickle_eval_expr(const std::string& expr)
{
	Tcl_Eval(interp, expr.c_str()); // TODO error check and cleanup
}

void tickle_run_file(const std::string& path)
{
	Tcl_EvalFile(interp, path.c_str()); // TODO error check and cleanup
}

// a repl from stdin
void tickle_main()
{
	std::string cmd;
	while(std::getline(cin, cmd)) {
		int err = Tcl_Eval(interp, cmd.c_str());
		if ( err != TCL_OK ){
				fprintf(stderr,"Error calling Tcl_Eval(): %s\n",Tcl_GetStringResult(interp));
		}
	}
}

void tickle_init(char* argv0)
{
	interp = Tcl_CreateInterp(); // deleted by Tcl_DeleteInterp
	assert(interp);
	int ok = Ploppy_Init(interp);
	if(ok == TCL_ERROR) {
		puts("couldn't Ploppy_Init");
		return;
	}
}
