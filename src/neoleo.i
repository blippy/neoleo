// my first attept
%module neoleo

%{
/* first go */
#include "neoleo_swig.h"
#include "basic.h"
//#include "global.h"
#include "io-term.h"
#include "mdi.h"
#include "mysql.h"
//#include "oleo_plot.h"


%}

%include "neoleo_swig.h"
//%include "basic.h"
//%include "global.h"
//%include "io-term.h"
%include "mdi.h"
%include "mysql.h"
//%include "oleo_plot.h"

// basic.h
//void read_file_and_run_hooks (FILE * fp, int ismerge, char * name);


// io-term.h
extern void InitializeGlobals(void);
void init_basics();
