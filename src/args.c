/*
 * $Id: args.c,v 1.6 2000/08/10 21:02:49 danny Exp $
 *
 * Copyright © 1993, 1999 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include <ctype.h>
#include "global.h"
#define DEFINE_STYLES	1
#include "args.h"
#include "cmd.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "io-edit.h"
#include "io-utils.h"
#include "format.h"



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

char *
char_verify (char ** end, struct command_arg * arg)
{
  if (!**end)
    return "No character specified";
  else
    {
      int ch = string_to_char (end);
      if (ch < 0)
	{
	  setn_edit_line ("", 0);
	  return "Illegal character constant.";
	}
      else
	{
	  arg->val.integer = ch;
	  return 0;
	}
    }
}

char *
symbol_verify (char ** end, struct command_arg * arg)
{
  char * e = *end;
  char * start = *end;
  if (isalpha (*e) || (*e == '-') || (*e == '_') || (*e == (Global->a0 ? '$' : ':')))
    {
      while (isalpha(*e) || isdigit(*e) || (*e == '-') || (*e == '_')
	     || (*e == (Global->a0 ? '$' : ':')))
	++e;
      if (!isspace(*e) && *e)
	goto bad_symbol;
      *end = e;
      arg->val.string = (char *)ck_malloc (e - start + 1);
      if (e - start)
	bcopy (start, arg->val.string, e - start);
      arg->val.string[e - start] = '\0';
      return 0;
    }
 bad_symbol:
  if (arg->arg_desc[1] == '\'')
    {
      arg->val.string = 0;
      return 0;
    }
  else
    return "Invalid symbol name.";
}

char *
word_verify (end, arg)
     char ** end;
     struct command_arg * arg;
{
  char * e = *end;
  char * start = *end;
  if (!isspace (*e))
    {
      while (*e && !isspace(*e))
	++e;
      *end = e;
      arg->val.string = (char *)ck_malloc (e - start + 1);
      if (e - start)
	bcopy (start, arg->val.string, e - start);
      arg->val.string[e - start] = '\0';
      return 0;
    }
  else if (arg->arg_desc[1] == '\'')
    {
      arg->val.string = 0;
      return 0;
    }
  else
    return "Invalid symbol name.";
}

void
symbol_destroy (struct command_arg * arg)
{
  if (arg->val.string)
    ck_free (arg->val.string);
}

char *
command_verify (char ** end, struct command_arg * arg)
{
  char * error = symbol_verify (end, arg);
  char * str;
  if (error)
    return error;
  str = arg->val.string;
  if (!(find_function (0, 0, arg->val.string, strlen(arg->val.string))
        && get_abs_rng (&str, 0)))
    return 0;
  else
    return "Not a command or macro address.";
}

char * 
read_file_verify (char ** end, struct command_arg * arg)
{
  FILE * fp = xopen_with_backup (arg->text.buf, "r");
  *end = 0;
  if (!fp)
    {
      io_error_msg ("Can't open file '%s':%s", arg->text.buf, err_msg ());
      return "";
    }
  else
    {
      arg->val.fp = fp;
      return 0;
    }
}

void
read_file_destroy (struct command_arg * arg)
{
  int num;
  num = xclose (arg->val.fp);
  if (num)
    io_error_msg ("Can't close '%s': Error code %d: %s",
		  arg->text.buf, num, err_msg ());
}


char * 
write_file_verify (char ** end, struct command_arg * arg)
{
  FILE * fp = xopen_with_backup (arg->text.buf, "w");
  *end = 0;
  if (!fp)
    {
      io_error_msg ("Can't open file '%s':%s", arg->text.buf, err_msg ());
      return "";
    }
  else
    {
      arg->val.fp = fp;
      return 0;
    }
}

void
write_file_destroy (struct command_arg * arg)
{
  int num;

  num = xclose (arg->val.fp);
  if (num)
    io_error_msg ("Can't close '%s': Error code %d: %s",
		  arg->text.buf, num, err_msg ());
}

/* As a special case, cmd_loop makes sure that keyseq arguments are only read
 * interactively.
 */

char *
keyseq_verify (char ** end, struct command_arg * arg)
{
  *end = 0;
  return 0;
}


char *
keymap_verify (char ** end, struct command_arg * arg)
{
  char * start = *end;
  char * error = symbol_verify (end, arg);
  int id;
  if (error)
    return error;
  id = map_idn (start, *end - start);
  return (id >= 0
	  ? (char *) 0
	  : "No such keymap.");
}


char *
number_verify (char ** end, struct command_arg * arg)
{
  char * e = *end;

  while (*e && isspace (*e))
    ++e;
  if (isdigit(*e) || (*e == '-'))
    {
      arg->val.integer = astol (end);
      if (arg->arg_desc[1] == '[')
	{
	  char * prompt = arg->arg_desc + 2;
	  {
	    long low = 0;
	    long high = -1;
	    low = astol (&prompt);
	    while (*prompt && isspace (*prompt))  ++prompt;
	    if (*prompt == ',') ++prompt;
	    high = astol (&prompt);
	    while (*prompt && isspace (*prompt))  ++prompt;
	    if (*prompt == ']') ++prompt;
	    if (   (low > arg->val.integer)
		|| (high < arg->val.integer))
	      io_error_msg ("Out of range %d (should be in [%d - %d]).",
			    arg->val.integer, low, high); /* no return */
	  }
	}
      return 0;
    }
  else
    return "Not a number.";
}


char *
double_verify (char ** end, struct command_arg * arg)
{
  char * e = *end;

  while (*e && isspace (*e))
    ++e;
  if (isdigit(*e) || ((*e == '-') && isdigit (*(e + 1))))
    {
      arg->val.floating = astof (end);
      return 0;
    }
  else
    return "Not a number.";
}


char * 
range_verify (char ** end, struct command_arg * arg)
{
  union command_arg_val * val = &arg->val;
  *end = arg->text.buf;
  if (get_abs_rng (end, &val->range))
    return "Not a range.";
  else
    return 0;
}

char * 
string_verify (char ** end, struct command_arg * arg)
{
  arg->val.string = arg->text.buf;
  *end = 0;
  return 0;
}

/* Unlike most verify functions, this
 * one may destroy the command frame that it is 
 * operating on.  It's purpose is to allow user's
 * to abort commands. 
 */
char * 
yes_verify (char ** end, struct command_arg * arg)
{
  if (words_imatch (end, "no"))
    {
      pop_unfinished_command ();
      return "Aborted.";
    }
  else if (words_imatch (end, "yes"))
    return 0;
  else
    {
      setn_edit_line ("", 0);
      return "Please answer yes or no.";
    }
}

char *
incremental_cmd_verify (char ** end, struct command_arg * arg)
{
  return 0;
}


char *
menu_verify (char ** end, struct command_arg * arg)
{
  char * error = char_verify (end, arg);
  if (error)
    return error;

  {
    int pick = arg->val.integer;
    char * key = arg->arg_desc + 1;
    while (*key && (*key != ']'))
      {
	if (*key == '\\')
	  {
	    ++key;
	    if (!*key)
	      break;
	  }
	if (pick == *key)
	  return 0;
	else
	  ++key;
      }
    setn_edit_line ("", 0);
    return "No such menu option.";
  }
}


char *
format_verify (char ** end, struct command_arg * arg)
{
  arg->val.integer = str_to_fmt (*end);
  if (arg->val.integer < 0)
    return "Unknown format.";
  *end = 0;
  return 0;
}


char *
noop_verify (char ** end, struct command_arg * arg)
{
  return 0;
}
