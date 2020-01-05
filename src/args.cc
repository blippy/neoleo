/*
 * $Id: args.c,v 1.6 2000/08/10 21:02:49 danny Exp $
 *
 * Copyright (c) 1993, 1999 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <ctype.h>
#include <string.h>

#include "global.h"
#define DEFINE_STYLES	1
#include "args.h"
#include "convert.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "io-edit.h"
#include "io-utils.h"
#include "format.h"
#include "utils.h"
//#include "xcept.h"



#include "cmd.h"
#include "io-term.h"
#include "basic.h"
#include "format.h"
#include "regions.h"
#include "window.h"
#include "sheet.h"


/* Returns 0 if the function is found.
 * Also returns (through parameters) the vector and cmd_func.
 * The output parameters can be NULL.
 */

	int 
find_function (int * vec_out, struct cmd_func ** cmd_out, const char * name, int len)
{
	int vector;
	struct cmd_func * cmd;
	for (vector = 0; vector < num_funcs; vector++)
		for (cmd = &the_funcs[vector][0]; cmd->func_name; cmd++)
			if (!(strincmp (name, cmd->func_name, len) || cmd->func_name[len]))
			{
				if (vec_out)
					*vec_out = vector;
				if (cmd_out)
					*cmd_out = cmd;
				return 0;
			}
	return 1;
}  

static struct cmd_func * named_macro_strings = 0;
static int num_named_macro_strings = 0;
static int named_macro_vec;

	void 
init_named_macro_strings (void)
{
	named_macro_strings =
		(struct cmd_func *) ck_malloc (sizeof (struct cmd_func));
	bzero (named_macro_strings, sizeof (struct cmd_func));
	named_macro_vec = add_usr_cmds (named_macro_strings);
}
/* These commands define the syntax and editting modes of command arguments.
 * Each _verify function parses some kind of argument and stores its value
 * in a command_arg structure.  An error message or NULL is returned.
 * If the match succeeds, a string pointer is advanced to the end of what was
 * parsed.
 *
 * Several arguments may be listed on one line separated only by
 * whitespace.  A _verify function should stop after its argument and ignore
 * everything after that. 
 */

	const char *
char_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}

	const char *
symbol_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}

	const char *
word_verify ( char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}

	void
symbol_destroy (struct command_arg * arg)
{
	ASSERT_UNCALLED();
}

	const char *
command_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}

	const char * 
read_file_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}

	void
read_file_destroy (struct command_arg * arg)
{
	ASSERT_UNCALLED();
}


	const char * 
write_file_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}

	void
write_file_destroy (struct command_arg * arg)
{
	ASSERT_UNCALLED();
}

/* As a special case, cmd_loop makes sure that keyseq arguments are only read
 * interactively.
 */

	const char *
keyseq_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}


	const char *
keymap_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}


	const char *
number_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}


	const char *
double_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}


	const char * 
range_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}

	const char * 
string_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}

/* Unlike most verify functions, this
 * one may destroy the command frame that it is 
 * operating on.  It's purpose is to allow user's
 * to abort commands. 
 */
	const char * 
yes_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}

	const char *
incremental_cmd_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}




	const char *
format_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}


	const char *
noop_verify (char ** end, struct command_arg * arg)
{
	ASSERT_UNCALLED();
	return 0;
}
