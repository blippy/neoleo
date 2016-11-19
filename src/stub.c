/*
 * $Id: stub.c,v 1.5 2000/07/22 06:13:16 danny Exp $
 *
 * Copyright © 1993, 2000 Free Software Foundation, Inc.
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

static char *rcsid = "$Id: stub.c,v 1.5 2000/07/22 06:13:16 danny Exp $";

/*
 * This is a collection of stubs that are used to call interactive functions.
 *
 * Their responsability is to extract arguments from a command_frame as
 * constructed by the function COMMAND_LOOP.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include "global.h"
#include "cmd.h"
#include "stub.h"

static void
find_args (struct command_arg ** argv_out, int argc, struct command_frame * frame)
{
  int found = 0;
  int pos = 0;
  while (found < argc)
    {
      if (frame->argv[pos].style->representation != cmd_none)
	argv_out[found++] = &frame->argv[pos];
      ++pos;
    }
}

/* These macros are invoked in stubs.h and are used to define
 * the stub functions.  Later, these macros will be redifined 
 * an used to build a table of stub functions.
 */

#define STUB1(STR,NAME, PRE, VAL, TYPE) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  struct command_arg * argv; \
  find_args (&argv, 1, frame); \
  ((void (*) (TYPE)) frame->cmd->func_func) (PRE argv->val.VAL); \
}

#define STUB2(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  struct command_arg * argv[2]; \
  find_args (argv, 2, frame); \
  ((void (*) (TYPE1, TYPE2)) frame->cmd->func_func) \
    (PRE1 argv[0]->val.VAL1,  \
     PRE2 argv[1]->val.VAL2); \
}


#define STUB3(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  struct command_arg * argv[3]; \
  find_args (argv, 3, frame); \
  frame->cmd->func_func \
    (PRE1 argv[0]->val.VAL1, \
     PRE2 argv[1]->val.VAL2, \
     PRE3 argv[2]->val.VAL3); \
}

#define STUB4(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3,PRE4,VAL4,TYPE4) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  struct command_arg * argv[4]; \
  find_args (argv, 4, frame); \
  frame->cmd->func_func \
    (PRE1 argv[0]->val.VAL1, \
     PRE2 argv[1]->val.VAL2, \
     PRE3 argv[2]->val.VAL3, \
     PRE4 argv[3]->val.VAL4); \
}

#define STUB5(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3,PRE4,VAL4,TYPE4,PRE5,VAL5,TYPE5) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  struct command_arg * argv[5]; \
  find_args (argv, 5, frame); \
  frame->cmd->func_func \
    (PRE1 argv[0]->val.VAL1, \
     PRE2 argv[1]->val.VAL2, \
     PRE3 argv[2]->val.VAL3, \
     PRE4 argv[3]->val.VAL4, \
     PRE5 argv[4]->val.VAL5); \
}

/* This contains the list of stub functions. */

#include "stubs.h"

/* There is only one `STUB0' so we needn't bother with a macro. */

static void
stub_void (struct command_frame * frame)
{
  frame->cmd->func_func ();
}

/* For define_usr_fmt */
static void
stub_isssssssss (frame)
     struct command_frame * frame;
{
  struct command_arg * argv[10];
  find_args (argv, 10, frame);
  frame->cmd->func_func (argv[0]->val.integer,
			 argv[1]->val.string,
			 argv[2]->val.string,
			 argv[3]->val.string,
			 argv[4]->val.string,
			 argv[5]->val.string,
			 argv[6]->val.string,
			 argv[7]->val.string,
			 argv[8]->val.string,
			 argv[9]->val.string);
}

/* Single character type-codes denote the types of arguments.  A string
 * of type-codes maps to a stub function (hopefully).
 */
struct cmd_stub
{
  char * type;
  cmd_invoker stub;
};

#undef STUB1
#undef STUB2
#undef STUB3
#undef STUB4
#undef STUB5

#define STUB1(STR,NAME,PRE1,VAL1,TYPE1) { STR, NAME },
#define STUB2(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2)  { STR, NAME },
#define STUB3(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3) \
	{ STR, NAME },
#define STUB4(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3,PRE4,VAL4,TYPE4) \
	{ STR, NAME },
#define STUB5(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3,PRE4,VAL4,TYPE4,PRE5,VAL5,TYPE5) \
	{ STR, NAME },

static struct cmd_stub the_stubs[] =
{
  { "", stub_void },
#include "stubs.h"
  { "isssssssss", stub_isssssssss },
  { 0, 0 }
};

/*
 * This looks at the arguments built for the current command and 
 * finds the right stub.
 */
cmd_invoker
find_stub (void)
{
	char	type_buf[100];
	int	x, bufpos;

	/* Figure out a name for the stub we want. */
	for (x = 0, bufpos = 0; x < cmd_argc; ++x)
		if (the_cmd_frame->argv[x].style->representation != cmd_none)
			type_buf[bufpos++] = the_cmd_frame->argv[x].style->representation;
	type_buf[bufpos] = '\0';

	/* Look for the stub. */
	for (x = 0; the_stubs[x].type; ++x)
		if (!stricmp (the_stubs[x].type, type_buf))
			break;

#if 0
	/* This causes an infinite loop */
	if (! the_stubs[x].stub) {
		io_error_msg("internal error: find_stub(%s) no match", type_buf);
	}
#endif

	return the_stubs[x].stub;
}
