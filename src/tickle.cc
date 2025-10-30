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

using std::cout;
using std::endl;


#include <tcl.h>

static Tcl_Interp *interp = nullptr;


static int tcl_hi( ClientData dummy,                /* Not used. */
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
	return TCL_OK;
}


static int tickle_max_row (ClientData dummy,  Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
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

	Tcl_CreateObjCommand(interp, "life", 	tickle_life, NULL, NULL);
	Tcl_CreateObjCommand(interp, "max_col", tickle_max_col, NULL, NULL);
	Tcl_CreateObjCommand(interp, "max_row", tickle_max_row, NULL, NULL);
	Tcl_CreateObjCommand(interp, "tcl_hi", 	tcl_hi, NULL, NULL);


#if 0
	int err = Tcl_Eval(interp, "tcl_hi");
	assert(err == TCL_OK);
	fprintf(stderr,"string result: %s\n",Tcl_GetStringResult(interp));
	Tcl_FreeResult(interp);
#endif
	//Tcl_FreeResult(interp);

}
