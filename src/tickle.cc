/*
 * tickle.cc
 *
 *  Created on: 28 Oct 2025
 *      Author: pi
 */

// https://github.com/hpaluch-pil/tcl-cpp-example/blob/master/tcl_ex.cpp

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

#include "oleofile.h"
#include "sheet.h"
#include "spans.h"

static Tcl_Interp *interp = nullptr;

//#define Ploppy_Init SWIG_init
extern "C"
int Ploppy_Init(Tcl_Interp *interp);
//extern "C" int SWIG_init(Tcl_Interp *interp);


char*  ploppy_get_cell(int r, int c)
{
	string s{string_cell(r, c)};
	char *return_string = (char*) malloc(s.size()+1);
	if(return_string == 0) return 0; // oops
	strcpy(return_string, s.c_str());
	return return_string;
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
#if 0
	  void* handle = dlopen("ploppy.so", RTLD_LAZY);
	  if(!handle) {
		  puts("couldn't open library");
		  return 1;
	  } else {
		  puts("Found the library");
	  }
#endif

	//Tcl_Interp* interp;

	//Tcl_FindExecutable(argv0);
	interp = Tcl_CreateInterp(); // deleted by Tcl_DeleteInterp
	assert(interp);
	int ok = Ploppy_Init(interp);
	if(ok == TCL_ERROR) {
		puts("couldn't Ploppy_Init");
		return;
	}

#if 0
	puts("aq");
	ok = Tcl_Eval(interp, "puts [twicely x3]");
	if(ok == TCL_ERROR) {
		puts("Error orrcurred");
		puts(interp->resultDontUse);
	}
	puts("az");
	//SWIG_init(interp);
	//Tcl_DeleteInterp(interp);
#endif

	// TODO possibly delete interpreter
}

#if 0
#if 0
static std::string string_cell (CELLREF row, CELLREF col)
{
	CELL* cp = find_cell (row, col);
	if(!cp) return "";

	value_t val = cp->get_value_2019();
	if(std::holds_alternative<std::monostate>(val)) return "";
	if(auto v = std::get_if<std::string>(&val)) return *v;
	if(auto v = std::get_if<num_t>(&val)) 	return flt_to_str(*v);
	if(auto v = std::get_if<bool_t>(&val)) 	return bool_name(*v);
	if(auto v = std::get_if<err_t>(&val)) 	return ename_desc[v->num];

	throw std::logic_error("Unhandled variant type in cell_value_string");
}
#endif


static int tickle_get_cell (ClientData dummy,  Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	//cout << "num args " << objc << endl;

	if(objc != 3 && objc != 4) {
		Tcl_WrongNumArgs(interp, 0, objv, "?-fmt? row col");
		return TCL_ERROR;
	}

	int r, c, status;
	//int row_idx = 1, col_idx = 2;
	char* fmtp = Tcl_GetString(objv[1]); // maybe we want a formatted output
	if("-fmt"s == fmtp) {
		// print a formatted version
		//row_idx = 2;
		//col_idx = 3;
		status = Tcl_GetIntFromObj(interp, objv[2], &r);
		if(status != TCL_OK) cerr << "get-cell: couldn't extract row" <<endl;
		status = Tcl_GetIntFromObj(interp, objv[3], &c);
		if(status != TCL_OK) cerr << "get-cell: couldn't extract col" <<endl;
		int w = get_width(c);
		CELL *cp = find_cell(r, c);
		std::string text;
		if (cp == 0) {
			text = pad_left("", w);
		} else {
			enum jst just = cp->get_cell_jst();
			text = string_cell(cp);
			text = pad_jst(text, w, just);
			//cout << txt;
		}
		Tcl_SetObjResult(interp, Tcl_NewStringObj(text.c_str(), text.size()));
		return TCL_OK;
	}


	// we just want a raw value
	status = Tcl_GetIntFromObj(interp, objv[1], &r);
	if(status != TCL_OK) cerr << "get-cell: couldn't extract row" <<endl;
	status = Tcl_GetIntFromObj(interp, objv[2], &c);
	if(status != TCL_OK) cerr << "get-cell: couldn't extract col" <<endl;
	CELL *cp = find_cell(r, c);
	std::string str{string_cell(cp)};
	//cout << "r " << r << " c " << c << " result " << str << endl;
	Tcl_SetObjResult(interp, Tcl_NewStringObj(str.c_str(), str.size()));
	return TCL_OK;
}

static int tickle_load_oleo (ClientData dummy,  Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	//extern void oleo_read_file (FILE *fp);
	//Tcl_SetObjResults(interp)
	//return max_col();
	//cout << "tickle_get_cell called " << endl;
	int r, c, status;
	char* str = Tcl_GetString(objv[1]);
	//printf("tickle_load_oleo: filename:%s\n", str);
	// TODO set filename
	FILE* fp = fopen(str, "r");
	oleo_read_file(fp);
	fclose(fp);

#if 0
	if(status != TCL_OK) cerr << "get-cell: couldn't extract row" <<endl;
	status = Tcl_GetIntFromObj(interp, objv[2], &c);
	if(status != TCL_OK) cerr << "get-cell: couldn't extract col" <<endl;
	CELL *cp = find_cell(r, c);
	std::string str{string_cell(cp)};
	//cout << "r " << r << " c " << c << " result " << str << endl;
	Tcl_SetObjResult(interp, Tcl_NewStringObj(str.c_str(), str.size()));
#endif
	return TCL_OK;
}

static int tickle_hi( ClientData dummy,                /* Not used. */
	    Tcl_Interp *interp,                /* Current interpreter. */
	    int objc,                        /* Number of arguments. */
	    Tcl_Obj *const objv[])        /* Argument objects. */
{
	cout << "Tcl says 'hi'" << endl;
	//Tcl_SetObjResult(interp, NULL); // I think returning nULL causes segfault
	return TCL_OK;
}

static int tickle_life (ClientData dummy,  Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	//Tcl_SetObjResults(interp)
	//return max_col();
	Tcl_SetObjResult(interp, Tcl_NewIntObj(42) );
	return TCL_OK;
}

static int tickle_max_col (ClientData dummy,  Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	//Tcl_SetObjResults(interp)
	//return max_col();
	Tcl_SetObjResult(interp, Tcl_NewIntObj(max_col()) );
	return TCL_OK;
}


static int tickle_max_row (ClientData dummy,  Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	//return max_row();
	Tcl_SetObjResult(interp, Tcl_NewIntObj(max_row()) );
	return TCL_OK;
}

static int tickle_save_oleo (ClientData dummy,  Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	//cout << "tickle_save_oleo objc = " << objc <<endl;
	if(objc>1) {
		char* path = Tcl_GetString(objv[1]);
		//cout << "saving file as " << path <<endl;
		oleo_write_file_as(path);
	} else {
		oleo_write_file();
	}
	return TCL_OK;
}

static int tickle_set_cell (ClientData dummy,  Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	//Tcl_SetObjResults(interp)
	//return max_col();
	//cout << "tickle_get_cell called " << endl;
	int r, c, status;
	status = Tcl_GetIntFromObj(interp, objv[1], &r);
	if(status != TCL_OK) cerr << "get-cell: couldn't extract row" <<endl;
	status = Tcl_GetIntFromObj(interp, objv[2], &c);
	if(status != TCL_OK) cerr << "get-cell: couldn't extract col" <<endl;
	char* formula = Tcl_GetString(objv[3]); // cell formula
	//status = Tcl_GetStringFromObj(interp, objv[2], &c);
	set_and_eval (r, c, formula, false);
	//CELL *cp = find_or_make_cell(r, c);
	//std::string str{print_cell(cp)};
	//cout << "r " << r << " c " << c << " result " << str << endl;
	//Tcl_SetObjResult(interp, Tcl_NewStringObj(str.c_str(), str.size()));
	return TCL_OK;
}


static int tickle_set_exit (ClientData dummy,  Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	//cout << "number of args: " << objc << endl;
	//assert(objc == 1); // it's actually 2
	Tcl_GetIntFromObj(interp, objv[1], &exit_value);
	//cout << "tickle_set_exit with " << exit_value << endl;
	//exit(exit_value);
	//return max_row();
	return TCL_OK;
}

#if 0
static int tickle_exit (ClientData dummy,  Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	//cout << "number of args: " << objc << endl;
	//assert(objc == 1); // it's actually 2
	Tcl_GetIntFromObj(interp, objv[1], &exit_value);
	//cout << "tickle_set_exit with " << exit_value << endl;
	//exit(exit_value);
	//return max_row();
	return TCL_OK;
}
#endif

double twicely(double d)
{
	return 2.0*d;
}

// runs TCL commands in tclCommands, returns EXIT_SUCESS or EXIT_FAILURE
static int Ex_RunTcl(const char *tclCommands){
	int rc = EXIT_FAILURE;
	int err = TCL_OK;



#if 0
	err = Ex_ExtendTcl(interp);
	if ( err != TCL_OK ){
		fprintf(stderr,"Error calling Ex_ExtendTcl(): %s\n",Tcl_GetStringResult(interp));
		goto exit2;
	}
	err = Tcl_Eval(interp,tclCommands);
	if ( err != TCL_OK ){
		fprintf(stderr,"Error calling Tcl_Eval(): %s\n",Tcl_GetStringResult(interp));
		goto exit2;
	}

	rc = EXIT_SUCCESS;

	Tcl_FreeResult(interp);
#endif


	exit0:
	return rc;
}

void atexit_handler_1()
{
    //std::cout << "Shutting down Tcl\n";
	Tcl_DeleteInterp(interp);
	interp = nullptr;
	Tcl_Finalize();
}

//extern "C"
//int _wrap_twice(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);

// use both by embedded and extended
void tickle_create_commands()
{
	Tcl_CreateObjCommand(interp, "get-cell",	tickle_get_cell, NULL, NULL);
	Tcl_CreateObjCommand(interp, "hi", 			tickle_hi, NULL, NULL);
	Tcl_CreateObjCommand(interp, "life", 		tickle_life, NULL, NULL);
	Tcl_CreateObjCommand(interp, "load-oleo",	tickle_load_oleo, NULL, NULL);
	Tcl_CreateObjCommand(interp, "max-col", 	tickle_max_col, NULL, NULL);
	Tcl_CreateObjCommand(interp, "max-row", 	tickle_max_row, NULL, NULL);
	Tcl_CreateObjCommand(interp, "save-oleo", 	tickle_save_oleo, NULL, NULL);
	Tcl_CreateObjCommand(interp, "set-exit", 	tickle_set_exit, NULL, NULL);
	Tcl_CreateObjCommand(interp, "set-cell", 	tickle_set_cell, NULL, NULL);
	Tcl_CreateObjCommand(interp, "oleo-hi", 	tickle_hi, NULL, NULL);

//	Tcl_CreateObjCommand(interp, "twice", 	_wrap_twice, NULL, NULL);

//	Tcl_CreateCommand(interp, "oleo-hi", 	tickle_hi, NULL, NULL);

}
// argv0 is argv[0] from main()
void tickle_init(char* argv0)
{
	//malloc(600);
	const int result_1 = std::atexit(atexit_handler_1);
	assert(!result_1);

	Tcl_FindExecutable(argv0);
	interp = Tcl_CreateInterp(); // deleted by Tcl_DeleteInterp
	assert(interp);

	tickle_create_commands();


#if 0
	int err = Tcl_Eval(interp, "tickle_hi");
	assert(err == TCL_OK);
	fprintf(stderr,"string result: %s\n",Tcl_GetStringResult(interp));
	Tcl_FreeResult(interp);
#endif
	//Tcl_FreeResult(interp);

}



// 25/11
// For extending Tcl, see:
// https://deepwiki.com/tcltk/tcl/3.2-creating-tcl-extensions
// This is for use in creating liboleo.so for extension
extern "C" int Oleo_Init(Tcl_Interp *interp0) {

	interp = interp0;
    /* Initialize the stubs mechanism */
    if (Tcl_InitStubs(interp0, TCL_VERSION, 0) == NULL)
        return TCL_ERROR;

    tickle_create_commands();

    /* Register the package */
    return Tcl_PkgProvide(interp, "Oleo", "0.1"); // TODO use project version rather than 0.1
}

// 25/11
// Similar to Oleo_Init(), but for Safe interpreters
extern "C" int Oleo_SafeInit(Tcl_Interp *interp0) {
    /* Safe interpreters can use this extension too */
    return Oleo_Init(interp0);
}

int xPlop_Init(Tcl_Interp *interp0)
{
	return Oleo_Init(interp0)	;
}

#endif
