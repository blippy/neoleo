/*
 * tickle.cc
 *
 *  Created on: 28 Oct 2025
 *      Author: pi
 */

// https://github.com/hpaluch-pil/tcl-cpp-example/blob/master/tcl_ex.cpp

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::endl;


#include <tcl.h>

#include "oleofile.h"
#include "sheet.h"

static Tcl_Interp *interp = nullptr;

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
	//Tcl_SetObjResults(interp)
	//return max_col();
	//cout << "tickle_get_cell called " << endl;
	int r, c, status;
	status = Tcl_GetIntFromObj(interp, objv[1], &r);
	if(status != TCL_OK) cerr << "get-cell: couldn't extract row" <<endl;
	status = Tcl_GetIntFromObj(interp, objv[2], &c);
	if(status != TCL_OK) cerr << "get-cell: couldn't extract col" <<endl;
	CELL *cp = find_cell(r, c);
	std::string str{print_cell(cp)};
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
	std::string str{print_cell(cp)};
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
	cout << "number of args: " << objc << endl;
	//assert(objc == 1);
	Tcl_GetIntFromObj(interp, objv[1], &exit_value);
	cout << "tickle_set_exit with " << exit_value << endl;
	//exit(exit_value);
	//return max_row();
	return TCL_OK;

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


void tickle_run_file(const std::string& path)
{
	Tcl_EvalFile(interp, path.c_str()); // TODO error check and cleanup
}

void atexit_handler_1()
{
    //std::cout << "Shutting down Tcl\n";
	Tcl_DeleteInterp(interp);
	interp = nullptr;
	Tcl_Finalize();
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

	Tcl_CreateObjCommand(interp, "get-cell",	tickle_get_cell, NULL, NULL);
	Tcl_CreateObjCommand(interp, "hi", 			tickle_hi, NULL, NULL);
	Tcl_CreateObjCommand(interp, "life", 		tickle_life, NULL, NULL);
	Tcl_CreateObjCommand(interp, "load-oleo",	tickle_load_oleo, NULL, NULL);
	Tcl_CreateObjCommand(interp, "max-col", 	tickle_max_col, NULL, NULL);
	Tcl_CreateObjCommand(interp, "max-row", 	tickle_max_row, NULL, NULL);
	Tcl_CreateObjCommand(interp, "set-exit", 	tickle_set_exit, NULL, NULL);
	Tcl_CreateObjCommand(interp, "set-cell", 	tickle_set_cell, NULL, NULL);


#if 0
	int err = Tcl_Eval(interp, "tickle_hi");
	assert(err == TCL_OK);
	fprintf(stderr,"string result: %s\n",Tcl_GetStringResult(interp));
	Tcl_FreeResult(interp);
#endif
	//Tcl_FreeResult(interp);

}
