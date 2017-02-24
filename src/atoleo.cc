#include <stdio.h>

#include "atldef.h"
#include "basic.h"
#include "cmd.h"
#include "ref.h"

#ifdef __cplusplus
extern "C" {
#endif

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
	{"04XCMD",	p4xcmd},
	{NULL,		(codeptr) 0 }
};

void
init_atoleo()
{
	atl_init();
	atl_primdef(oleop);
}


#ifdef __cplusplus
}
#endif

