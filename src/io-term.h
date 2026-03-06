#ifndef IO_TERMH
#define IO_TERMH
/*
 * $Id: io-term.h,v 1.11 2011/07/05 00:16:13 delqn Exp $
 *
 * Copyright © 1992, 1993 Free Software Foundation, Inc.
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

#include <setjmp.h>
#include "global.h"

extern int using_x;
extern int using_curses;
extern int using_motif;
extern jmp_buf error_exception;

/* Cell size paramaters. */
extern unsigned int default_width;
extern unsigned int default_height;

/* These values are used by clear_spreadsheet ()
 * to restore the defaults.
 */
extern unsigned int saved_default_width;
extern unsigned int saved_default_height;

/* Other cell defaults: */
extern int default_jst;
extern int default_fmt;
extern int default_lock;


/* When printing ascii output, this controls page width. */
extern int modified;


extern struct cmd_func *end_macro_cmd;
extern struct cmd_func *digit_0_cmd;
extern struct cmd_func *digit_9_cmd;
extern struct cmd_func * break_cmd;
extern struct cmd_func * universal_arg_cmd;

extern struct cmd_func * break_cmd;
extern struct cmd_func * universal_arg_cmd;
extern struct cmd_func * end_macro_cmd;




extern void set_options (char * ptr);
extern void show_options (void);
extern void read_mp_usr_fmt (char *ptr);
extern void write_mp_options (FILE *fp);
extern void read_mp_options (char *str);
extern void set_var (struct rng *var, char * val);
extern void unset_var (char *var);
extern void show_var (char *ptr);
extern void show_all_var (void);
extern void write_variables (FILE * fp);
extern void read_variables (FILE * fp);
extern int add_usr_cmds (struct cmd_func *new_cmds);
extern int main (int argc, char **argv);
extern void InitializeGlobals(void);

#ifndef FALSE
#define FALSE 0
#endif /* FALSE */

#ifndef TRUE
#define TRUE 1
#endif /* TRUE */

extern int option_filter;

extern void oleo_catch_signals(void (*h)(int));

#endif /* IO_TERMH */
