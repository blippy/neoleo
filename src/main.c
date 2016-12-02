#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#include "io-term.h"


// dynamic linking taken from
// http://tldp.org/HOWTO/Program-Library-HOWTO/dl-libraries.html


int main(int argc, char **argv)
{
	//void *handle;
	//double (*cosine)(double);
	int (*main0)(int , char **);
	char *error;

	void *handle = dlopen ("libneoleo.so", RTLD_LAZY);
	//handle = dlopen ("libneoleo.so", RTLD_NOW);
	if (!handle) {
		fputs (dlerror(), stderr);
		exit(1);
	}

	//cosine = dlsym(handle, "main0");
	main0 = dlsym(handle, "main0");
	if ((error = dlerror()) != NULL)  {
		fputs(error, stderr);
		exit(1);
	}

	//printf ("%f\n", (*cosine)(2.0));
	int ret_status = (*main0)(argc, argv);
	dlclose(handle);

	return ret_status;
}
