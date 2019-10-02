#pragma once
/*
 * $Id: args.h,v 1.4 2000/08/10 21:02:49 danny Exp $
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
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Interacting with command arguments. */

#include <functional>

#include "global.h"


/*
 * The structure CMD_FUNC describes C functions that can be called
 * interactively. 
 *
 * FUNC_NAME: The name that `execute-function' uses.
 * FUNC_DOC: A `char **' of documentation for the function (NULL terminated).
 * FUNC_ARGS: Similar to an emacs interactive string, however,
 *	      rather than a newline separated string, Oleo uses a `char **'.
 * FUNC_FUNC: The C function the implements the command.
 * INIT_CODE: an array of macros or NULL to be executed before editting
 *	      begins on an interactive argument.
 *
 *      The syntax of FUNC_ARGS:
 *
 *      In this list, ? stands for an optional arbitrary edit buffer name,
 *      delimited by <>.  For example, to prompt for a file:
 *
 *	    "fw<spreadsheets>Save spreadsheet: "
 *
 * 	c? - prompt for a single character.
 *	     If written c', then the prompt times out after a small
 *	     number of seconds, and -1 is passed to the function.
 *	     If written c#, then the first character of the prompt string will
 *	     be passed to the function and the user will not be prompted.
 *      C? - Command name (or the address of a macro or name of a variable)
 *	d  - Prompt for a double precision floating point number.
 *	fr, fw - Get a file name and pass the opened FILE * to the
 *	         function. The file is closed when the function returns.
 *	fn - a file name.
 *	F  - read a cell
 * 	k  - read a keysequence (passed as `struct key').
 * 	K? - read the name of a keymap
 *      l  - last character typed to invoke this command.
 *	m  - the prompt should be a keymap name.  The user is given
 *	     an opertunity to interact using that map as the top-level
 * 	     map until exit-minibuffer is called.  This generates no
 * 	     arguments to the function. 
 *	     Written m', only one keysequence is read, and that is passed
 *	     as a struct keyseq *.
 *      M  - yes or no verify if the buffer has been modified.
 *	N? - Prefix arg as a number, or if none, read from the minibuffer
 *	     Input can be constrained to a particular range like this:
 *			"N[1,16]Format number [1-16]: "
 *	n? - like `N', but it always prompts (never uses the prefix).
 * 	p  - Prefix arg converted to number.  Does not do I/O.
 *	     However, this may be modified by following `p' with:
 *		? - pass 1 if a prefix arg was explicitly provided, else 0.
 *	        r - pass the raw prefix as `struct line *'  (NOT IMPLEMENTED)
 *	r? - Prompt for a range argument (use the mark if it is set).
 *	R? - Prompt for a range argument (don't use the mark, though).
 *	s? - Prompt the user for an arbitrary string.
 *  	S? - Any symbol.  If written S', then a NULL char * will be passed
 *	     if the user doesn't provide a symbol name.
 *      V  - enter a vi-like navigation state.
 *	     In this case, the_cmd_arg.inc_cmd should be set to a hook that
 *	     will repeatedly be called with two integer arguments (a direction
 *	     and repeat count).  (e.g. imove)
 *	w  - read a word (whitespace delimited).  w' means that the empty
 *	     string (normally forbidden) stands for NULL.
 *	#n - N should be an integer.  Pass N (as an int). Does not do I/O.
 *      .  - Return the {mk,}cu{row,col} from the command's frame.
 *	     Written .', the mark is ignored, and a one cell range of 
 *	     cu{row,col} is returned.
 *	$  - Edit a formula, beginning with the current cells formula as
 *	     default.  If you don't want the old formula as default, use $'
 *      [  - like c, except that input is constrained to a particular 
 *	     character set.  e.g.  "[upd][U]nprotected [P]rotected [D]efault "
 *	=  - pass the (expanded) prompt as a string arument.
 *
 * In addition, if the first character of the first FUNC_ARGS string is a `+'
 * then in expanded macro syntax, the name of the function may be followed
 * by an integer that will be made the value of the prefix argument.  For
 * example:  `{go-right 5}' because the FUNC_ARGS string for go-right is: `+p'.
 */

typedef void (*interactive_function) ();

struct cmd_func
{
	const char *func_name;
	char ** func_args;
	char ** init_code;
	interactive_function func_func;
	const char *cpp_func_name;
};

// cast it into the void for table look-up purposes
// Taken from:
// https://stackoverflow.com/questions/45009306/combining-using-and-reinterpret-cast
using vptr = void (*) ();
template <typename Func>
constexpr vptr to_vptr(Func && func)
{ return reinterpret_cast<vptr>(func); }

/*
 * There is a 2d (argv style) array of command functions.  Each 0 dimension
 * slice is called a `vector'.  The first vector contains the oleo built-ins.
 * When looking for a function of a particular name, the array is
 * searched in vector-major order.
 *
 */

extern int num_funcs;
extern struct cmd_func **the_funcs;

#define find_func(VEC,CMD,NAME) find_function(VEC,CMD,NAME,strlen(NAME))

extern int find_function (int * vec_out, struct cmd_func ** cmd_out, const char * name, int len);
extern void init_named_macro_strings (void);
/* See find_stub. */
enum command_arg_representation
{
  cmd_char  = 'c',
  cmd_float = 'd',
  cmd_file  = 'f',
  cmd_int   = 'i',
  cmd_key   = 'k',
  cmd_range = 'r',
  cmd_string = 's',
  cmd_none  = '0'
};

struct command_arg;

/* See comments in arg.c: */

//typedef const char * (*arg_verifier) (char ** end_out, struct command_arg *);
using arg_verifier = std::function<const char*(char**, struct command_arg*)>;
//typedef void (*arg_destroyer) (struct command_arg *);
using arg_destroyer = std::function<const void(struct command_arg *)>;

/* For every kind of prompt allowed in a FUNC_ARGS string, 
 * there exists a prompt_style that describes how that
 * kind of argument should be editted/passed:
 */

struct prompt_style
{
	arg_verifier verify;
	arg_destroyer destroy;
	enum command_arg_representation representation;
	char * keymap;
};



const char * char_verify (char ** end, struct command_arg * arg);
const char * symbol_verify (char ** end, struct command_arg * arg);
const char * word_verify (char ** end, struct command_arg * arg);
void symbol_destroy (struct command_arg * arg);
const char * command_verify (char ** end, struct command_arg * arg);
const char * read_file_verify (char ** end, struct command_arg * arg);
void read_file_destroy (struct command_arg * arg);
const char * write_file_verify (char ** end, struct command_arg * arg);
void write_file_destroy (struct command_arg * arg);
const char * keyseq_verify (char ** end, struct command_arg * arg);
const char * keymap_verify (char ** end, struct command_arg * arg);
const char * number_verify (char ** end, struct command_arg * arg);
const char * double_verify (char ** end, struct command_arg * arg);
const char * range_verify (char ** end, struct command_arg * arg);
const char * string_verify (char ** end, struct command_arg * arg);
const char * yes_verify (char ** end, struct command_arg * arg);
const char * incremental_cmd_verify (char ** end, struct command_arg * arg);
const char * format_verify (char ** end, struct command_arg * arg);
const char * noop_verify (char ** end, struct command_arg * arg);

#ifdef DEFINE_STYLES
#define DEFSTYLE(NAME,VER,DEST,REP,KEYMAP) \
struct prompt_style NAME = \
{ \
  VER, \
  DEST, \
  REP, \
  KEYMAP \
}
#else
#define DEFSTYLE(NAME,VER,DEST,REP,KEYMAP) extern struct prompt_style NAME
#endif

DEFSTYLE(char_style, char_verify, 0, cmd_int, "read-char");
DEFSTYLE(double_style, double_verify, 0, cmd_float, "read-float");
DEFSTYLE(format_style, format_verify, 0, cmd_int, "read-format");
DEFSTYLE(symbol_style,
	 symbol_verify, symbol_destroy, cmd_string, "read-symbol");
DEFSTYLE(word_style,
	 word_verify, symbol_destroy, cmd_string, "read-word");
DEFSTYLE(command_style,
	 command_verify, symbol_destroy, cmd_string, "read-symbol");
DEFSTYLE(read_file_style,
	 read_file_verify, read_file_destroy, cmd_file, "read-filename");
DEFSTYLE(write_file_style,
	 write_file_verify, write_file_destroy, cmd_file, "read-filename");
DEFSTYLE(keyseq_style, keyseq_verify, 0, cmd_key, "read-keyseq");
DEFSTYLE(keymap_style,
	 keymap_verify, symbol_destroy, cmd_string, "read-symbol"); 
DEFSTYLE(number_style, number_verify, 0, cmd_int, "read-integer");
DEFSTYLE(range_style, range_verify, 0, cmd_range, "read-range");
DEFSTYLE(formula_style, string_verify, 0, cmd_string, "read-formula");
DEFSTYLE(string_style, string_verify, 0, cmd_string, "read-string");
DEFSTYLE(file_name_style, string_verify, 0, cmd_string, "read-filename");
DEFSTYLE(yes_style, yes_verify, 0, cmd_none, "read-string");
DEFSTYLE(int_constant_style, 0, 0, cmd_int, 0);
DEFSTYLE(range_constant_style, 0, 0, cmd_range, 0);
DEFSTYLE(inc_cmd_style, incremental_cmd_verify, 0, cmd_none, "direction");
DEFSTYLE(mode_style, noop_verify, 0, cmd_none, 0);
