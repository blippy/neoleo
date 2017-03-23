#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>



#include "atldef.h"
#include "atlast.h"
#include "atoleo.h"
#include "basic.h"
#include "cmd.h"
#include "ref.h"

// For some reason, S1() macro doesn't compile here, so
// redefining them again seems to "fix" the problem.
// Bizarre
#define S1A(x) if ((stk-stack)<(x)) {stakunder(); return Memerrs;}

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


prim p4included()
{

	S1A(1); 
	char *path = (char *)S0;
	Pop;
	FILE *fp = fopen(path, "r");
	if(!fp) return;
	int stat = atl_load(fp);

	fclose(fp);
}

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
	{"04INCLUDED",	p4included},
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



/* ****************************************************************** */
/* a repl using readline */

/*
void
forth_repl()
{
	char t[132];
	while(printf("-> "),
			fgets(t, 132, stdin) != NULL)
		atl_eval(t);
}
*/

// taken from http://web.mit.edu/gnu/doc/html/rlman_2.html
char *
rl_gets()
{
	static char *line_read = (char *)NULL;

	if(read_line) {
		// if buffer allocated, return the mmeory to the free pool
		free(line_read);
		line_read = (char *)NULL;
	}

	line_read = readline("-> ");

	if(line_read && *line_read)
		add_history(line_read);

	return line_read;
}

void
forth_repl()
{
	char *line;
	while(line = rl_gets())
		atl_eval(line);
}




/* ****************************************************************** */

//#ifdef __cplusplus
//}
//#endif

