/*
 * $Id: funcs.c,v 1.5 2000/08/10 21:02:50 danny Exp $
 *
 * Copyright © 1993 Free Software Foundation, Inc.
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

/* 
 * This file contains descriptions of all the interactive functions 
 * built into oleo. 
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include "global.h"
#include "cmd.h"
#include "key.h"
#include "io-term.h"
#include "basic.h"
#include "format.h"
#include "print.h"
#include "font.h"
#include "io-x11.h"
#include "io-edit.h"
#include "regions.h"
#include "help.h"
#include "window.h"
#include "font.h"
#include "graph.h"
#include "lists.h"


/* This include builds the function table, doc strings, and FUNC_ARGS strings.
 */
#include "defuns.h"


/* Returns 0 if the function is found.
 * Also returns (through parameters) the vector and cmd_func.
 * The output parameters can be NULL.
 */

int find_function (int * vec_out, struct cmd_func ** cmd_out, char * name, int len)
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

void init_named_macro_strings (void)
{
  named_macro_strings =
    (struct cmd_func *) ck_malloc (sizeof (struct cmd_func));
  bzero (named_macro_strings, sizeof (struct cmd_func));
  named_macro_vec = add_usr_cmds (named_macro_strings);
}


void name_macro_string (char * name, char * str)
{
  int i = num_named_macro_strings;
  ++num_named_macro_strings;
  named_macro_strings =
    ((struct cmd_func *)
     ck_realloc (named_macro_strings,
		 ((1 + num_named_macro_strings) * sizeof (struct cmd_func))));
  the_funcs [named_macro_vec] = named_macro_strings;
  bzero (named_macro_strings + num_named_macro_strings,
	 sizeof (struct cmd_func));
  {
    struct cmd_func * cf = &named_macro_strings [i];
    cf->func_name = ck_savestr (name);
    cf->func_func = run_string_as_macro;
    cf->init_code = 0;
    {
      struct info_buffer * ib = find_or_make_info (name);
      clear_info (ib);
      print_info (ib, "Equivalent to %s.", str);
      cf->func_doc = ib->text;
    }
    {
      int i = 0;
      cf->func_args = (char **)ck_malloc (3 * sizeof (char *));
      cf->func_args [i++] = mk_sprintf ("=%s", str);
      cf->func_args [i++] = "p";
      cf->func_args [i++] = 0;
    }
  }
}

