#include <stdio.h>

#include "atldef.h"
#include "basic.h"
#include "cmd.h"
#include "ref.h"

/*
prim p4eval()
{
	Sl(1);
	char *cmd = S0;
	Pop;

	atl_eval(cmd);
}
*/

prim p4life()
{
	//puts("TODO plife");
	//edit_cell("42");
	//set_cell(curow, cucol, "42");
	//io_repaint();
	new_value(curow, cucol, "42");
	//atl_eval("\"added the meaning of life\" type cr");
}

static struct primfcn oleop[] = {
	//{"04EVAL",	p4eval},
	{"04LIFE",	p4life},
	{NULL,		(codeptr) 0 }
};

void
init_atoleo()
{
	atl_init();
	atl_primdef(oleop);
}
