/*
 * tickle.cc
 *
 *  Created on: 28 Oct 2025
 *      Author: pi
 */

// https://github.com/hpaluch-pil/tcl-cpp-example/blob/master/tcl_ex.cpp

#include <cstdlib>
#include <iostream>

using std::cout;
using std::endl;


#include <tcl.h>


// runs TCL commands in tclCommands, returns EXIT_SUCESS or EXIT_FAILURE
static int Ex_RunTcl(const char *tclCommands){
	int rc = EXIT_FAILURE;
	int err = TCL_OK;

	Tcl_Interp *interp = Tcl_CreateInterp();
	if (interp == NULL){
		fprintf(stderr,"Tcl_CreateInterp() returned NULL");
		goto exit0;
	}

#if 0
	err = Ex_ExtendTcl(interp);
	if ( err != TCL_OK ){
		fprintf(stderr,"Error calling Ex_ExtendTcl(): %s\n",Tcl_GetStringResult(interp));
		goto exit2;
	}
#endif
	err = Tcl_Eval(interp,tclCommands);
	if ( err != TCL_OK ){
		fprintf(stderr,"Error calling Tcl_Eval(): %s\n",Tcl_GetStringResult(interp));
		goto exit2;
	}

	rc = EXIT_SUCCESS;

	exit2:
	Tcl_FreeResult(interp);

//	exit1:
	Tcl_DeleteInterp(interp);

	exit0:
	return rc;
}

void atexit_handler_1()
{
    std::cout << "At exit #1\n";
}

// argv0 is argv[0] from main()
void tickle_init(char* argv0)
{
	//puts("tickle init called");
	//cout << "herllo " << endl;
	//return;
//	int rc = EXIT_SUCCESS;
	const int result_1 = std::atexit(atexit_handler_1);

	Tcl_FindExecutable(argv0);
	int rc = Ex_RunTcl("puts \"Hello, world\"");
	rc = Ex_RunTcl("set q 20 ; puts $q");
	//rc = Ex_RunTcl("puts $q");
	//cout << endl;
	//Tcl_Finalize();
}
