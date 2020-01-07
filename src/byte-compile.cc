/*
 * $Id: byte-compile.c,v 1.12 2000/11/22 19:33:00 danny Exp $
 *
 * Copyright © 1990, 1992, 1993 Free Software Foundation, Inc.
 * 
 * This file is part of Oleo, the GNU Spreadsheet.
 * 
 * Oleo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * Oleo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Oleo; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstddef>
#include <string>
#include <stack>
#include <map>

//#include "funcs.h"
#include "parse_parse.h"

#include "global.h"
#include "byte-compile.h"
#include "node.h"
#include "eval.h"
#include "logging.h"
#include "ref.h"
#include "sort.h"
//#include "busi.h"
#include "parse_parse.h"

#include "cell.h"
#include "byte-compile.h"
#include "utils.h"
#include "ref.h"

using namespace std::string_literals;

using IFPTR = int (*)(int, int);
using VIFPTR = void (*)(int, int);

//extern function_t busi_funs[];
extern function_t cells_funs[];

extern char *instr;
extern int parse_error;
extern struct node *parse_return;

static void add_backpatch (unsigned, unsigned);

struct backpatch
{
	unsigned from, to;
};

static struct backpatch *patches;
static int patches_allocated;
static int patches_used;
static void *fn_stack;
static void *str_stack;


#define V (void(*)())
double fabs1(double x) { return  fabs(x); }
double pow1(double x, double y) { return pow(x, y); }


#ifndef USE_OBSTACK
	obsmem bcomp_obsmem;
#endif

// wrappers around obstack, planning its eventual demise

	
void obstack_mc_begin(int n)
{
#ifdef USE_OBSTACK
	obstack_begin(&tmp_mem, n);
#endif
}

char * obstack_mc_alloc(int n)
{
#ifdef USE_OBSTACK
	return (char *) obstack_alloc(&tmp_mem, n);
#else
	return bcomp_obsmem.alloc(n);
#endif
}

void obstack_mc_grow(void* data, int size)
{
#ifdef USE_OBSTACK
	obstack_grow(&tmp_mem, data, size);
#else
	bcomp_obsmem.grow(data, size);
#endif
}

void obstack_mc_1grow(char c)
{
#ifdef USE_OBSTACK
	obstack_1grow(&tmp_mem, c);
#else
	bcomp_obsmem.grow1(c);
#endif
}


int obstack_mc_object_size()
{
#ifdef USE_OBSTACK
	return obstack_object_size(&tmp_mem);
#else
	return bcomp_obsmem.size();
#endif
}

void* obstack_mc_finish()
{
#ifdef USE_OBSTACK
	return obstack_finish(&tmp_mem);
#else
	return bcomp_obsmem.finish();
#endif
}

void obstack_mc_free(void *mem_start)
{
#ifdef USE_OBSTACK
	obstack_free(&tmp_mem, mem_start);
#else
	bcomp_obsmem.free_mem();
#endif
}

void obstack_mc_free_all()
{
#ifndef USE_OBSTACK
	bcomp_obsmem.reset();
#endif
}

#define S (char *)
/* These have to go in some file or other, so it is stuck in here (for now).
 */
function_t the_funs[] =
{
  {0, X_A0, "", 0, S "<END>"},
  {0, X_A0, "", 0, S "<DUMMY1>"},

  {C_IF | R | INF (1), X_A1 | X_J, "D", 0, S "?"},
  {C_IF | R | INF (1), X_A1 | X_JL, "D", 0, S "?"},
  {C_IF, X_A1 | X_J, "D", 0, S "if"},
  {C_IF, X_A1 | X_JL, "D", 0, S "if"},
  {C_ANDOR, X_A1 | X_J, "D", 0, S "and"},
  {C_ANDOR, X_A1 | X_JL, "D", 0, S "and"},
  {C_ANDOR, X_A1 | X_J, "D", 0, S "or"},
  {C_ANDOR, X_A1 | X_JL, "D", 0, S "or"},
  {C_STR, X_A0 | X_J, "", 0, S "\"%s\""},
  {C_STR, X_A0 | X_JL, "", 0,S  "\"%s\""},

  {C_CELL, X_A0,  "", 0, S "$%s$%u"},
  {C_CELL, X_A0, "", 0, S "$%s%u"},
  {C_CELL, X_A0, "", 0, S "%s$%u"},
  {C_CELL, X_A0, "", 0, S "%s%u"},
  {C_RANGE, X_A0, "", 0, S "$%s$%u:$%s$%u"},
  {C_RANGE, X_A0, "", 0, S "$%s%u:$%s$%u"},
  {C_RANGE, X_A0, "", 0, S "$%s$%u:$%s%u"},
  {C_RANGE, X_A0, "", 0, S "$%s%u:$%s%u"},
  {C_RANGE, X_A0, "", 0, S "%s$%u:$%s$%u"},
  {C_RANGE, X_A0, "", 0, S "%s%u:$%s$%u"},
  {C_RANGE, X_A0, "", 0, S "%s$%u:$%s%u"},
  {C_RANGE, X_A0, "", 0, S "%s%u:$%s%u"},
  {C_RANGE, X_A0, "", 0, S "$%s$%u:%s$%u"},
  {C_RANGE, X_A0, "", 0, S "$%s%u:%s$%u"},
  {C_RANGE, X_A0, "", 0, S "$%s$%u:%s%u"},
  {C_RANGE, X_A0, "", 0, S "$%s%u:%s%u"},
  {C_RANGE, X_A0, "", 0, S "%s$%u:%s$%u"},
  {C_RANGE, X_A0, "", 0, S "%s%u:%s$%u"},
  {C_RANGE, X_A0, "", 0, S "%s$%u:%s%u"},
  {C_RANGE, X_A0, "", 0, S "%s%u:%s%u"},

  {C_CONST, X_A0, "", 0, tname},
  {C_CONST, X_A0, "", 0, fname},

  {C_CONST, X_A0, "", 0, iname},
  {C_CONST, X_A0, "", 0, mname},
  {C_CONST, X_A0, "", 0, nname},
  {C_ERR, X_A0 | X_J, "", 0,S  "%s"},
  {C_FLT, X_A0, "", 0, S "%.15g"},
  {C_INT, X_A0, "", 0, S "%ld"},

  {C_VAR, X_A0, "", 0, S "%s"},

  {C_UNA, X_A1, "F", 0, S "-"},
  {C_UNA, X_A1, "B", 0, S "!"},

  {C_INF | L | INF (6), X_A2, "NN", 0, S "-"},
  {C_INF | L | INF (7), X_A2, "NN", 0, S "/"},
  {C_INF | L | INF (7), X_A2, "NN", 0, S "%"},
  {C_INF | L | INF (7), X_A2, "NN", 0, S "*"},
  {C_INF | L | INF (6), X_A2, "NN", 0, S "+"},
  {C_INF | L | INF (2), X_A2, "SS", 0, S "&"},
  {C_INF | N | INF (4), X_A2, "AA", 0, S "="},
  {C_INF | N | INF (5), X_A2, "AA", 0, S ">="},
  {C_INF | N | INF (5), X_A2, "AA", 0, S ">"},
  {C_INF | N | INF (5), X_A2, "AA", 0, S "<"},
  {C_INF | N | INF (5), X_A2, "AA", 0, S "<="},
  {C_INF | N | INF (4), X_A2, "AA", 0, S "!="},
  {C_INF | R | INF (8), X_A2, "FF", V pow1, S "^"},

  {C_FN0, X_A0, "", 0, S "pi"},
  {C_FN0X, X_A0, "", 0, S "row"},
  {C_FN0X, X_A0, "", 0, S "col"},
  {C_FN0 | C_T, X_A0, "", 0, S "now"},

  {C_FN1, X_A1, "F", V fabs1, S "abs"},
  {C_FN1, X_A1, "A", 0, S "negate"},
  {C_FN1, X_A1, "A", 0, S "not"},

  {C_FN1, X_A1, "R", 0, S "rows"},
  {C_FN1, X_A1, "R", 0, S "cols"},
  {C_FN2, X_A2, "FI", 0, S "fixed"},
  {C_FN2, X_A2, "AA", 0, S "iferr"},

  {C_FNN, X_AN, "SIIA", 0, S "file"},
  {C_FNN, X_AN, "EEEE", 0, S "sum"},
  {C_FNN, X_AN, "EEEE", 0, S "prod"},
  {C_FNN, X_AN, "EEEE", 0, S "avg"},
  {C_FNN, X_AN, "EEEE", 0, S "std"},
  {C_FNN, X_AN, "EEEE", 0, S "max"},
  {C_FNN, X_AN, "EEEE", 0, S "min"},
  {C_FNN, X_AN, "EEEE", 0, S "count"},
  {C_FNN, X_AN, "EEEE", 0, S "var"},
  0

};

/*
 * This is the place where you can add extra functions
 */
static function_t *__usr_funs[] =
{
	//busi_funs,
	cells_funs,
	/* Add something here */
};

/*
 * A small function in each module tells us how many functions
 * it defines.
 */
//extern int init_busi_function_count(void);
extern int init_cells_function_count(void);
//extern int init_mysql_function_count(void);
//extern int init_gsl_function_count(void);
/* Add something here */

typedef int (*init_function_count)(void);

static init_function_count init_function_counts[] = {
	//&init_busi_function_count,
	&init_cells_function_count,
	/* Add something here */
};

/* Determine how many groups of functions we have */
int n_usr_funs = sizeof(init_function_counts) / sizeof(init_function_count);

/*
 * This is an array containing the number of functions in each group.
 * It is allocated and initialised in init_mem() below.
 * Previous versions of Oleo required manual maintenance of this.
 */
int *usr_n_funs = NULL;

function_t **usr_funs = __usr_funs;

/* ... A whole huge empty space, then ... */
function_t skip_funs[] =
{
  {C_SKIP, X_A0 | X_J, "", 0,  (char *)"<Skip %u>"},
  {C_SKIP, X_A0 | X_JL,  "", 0,  (char *)"<SkipL %u>"},
};



void init_bcode_func(const char* name, function_t* funcs)
{
	add_parse_hash(name, funcs);
}

/* The memory allocated here is used for several things, but byte_compile
   is a small file, so it might as well be here */
void
init_mem ()
{
	int n, i;

	/* Initialise counters */
	usr_n_funs = (int *)calloc(n_usr_funs, sizeof(int));
	for (i=0; i<n_usr_funs; i++)
		usr_n_funs[i] = init_function_counts[i]();

	//parse_hash = hash_new ();
	init_bcode_func(the_funs[F_IF].fn_str, &the_funs[F_IF]);
	init_bcode_func(the_funs[AND].fn_str, &the_funs[AND]);
	init_bcode_func(the_funs[OR].fn_str, &the_funs[OR]);
	for (n = F_PI; n < USR1; n++) {
		auto f = the_funs[n];
		if(f.fn_comptype==0) break;
		const char* fn_name = f.fn_str;
		if(fn_name)
			init_bcode_func(fn_name, &the_funs[n]);
	}

	for (n = 0; n < n_usr_funs; n++)
	{
		for (int nn = 0; usr_funs[n][nn].fn_str; nn++)
			init_bcode_func(usr_funs[n][nn].fn_str,  &usr_funs[n][nn]);
	}

	fn_stack = init_stack ();
	str_stack = init_stack ();
	obstack_mc_begin(400);
#ifdef USE_OBSTACK
	tmp_mem_start =  obstack_mc_alloc(0);
#else
	obstack_mc_alloc(0);
#endif
}

/* Stash away a backpatch for future editing. */
static void
add_backpatch (unsigned from, unsigned to)
{
	if (!patches)
	{
		patches_allocated = 5;
		patches = (struct backpatch *) ck_malloc (patches_allocated * sizeof (struct backpatch));
		patches_used = 0;
	}
	if (patches_allocated == patches_used)
	{
		patches_allocated *= 2;
		patches = (struct backpatch *) ck_realloc (patches, patches_allocated * sizeof (struct backpatch));
	}
	patches[patches_used].from = from;
	patches[patches_used].to = to;
	patches_used++;
}

static int
cmp_patch (int n1, int n2)
{
  int ret;

  ret = (patches[n1].from == patches[n2].from) ? patches[n1].to - patches[n2].to : patches[n1].from - patches[n2].from;
  return ret;
}

static void
swp_patch (int n1, int n2)
{
  struct backpatch tmp;

  tmp = patches[n1];
  patches[n1] = patches[n2];
  patches[n2] = tmp;
}

static void
rot_patch (int n1, int n2)
{
	struct backpatch tmp;
	tmp = patches[n2];
	while (n2 > n1)
	{
		patches[n2] = patches[n2 - 1];
		--n2;
	}
	patches[n2] = tmp;
}


bool process_function(cell* cp, const function_t *f, struct node*& node, int& byte)
{
	struct node *new_node;

	switch (GET_COMP (f->fn_comptype))
	{
		case C_IF:
			/* if compiles to
			   test-code IF amt-to-skip-on-false true-code SKIP
			   amt-to-skip-on-true false-code */
			if (node->n_x.v_subs[0])
			{
				if (node->n_x.v_subs[0]->n_x.v_subs[0])
				{
					/* Put out the test-code */
					push_stack (fn_stack, node);
					new_node = node->n_x.v_subs[0]->n_x.v_subs[0];
					node->n_x.v_subs[0]->n_x.v_subs[0] = 0;
					node = new_node;
					goto loop;
				}
				/* Put out IF, null-byte to backpatch */
				obstack_mc_1grow(byte);
				node->add_byte = obstack_mc_object_size();
				obstack_mc_1grow(0);

				/* put out true-code */
				push_stack (fn_stack, node);
				new_node = node->n_x.v_subs[0]->n_x.v_subs[1];
				node->n_x.v_subs[0] = 0;
				node = new_node;
				goto loop;
			}
			if (node->n_x.v_subs[1])
			{

				obstack_mc_1grow((char)SKIP);
				obstack_mc_1grow(0);
				add_backpatch (node->add_byte, obstack_mc_object_size());
				node->add_byte = obstack_mc_object_size() - 1;

				push_stack (fn_stack, node);
				new_node = node->n_x.v_subs[1];
				node->n_x.v_subs[1] = 0;
				node = new_node;
				goto loop;
			}
			add_backpatch (node->add_byte, obstack_mc_object_size());
			break;

		case C_ANDOR:
			if (node->n_x.v_subs[0])
			{
				push_stack (fn_stack, node);
				new_node = node->n_x.v_subs[0];
				node->n_x.v_subs[0] = 0;
				node = new_node;
				goto loop;
			}
			if (node->n_x.v_subs[1])
			{
				obstack_mc_1grow(byte);
				node->add_byte = obstack_mc_object_size();
				obstack_mc_1grow(0);	/* for backpatching */
				push_stack (fn_stack, node);
				new_node = node->n_x.v_subs[1];
				node->n_x.v_subs[1] = 0;
				node = new_node;
				goto loop;
			}
			add_backpatch (node->add_byte, obstack_mc_object_size());
			break;

		case C_ERR:
			obstack_mc_1grow(byte);
			node->add_byte = obstack_mc_object_size();
			obstack_mc_1grow(0);
			obstack_mc_1grow(node->n_x.v_int);
			node->n_x.v_string = ename[node->n_x.v_int];
			push_stack (str_stack, node);
			break;

		case C_FLT:
			obstack_mc_1grow(byte);
			obstack_mc_grow(&(node->n_x.v_float), sizeof (num_t));
			break;

		case C_INT:
			obstack_mc_1grow(byte);
			obstack_mc_grow(&(node->n_x.v_int), sizeof (long));
			break;

		case C_STR:
			obstack_mc_1grow(byte);
			node->add_byte = obstack_mc_object_size();
			obstack_mc_1grow(0);
			push_stack (str_stack, node);
			break;

		case C_VAR:
			add_ref_to (cp, obstack_mc_object_size());
			add_var_ref (node->n_x.v_var);
			obstack_mc_1grow(byte);
			obstack_mc_grow(&(node->n_x.v_var), sizeof (struct var *));
			break;

		case C_CELL:
			add_ref_to (cp, obstack_mc_object_size());
			add_ref (node->n_x.v_rng.lr, node->n_x.v_rng.lc);
			obstack_mc_1grow(byte);
			obstack_mc_1grow(node->n_x.v_rng.lr >> 8);
			obstack_mc_1grow(node->n_x.v_rng.lr);
			obstack_mc_1grow(node->n_x.v_rng.lc >> 8);
			obstack_mc_1grow(node->n_x.v_rng.lc);
		       	break;

		case C_RANGE:
			add_ref_to (cp, obstack_mc_object_size());
			add_range_ref (&(node->n_x.v_rng));
			obstack_mc_1grow(byte);
			obstack_mc_grow(&(node->n_x.v_rng), sizeof (struct rng));
			break;

		case C_FN0X:
			add_ref_to (cp, obstack_mc_object_size());
			/* FALLTHROUGH */
		case C_FN0:
		case C_CONST:
add_byte:
			if (f->fn_comptype & C_T)
				add_timer_ref (cp, obstack_mc_object_size());
			obstack_mc_1grow(byte);
			if (byte >= USR1 && byte < SKIP)
				obstack_mc_1grow((int) node->sub_value);
			break;

		case C_FN1:
		case C_UNA:
			if (node->n_x.v_subs[0])
			{
				push_stack (fn_stack, node);
				new_node = node->n_x.v_subs[0];
				node->n_x.v_subs[0] = 0;
				node = new_node;
				goto loop;
			}
			goto add_byte;

		case C_FN2:
		case C_INF:
			if (node->n_x.v_subs[0])
			{
				push_stack (fn_stack, node);
				new_node = node->n_x.v_subs[0];
				node->n_x.v_subs[0] = 0;
				node = new_node;
				goto loop;
			}
			if (node->n_x.v_subs[1])
			{
				push_stack (fn_stack, node);
				new_node = node->n_x.v_subs[1];
				node->n_x.v_subs[1] = 0;
				node = new_node;
				goto loop;
			}
			goto add_byte;

		case C_FN3:
			if (node->n_x.v_subs[0])
			{
				if (node->n_x.v_subs[0]->n_x.v_subs[0])
				{
					push_stack (fn_stack, node);
					new_node = node->n_x.v_subs[0]->n_x.v_subs[0];
					node->n_x.v_subs[0]->n_x.v_subs[0] = 0;
					node = new_node;
					goto loop;
				}
				push_stack (fn_stack, node);
				new_node = node->n_x.v_subs[0]->n_x.v_subs[1];
				node->n_x.v_subs[0] = 0;
				node = new_node;
				goto loop;
			}
			if (node->n_x.v_subs[1])
			{
				push_stack (fn_stack, node);
				new_node = node->n_x.v_subs[1];
				node->n_x.v_subs[1] = 0;
				node = new_node;
				goto loop;
			}
			goto add_byte;

		case C_FN4:
			if (node->n_x.v_subs[0])
			{
				if (node->n_x.v_subs[0]->n_x.v_subs[0])
				{
					push_stack (fn_stack, node);
					new_node = node->n_x.v_subs[0]->n_x.v_subs[0];
					node->n_x.v_subs[0]->n_x.v_subs[0] = 0;
					node = new_node;
					goto loop;
				}
				push_stack (fn_stack, node);
				new_node = node->n_x.v_subs[0]->n_x.v_subs[1];
				node->n_x.v_subs[0] = 0;
				node = new_node;
				goto loop;
			}
			if (node->n_x.v_subs[1])
			{
				if (node->n_x.v_subs[1]->n_x.v_subs[0])
				{
					push_stack (fn_stack, node);
					new_node = node->n_x.v_subs[1]->n_x.v_subs[0];
					node->n_x.v_subs[1]->n_x.v_subs[0] = 0;
					node = new_node;
					goto loop;
				}
				push_stack (fn_stack, node);
				new_node = node->n_x.v_subs[1]->n_x.v_subs[1];
				node->n_x.v_subs[1] = 0;
				node = new_node;
				goto loop;
			}
			goto add_byte;

		case C_FNN:
			if (node->n_x.v_subs[1])
			{
				if (node->add_byte == 0)
					for (new_node = node; new_node->n_x.v_subs[1]; new_node = new_node->n_x.v_subs[1])
						node->add_byte++;
				for (new_node = node; new_node->n_x.v_subs[1]->n_x.v_subs[1]; new_node = new_node->n_x.v_subs[1])
					;
				push_stack (fn_stack, node);
				node = new_node->n_x.v_subs[1]->n_x.v_subs[0];
				new_node->n_x.v_subs[1] = 0;
				goto loop;
			}
			obstack_mc_1grow(byte);
			if (byte >= USR1 && byte < SKIP)
				obstack_mc_1grow((int) node->sub_value);
			obstack_mc_1grow(node->add_byte);
			break;

		default:
			panic ("Bad comptype %d", f->fn_comptype);
	}

	return false;
loop:
	return true;

}


/* This takes an ascii string and returns a pointer to the byte-compiled
   result.  It calls yyparse() to do the actual parsing.  This is complicated
   only because yyparse returns a parse tree which needs to be turned into
   postfix compiled bytes.  This is further complicated by the presence of
   forward branches in the byte-compiled code.  That's what the backpatch
   stuff is for.

   It'd be nice if oneof() could compile into
   arg1
   ONEOF n_possibilities
   JUMP poss1
   JUMP poss2
   JUMP poss3
   ...
   JUMP error
   {poss 1}
   JUMP end
   {poss 2}
   JUMP end
   ...
   end: {rest of expression}
   instead of the simplistic (and slow-to-execute) version currently used

   It'd also be nice if byte-compiled expressions could have *BIG*
   subexpressions, instead of silently failing as they do now.  Error checking
   and a way to encode longer branches would be a *good* idea.
 */

formula_t
parse_and_compile(cell* cp, const char* string)
{
	mem_c the_mem; // we're just going to ignore the mem allocated
	return parse_and_compile(cp, string, the_mem);
}
formula_t
parse_and_compile(cell* cp, const std::string& str)
{
	mem_c the_mem; // we're just going to ignore the mem allocated
	return parse_and_compile(cp, str.c_str(), the_mem);
}

formula_t parse_and_compile (cell* cp)
{
	auto frm = cp->get_formula_text();
	return parse_and_compile(cp, frm);
}

formula_t
parse_and_compile_1 (cell* cp, const char *string, mem_c& the_mem)
{
	struct node *node;
	const function_t *f;
	formula_t ret;
	int n;
	unsigned buf_siz;
	int need_relax;
	int byte;

	//parse_error = 0;
	patches_used = 0;

	FormulaParser yy;
	bool ok = yy.parse(string);
	cp->set_refs(get_parse_prec_cells());
	//if (yyparse_parse(string, the_mem) || parse_error)
	if(!ok)
	{
		ret = (formula_t) ck_malloc (strlen (string) + 5);
		ret[0] = CONST_ERR;
		ret[1] = 2;
		ret[2] = parse_error;
		ret[3] = ENDCOMP;
		strcpy ((char *) &ret[4], string);
		return ret;
	}

	node = yy.root();
	if (!node)
		return 0;

loop:
	if (node->comp_value < USR1)
	{
		f = &the_funs[node->comp_value];
	}
	else if (node->comp_value < SKIP)
	{
		n = node->sub_value;
		f = &usr_funs[node->comp_value - USR1][n];
	}
	else
	{
		f = &skip_funs[node->comp_value - SKIP];
	}
	byte = node->comp_value;

#ifdef TEST
	if (!f)
		panic ("f is zero in byte_compile!");
#endif
	if(process_function(cp, f, node, byte)) goto loop;

	node = (struct node *) pop_stack (fn_stack);
	if (node)
		goto loop;

	obstack_mc_1grow(0);

	while (node = (struct node *) pop_stack (str_stack))
	{
		add_backpatch (node->add_byte, obstack_mc_object_size());
		obstack_mc_grow(node->n_x.v_string, strlen (node->n_x.v_string) + 1);
	}

	buf_siz = obstack_mc_object_size();
	ret = (formula_t) ck_malloc (buf_siz);
	//the_mem.add_ptr(ret);
	bcopy (obstack_mc_finish(), ret, buf_siz);

	need_relax = 0;
	for (n = 0; n < patches_used; n++)
	{
		long offset;

		offset = (patches[n].to - patches[n].from) - 1;
		if (offset < 0 || offset > 255)
			need_relax++;
		else
			ret[patches[n].from] = offset;
	}
	if (need_relax)
	{
		int n_lo;
		long offset;
		int start;

		/* ... Sort the patches list ... */
		sort ((int) patches_used, (IFPTR) cmp_patch, (VIFPTR) swp_patch, (VIFPTR)rot_patch);

		while (need_relax)
		{
			ret = (formula_t) ck_realloc (ret, buf_siz + need_relax);
			for (n_lo = 0; n_lo < patches_used; n_lo++)
			{
				offset = (patches[n_lo].to - patches[n_lo].from) - 1;
				if (offset < 0 || offset > 255 - need_relax)
					break;
			}

			/* n_lo points to the first jump that may need to be relaxed */
			for (n = n_lo; n < patches_used; n++)
			{
				offset = (patches[n].to - patches[n].from) - 1;
				if (offset < 0 || offset > 255)
				{
					int nn;

					start = patches[n].from;

					ret[start - 1]++;	/* Translate insn to LONG */
					ret[start] = offset;
					bcopy (&ret[start + 1], &ret[start + 2], buf_siz - start);
					ret[start + 1] = offset >> 8;
					need_relax--;
					buf_siz++;
					for (nn = 0; nn < patches_used; nn++)
					{
						if (patches[nn].from > start)
							patches[nn].from++;
						if (patches[nn].to > start)
							patches[nn].to++;
						if (patches[nn].from < start && patches[nn].to > start && ret[patches[nn].from]++ == 255)
						{
							if (ret[patches[nn].from - 1] & 01)
								ret[patches[nn].from + 1]++;
							else
								need_relax++;
						}
					}
				}
			}
		}
	}


	patches_used = 0;

	return ret;
}

// mcarter 29-Apr-2018
// caller is responsible for free'ing `char* ret'. The chances are
// that it will be stored in the cells, and become automatically
// reaped that way.
formula_t
parse_and_compile (cell* cp, const char *string, mem_c& the_mem)
{
	formula_t ret = parse_and_compile_1(cp, string, the_mem);
#ifdef USE_OBSTACK
	if(ret) obstack_mc_free(tmp_mem_start);
#else
	if(ret) obstack_mc_free_all();
#endif
	return ret;
}


/* This tries to tell if a byte-compiled expression is a constant.  If it
   is a constant, we can free it, and never try to recompute its value.
   This returns non-zero if the expression is constant.*/
int
is_constant (const unsigned char *bytes)
{
  /* It's constant, but it's already been dealt with.
	   Pretend it isn't. */
  if (!bytes)
    return 0;

  switch (bytes[0])
    {
    case CONST_ERR:
      return (bytes[3] == 0 && !strcmp ((char *) bytes + 4, ename[bytes[2]]));
    case CONST_INT:
      return bytes[sizeof (long) + 1] == ENDCOMP;
    case CONST_FLT:
      return bytes[sizeof (double) + 1] == ENDCOMP;
    case CONST_STR:
      return bytes[2] == ENDCOMP;
    case F_TRUE:
    case F_FALSE:
    case CONST_INF:
    case CONST_NINF:
    case CONST_NAN:
      return bytes[1] == ENDCOMP;
    default:
      return 0;
    }
}
