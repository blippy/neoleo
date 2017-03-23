#include <stdio.h>

#include "atldef.h"
#include "atoleo.h"
#include "basic.h"
#include "cmd.h"
#include "ref.h"

//#ifdef __cplusplus
//extern "C" {
//#endif

/*
prim p4eval()
{
	Sl(1);
	char *cmd = (char *)S0;
	Pop;

	atl_eval(cmd);
}
*/

prim p4life()
{
	char str[] = "42";
	new_value(curow, cucol, str);
}

char * 
get_formula(int r, int c)
{
        CELL *cp = find_cell(r, c);
        return decomp(r, c, cp);
}

prim p4prform()
{
	int r = curow, c = cucol;
	printf("Formula at (%d,%d) is:%s\n", r, c, get_formula(r,c));
	decomp_free();
}

prim p4xcmd()
{
	Sl(1);
        char *cmd = (char *)S0;
        Pop;
	execute_command(cmd);
}

static struct primfcn oleop[] = {
	//{"04EVAL",	p4eval},
	{"04LIFE",	p4life},
	{"04PRFORM",	p4prform},
	{"04XCMD",	p4xcmd},
	{NULL,		(codeptr) 0 }
};

void
init_atoleo()
{
	atl_init();
	atl_primdef(oleop);
}

void
forth_repl()
{
	char t[132];
	while(printf("-> "),
			fgets(t, 132, stdin) != NULL)
		atl_eval(t);
}


//#ifdef __cplusplus
//}
//#endif

