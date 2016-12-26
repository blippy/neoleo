/* a test driver, mainly to see where Tcl crashes. It could also be used 
 * to run experiments generally
 */

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#include "io-term.h"
#include "neoleo_swig.h"



int 
main_neot(int argc, char **argv)
{
	//void *handle;
	//double (*cosine)(double);
	int (*neot_test0)(int , char **);
	char *error;

	void *handle = dlopen ("libneoleo.so", RTLD_LAZY);
	if (!handle) {
		fputs (dlerror(), stderr);
		exit(1);
	}

	neot_test0 = dlsym(handle, "neot_test0");
	if ((error = dlerror()) != NULL)  {
		fputs(error, stderr);
		exit(1);
	}

	int ret_status = (*neot_test0)(argc, argv);
	dlclose(handle);

	return ret_status;
}
