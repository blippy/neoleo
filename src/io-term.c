/*
 * $Id: io-term.c,v 1.61 2011/07/05 00:16:13 delqn Exp $
 *
 * Copyright © 1990, 1992, 1993, 1999, 2000, 2001, 2004, 2005 Free Software Foundation, Inc.
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

static char *rcsid = "$Id: io-term.c,v 1.61 2011/07/05 00:16:13 delqn Exp $";

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <errno.h>

#include "global.h"

#include "basic.h"
#include "cell.h"
#include "cmd.h"
#include "format.h"
#include "font.h"
#include "getopt.h"
#include "init.h"
#define DEFINE_IO_VARS 1
#include "io-abstract.h"
#include "io-curses.h"
#include "io-edit.h"
#include "io-generic.h"
#include "io-term.h"
#include "io-utils.h"
#include "io-x11.h"
#include "key.h"
#include "line.h"
#include "lists.h"
#define obstack_chunk_alloc ck_malloc
#define obstack_chunk_free free
#include "obstack.h"
#include "oleofile.h"
#include "print.h"
#include "ref.h"
#include "regions.h"
#include "window.h"
#include "funcs.h"
#include "graph.h"
#include "mdi.h"
#include "postscript.h"

#include "userpref.h"
#include "mysql.h"
#ifdef	HAVE_MOTIF
#include "io-motif.h"
#endif

#ifdef HAVE_LIBGTK
#include "gtk/gtk.h"
#endif 

#include "list.h"
#include "sc.h"
#include "sylk.h"

#if	ENABLE_NLS
extern char *gettext(char *);
#endif

/*
 * The ultimate global variable
 */
#if 1
struct OleoGlobal *Global = NULL;
#else
struct OleoGlobal	__tempGlobal,
			*Global = &__tempGlobal;
#endif

/* lds moved these here from io-x11 because even curses can set colors */
char *default_bg_color_name = "black";
char *default_fg_color_name = "white";

/* lds added to set colors & xterm size*/
#if 0
extern char geom_string[80];
extern int geom_w;
extern int geom_h;
#endif
int geom_w = 675;
int geom_h = 350;
char geom_string[80] = "675x350+0+0";

/* These are the hooks used to do file-io. */
void (*read_file) (FILE *, int) = oleo_read_file;
void (*write_file) (FILE *, struct rng *) = oleo_write_file;
int (*set_file_opts) (int, char *) = oleo_set_options;
void (*show_file_opts) () = oleo_show_options;

static char	option_separator = '\t';
static char	*option_format = NULL;
int		option_filter = 0;

static char short_options[] = "VqfxthsFSvbcw";
static struct option long_options[] =
{
	{"version",		0,	NULL,			'V'},
	{"quiet",		0,	NULL,			'q'},
	{"ignore-init-file",	0,	NULL,			'f'},
	{"nw",			0,	NULL,			'x'},
	{"no-toolkit",		0,	NULL,			't'},
	{"help",		0,	NULL,			'h'},
	{"separator",		1,	NULL,			's'},
	{"space",		0,	NULL,			'S'},
	{"format",		1,	NULL,			'F'},
	{"filter",		0,	NULL,			'-'},
	{"version",		0,	NULL,			'v'},
	{"bgcolor",       	1,	NULL,			'b'},
	{"bgfgcolor",       	2,	NULL,			'c'},
	{"window",		2,	NULL,			'w'},
	{NULL,			0,	NULL,			0}
};

/* Avoid needless messages to stdout. */
int spread_quietly = 0;

/* Avoid using Displays no matter what else. (-x --no-x) */
int no_gtk = 0;
int no_x = 0;
int no_curses = 0;
int no_motif = 0;

/* What kind of display? */
int using_x = 0;
int using_curses = 0;
int using_gtk = 0;
int using_motif = 0;


/* Cell size paramaters. */
unsigned int default_width = 8;
unsigned int default_height = 1;

/* These values are used by clear_spreadsheet ()
 * to restore the defaults.
 */
unsigned int saved_default_width = 8;
unsigned int saved_default_height = 1;

/* Other cell defaults: */
int default_jst = JST_LFT;
int default_fmt = FMT_GEN;
int default_prc = 0x0F;		/* FIX ME */
int default_lock = LCK_UNL;

/* Pointers to interesting cmd_func structures. */
struct cmd_func *end_macro_cmd;
struct cmd_func *digit_0_cmd;
struct cmd_func *digit_9_cmd;
struct cmd_func * break_cmd;
struct cmd_func * universal_arg_cmd;

/* A bland signal handler. */
static RETSIGTYPE
got_sig (int sig)
{
}

/*
 * Simple table of variables to set
 *	All these variables belong in a UserPreferences structure.
 */

struct UserPreferences UserPreferences;

/*
 * How to use this ?
 *
 * Set "name" to the string we'll find in the .oleorc.
 * Put a pointer to the variable to assign to in "var".
 *
 * If a function needs to be called upon encountering this option,
 *	put its address in "trigger".
 * If it's an integer, put the value to assign in "value".
 * If it's a string, set "copynext" to 1. The string in the option will be
 *	copied to the variable.
 *
 * Set "write" to 1 if this needs to be saved in .oleorc.
 *	Even then, only saved to file if integer has the "value" from the table,
 *	or if the string is non-empty.
 * Set cont to 1 if processing in do_set_option is to continue.
 */

#define ncolors 9

static char colorstr[ncolors][8] = {"black","red","green","orange","blue","magenta","cyan","white","yellow"};

static struct pref {
	char	*name;
	void	*var;  /* if copynext then actual type is char** else int* */
	int	value;
	void	(*trigger)(char *);
	int	copynext;
	int	write;
	int	cont;
} Preferences [] = {
	{ "a0",		&UserPreferences.a0,		1,	NULL,	0, 1, 1},
	{ "noa0",	&UserPreferences.a0,		0,	NULL,	0, 1, 1},
	{ "bgcolor",	&UserPreferences.bgcolor,	0,	NULL,	1, 0, 1},
	{ "file",	&UserPreferences.file_type,	0,	NULL,	1, 1, 1},
	{ "encoding",	&UserPreferences.encoding,	0,	OleoUserPrefEncoding,	1, 1, 0},
	{ NULL,	0 }
};

/* An parser for the language grokked by option setting commands. */

static int 
do_set_option (char *ptr)
{
  int	set_opt = 1;
  int	i, l;
  char	*p;

  while (*ptr == ' ')
    ptr++;

  for (l=0,p=ptr; *p && !isspace(*p); p++) l++;
 
  for (i=0; Preferences[i].name; i++)
	if (strncmp(ptr, Preferences[i].name, l) == 0) {
		if (Preferences[i].trigger != NULL)
			(Preferences[i].trigger)(ptr);

		if (Preferences[i].copynext) {
			*(char **)Preferences[i].var = strdup(ptr +
			  strlen(Preferences[i].name) + 1);
		} else if (Preferences[i].var)
			*(int *)Preferences[i].var = Preferences[i].value;

		if (Preferences[i].cont == 0)
			return 1;
		break;
	}

  if (!strincmp ("no", ptr, 2))
    {
      ptr += 2;
      set_opt = 0;
      while (*ptr == ' ')
	ptr++;
    }
  if (!stricmp ("auto", ptr))
    {
      Global->auto_recalc = set_opt;
      return 0;
    }
  if (!stricmp ("bkgrnd", ptr) || !stricmp ("background", ptr))
    {
      Global->bkgrnd_recalc = set_opt;
      return 0;
    }
  if (!stricmp ("a0", ptr))
    {
      Global->a0 = set_opt;
      io_repaint ();
      return 0;
    }
  if (!stricmp ("backup", ptr))
    {
      __make_backups = set_opt;
      return 0;
    }
  if (!stricmp ("bkup_copy", ptr))
    {
      __backup_by_copying = set_opt;
      return 0;
    }
  if (set_opt && !strincmp ("ticks ", ptr, 6))
    {
      ptr += 6;
      cell_timer_seconds = astol (&ptr);
      return 0;
    }
  if (set_opt && !strincmp ("print ", ptr, 6))
    {
      ptr += 6;
      print_width = astol (&ptr);
      return 0;
    }
//  if (set_opt && !strincmp ("file ", ptr, 5))
  if (set_opt && !strincmp ("filetype ", ptr, 9))
    {
      ptr += 9;
      if (!stricmp ("oleo", ptr))
	{
	  read_file = oleo_read_file;
	  write_file = oleo_write_file;
	  set_file_opts = oleo_set_options;
	  show_file_opts = oleo_show_options;
	  file_set_default_format(ptr);
	}
      else if (!stricmp ("sylk", ptr))
	{
	  Global->sylk_a0 = 1;
	  read_file = sylk_read_file;
	  write_file = sylk_write_file;
	  set_file_opts = sylk_set_options;
	  show_file_opts = sylk_show_options;
	  file_set_default_format(ptr);
	}
      else if (!stricmp ("sylk-noa0", ptr))
	{
	  Global->sylk_a0 = 0;
	  read_file = sylk_read_file;
	  write_file = sylk_write_file;
	  set_file_opts = sylk_set_options;
	  show_file_opts = sylk_show_options;
	  file_set_default_format(ptr);
	}
      else if (!stricmp ("sc", ptr))
	{
	  read_file = sc_read_file;
	  write_file = sc_write_file;
	  set_file_opts = sc_set_options;
	  show_file_opts = sc_show_options;
	  file_set_default_format(ptr);
	}
      else if (!stricmp ("list", ptr))
	{
	  Global->sl_sep = '\t';
	  read_file = list_read_file;
	  write_file = list_write_file;
	  set_file_opts = list_set_options;
	  show_file_opts = list_show_options;
	  /*if (ptr[4])
	    {
	    ptr+=4;
	    sl_sep=string_to_char(&ptr);
	    } */
	}
      else if (!stricmp ("csv", ptr))
	{
	  Global->sl_sep = ',';
	  read_file = list_read_file;
	  write_file = list_write_file;
	  set_file_opts = list_set_options;
	  show_file_opts = list_show_options;
	  file_set_default_format(ptr);
	  /*if (ptr[4])
	    {
	    ptr+=4;
	    sl_sep=string_to_char(&ptr);
	    } */
	}
      else
	io_error_msg ("Unknown file format %s", ptr);
      return 0;
    }
  if (set_window_option (set_opt, ptr) == 0)
    {
      if ((*set_file_opts) (set_opt, ptr))
	io_error_msg ("Unknown option '%s'", ptr);
      return 0;
    }
  return 1;
}

void
save_preferences(void)
{
	char	*home = getenv("HOME");
	char	*rc, *rc2;
	FILE	*fp;
	int	i;

	rc = malloc(strlen(home) + strlen(RCFILE) + 4);
	rc2 = malloc(strlen(home) + strlen(RCFILE) + 4);

	sprintf(rc, "%s/%s", home, RCFILE);
	sprintf(rc2, "%s/%s.bak", home, RCFILE);

	(void)unlink(rc2);
	rename(rc, rc2);

	free(rc2);

	fp = fopen(rc, "w");
	if (fp == NULL) {
		io_info_msg("Couldn't save preferences in %s: %s",
			rc, strerror(errno));
		free(rc);
		return;
	}

	for (i=0; Preferences[i].name; i++)
		if (Preferences[i].write) {
			if (Preferences[i].copynext) {
				if (strlen(*(char **)Preferences[i].var) != 0)
				    fprintf(fp, "set-option %s %s\n",
					Preferences[i].name,
					*(char **) Preferences[i].var);
			} else if (Preferences[i].value == *(int *)Preferences[i].var)
				fprintf(fp, "set-option %s\n",
					Preferences[i].name);
		}

	fclose(fp);

	io_info_msg("Saved preferences to %s", rc);

	free(rc);
}

void
set_options (char * ptr)
{
  if (do_set_option (ptr))
    io_recenter_cur_win ();
}

void 
show_options (void)
{
  int n;
  int fmts;
  char *data_buf[9];

  n = Global->auto_recalc;
  io_text_start ();

  io_text_line ("auto-recalculation: %s        Recalculate in background: %s",
		n ? " on" : "off", Global->bkgrnd_recalc ? "on" : "off");
  io_text_line ("make backup files:  %s        Copy files into backups:   %s",
	__make_backups ? " on" : "off", __backup_by_copying ? "on" : "off");

  io_text_line ("Asynchronous updates every %u ???",
		cell_timer_seconds);

  io_text_line ("Print width:      %5u", print_width);

  io_text_line ("");

  (*show_file_opts) ();

  io_text_line ("");
  show_window_options ();
  io_text_line ("");

  fmts = usr_set_fmts ();
  if (fmts)
    {
      io_text_line ("User-defined formats:");
      io_text_line ("Fmt    +Hdr    -Hdr   +Trlr   -Trlr    Zero   Comma Decimal  Prec         Scale");
      for (n = 0; n < 16; n++)
	{
	  if (fmts & (1 << n))
	    {
	      get_usr_stats (n, data_buf);
	      io_text_line ("%3d %7s %7s %7s %7s %7s %7s %7s %5s %13s",
			    n + 1,
			    data_buf[0],
			    data_buf[1],
			    data_buf[2],
			    data_buf[3],
			    data_buf[4],
			    data_buf[5],
			    data_buf[6],
			    data_buf[7],
			    data_buf[8]);
	    }
	}
    }
  else
    io_text_line ("No user-defined formats have been defined");

  io_text_finish ();
}


void
read_mp_usr_fmt (char *ptr)
{
  int usr_n = -1;
  int n_chrs = 0;
  char *p;
  char *buf[9];
  int i;

  for (i = 0; i < 9; i++)
    buf[i] = "";
  p = ptr;
  while (*p == ';')
    {
      *p++ = '\0';
      switch (*p++)
	{
	case 'N':
	  usr_n = astol (&p) - 1;
	  break;
	case 'H':
	  switch (*p++)
	    {
	    case 'P':
	      i = 0;
	      break;
	    case 'N':
	      i = 1;
	      break;
	    default:
	      goto badline;
	    }
	  goto count_chars;
	case 'T':
	  switch (*p++)
	    {
	    case 'P':
	      i = 2;
	      break;
	    case 'N':
	      i = 3;
	      break;
	    default:
	      goto badline;
	    }
	  goto count_chars;

	case 'Z':
	  i = 4;
	  goto count_chars;

	case 'C':
	  i = 5;
	  goto count_chars;

	case 'D':
	  i = 6;
	  goto count_chars;

	case 'P':
	  i = 7;
	  goto count_chars;

	case 'S':
	  i = 8;
	  goto count_chars;

	count_chars:
	  buf[i] = p;
	  n_chrs++;
	  while (*p && *p != ';')
	    {
	      p++;
	      n_chrs++;
	    }
	  break;

	default:
	badline:
	  io_error_msg ("Unknown OLEO line %s", ptr);
	  return;
	}
    }
  if (*p || usr_n < 0 || usr_n > 15)
    goto badline;

  set_usr_stats (usr_n, buf);
}

/* Modify this to write out *all* the options */
void
write_mp_options (FILE *fp)
{
  fprintf (fp, "O;%sauto;%sbackground;%sa0;ticks %d\n",
	   Global->auto_recalc ? "" : "no",
	   Global->bkgrnd_recalc ? "" : "no",
	   Global->a0 ? "" : "no",
	   cell_timer_seconds);
}

void 
read_mp_options (char *str)
{
  char *np;

  while ((np = (char *)index (str, ';')))
    {
      *np = '\0';
      (void) do_set_option (str);
      *np++ = ';';
      str = np;
    }
  if ((np = (char *)rindex (str, '\n')))
    *np = '\0';
  (void) do_set_option (str);
}


/* Commands related to variables. */

void
set_var (struct rng *val, char *var)
{
  char *ret;

  Global->modified = 1;
  ret = new_var_value (var, strlen(var), val);

  if (ret)
    io_error_msg ("Can't set-var %s: %s\n", var, ret);
}

void
unset_var (char *var)
{
  struct rng tmp_rng;
  struct var *v;

  v = find_var(var, strlen(var));

  if (!v || v->var_flags == VAR_UNDEF)
    {
      io_error_msg ("No variable named %s exists.", var);
    }
  else
    {
      tmp_rng.lr = tmp_rng.hr = NON_ROW;
      tmp_rng.lc = tmp_rng.hc = NON_COL;
      v = find_or_make_var(var, strlen(var));
      v->v_rng = tmp_rng;
      v->var_flags = VAR_UNDEF;
    }
}

void
show_var (char *ptr)
{
  struct var *v;
  int num;
  char buf[2048];

  while (*ptr == ' ')
    ptr++;
  for (num = 0; ptr[num] && ptr[num] != ' '; num++)
    ;

  v = find_var (ptr, num);
  if (!v || v->var_flags == VAR_UNDEF)
    {
      io_error_msg ("There is no '%s'", ptr);
      return;
    }
  if (Global->a0)
    {
      if (v->v_rng.lr != v->v_rng.hr || v->v_rng.lc != v->v_rng.hc)
	/* FOO */ sprintf (buf, "%s $%s$%u:$%s$%u", v->var_name, col_to_str (v->v_rng.lc), v->v_rng.lr, col_to_str (v->v_rng.hc), v->v_rng.hr);
      else
	/* FOO */ sprintf (buf, "%s $%s$%u", v->var_name, col_to_str (v->v_rng.lc), v->v_rng.lr);
    }
  else
    sprintf (buf, "%s %s", v->var_name, range_name (&(v->v_rng)));
  io_info_msg (buf);
}

static void
show_a_var (char *name, struct var *v)
{
  if (v->var_flags == VAR_UNDEF)
    return;
  if (Global->a0)
    {
      if (v->v_rng.lr != v->v_rng.hr || v->v_rng.lc != v->v_rng.hc)
	/* FOO */ io_text_line ("%-20s  $%s$%u:$%s$%u", v->var_name, col_to_str (v->v_rng.lc), v->v_rng.lr, col_to_str (v->v_rng.hc), v->v_rng.hr);
      else
	/* FOO */ io_text_line ("%-20s  $%s$%u", v->var_name, col_to_str (v->v_rng.lc), v->v_rng.lr);
    }
  else
    io_text_line ("%-20s  %s", v->var_name, range_name (&(v->v_rng)));
}

void
show_all_var (void)
{
  io_text_start ();
  io_text_line ("%-20s  Current Value", "Variable Name");
  for_all_vars (show_a_var);
  io_text_finish ();
}

static FILE * write_variable_fp = 0;

static void
write_a_var (char *name, struct var *v)
{
  CELLREF r, c;
  if (v->var_flags == VAR_UNDEF)
    return;
  r = v->v_rng.lr;
  c = v->v_rng.lc;
  if (v->var_flags == VAR_CELL)
    fprintf (write_variable_fp, "%s=%s\n",
	     v->var_name, cell_value_string (r, c, 1));
}

void
write_variables (FILE * fp)
{
  if (write_variable_fp)
    io_error_msg ("Can't re-enter write_variables.");
  else
    {
      write_variable_fp = fp;
      for_all_vars (write_a_var);
      write_variable_fp = 0;
    }
}

void
read_variables (FILE * fp)
{
  char buf[1024];
  int lineno = 0;
  while (fgets (buf, 1024, fp))
    {
      char * ptr;
      for (ptr = buf; *ptr && *ptr != '\n'; ++ptr)
	;
      *ptr = '\0';
      for (ptr = buf; isspace (*ptr); ptr++)
	;
      if (!*ptr || (*ptr == '#'))
	continue;
      {
	char * var_name = ptr;
	int var_name_len;
	char * value_string;
	while (*ptr && *ptr != '=')
	  ++ptr;
	if (!*ptr)
	  {
	    io_error_msg ("read-variables: format error near line %d.", lineno);
	    return;
	  }
	var_name_len = ptr - var_name;
	++ptr;
	value_string = ptr;
	{
	  struct var * var = find_var (var_name, var_name_len);
	  if (var)
	    {
	      switch (var->var_flags)
		{
		case VAR_UNDEF:
		  break;
		case VAR_CELL:
		  {
		    char * error = new_value (var->v_rng.lr, var->v_rng.lc,
					      value_string); 
		    if (error)
		      {
			io_error_msg (error);
			return;	/* actually, io_error_msg never returns. */
		      }
		    else
		      Global->modified = 1;
		    break;
		  }
		case VAR_RANGE:
		  io_error_msg ("read-variables (line %d): ranges not supported.",
				lineno);
		  return;
		}
	    }
	}
      }
      ++lineno;
    }
  if (!feof (fp))
    {
      io_error_msg ("read-variables: read error near line %d.", lineno);
      return;
    }
}

extern struct cmd_func cmd_funcs[];

static void
init_maps (void)
{
  num_maps = 0;
  the_maps = 0;
  map_names = 0;
  map_prompts = 0;

  the_funcs = ck_malloc (sizeof (struct cmd_func *) * 2);
  num_funcs = 1;
  the_funcs[0] = &cmd_funcs[0];

  find_func (0, &end_macro_cmd, "end-macro");
  find_func (0, &digit_0_cmd, "digit-0");
  find_func (0, &digit_9_cmd, "digit-9");
  find_func (0, &break_cmd, "break");
  find_func (0, &universal_arg_cmd, "universal-argument");

  create_keymap ("universal", 0);
  push_command_frame (0, 0, 0);
}

int 
add_usr_cmds (struct cmd_func *new_cmds)
{
  num_funcs++;
  the_funcs = ck_realloc (the_funcs, num_funcs * sizeof (struct cmd_func *));
  the_funcs[num_funcs - 1] = new_cmds;
  return num_funcs - 1;
}

/*
 * The variable below makes it possible for the program "what" to identify
 * which version of oleo this executable is.
 */

static char *what_version = "@(#)" PACKAGE " " VERSION ;

static void
show_usage (void)
{

  printf(_("This is %s %s\n\n"), PACKAGE, VERSION);

  printf(_("\
Usage: %s [OPTION]... [FILE]...\n\
"), PACKAGE);
  printf(_("\
\n\
  -h, --help               display this help and exit\n\
  -V, --version            output version information and exit\n\
  -q, --quiet              do not display startup messages\n\
  -f, --ignore-init-file   ignore settings defined in init file\n\
  -t, --no-toolkit         disable X toolkit\n\
  -x, --nw                 disable graphics and fallback to curses\n\
  -s x, --separator x	   set separator for 'list' file type to x\n\
  -S, --space		   set separator for 'list' file type to a space\n\
  -F x, --format x	   set default file type to x (oleo, list, sc  ...)\n\
  --filter		   read file from stdin, write to stdout on exit\n\
\n\
Report bugs to <bug-oleo@gnu.org>.\n\
"));
}

static RETSIGTYPE
continue_oleo (int sig)
{
  io_repaint ();
  if (using_curses)
    cont_curses ();
}

void InitializeGlobals(void)
{
  Global->display_opened = 0;
  Global->return_from_error = 0;

  /* Initialize stuff that's now in Global */
  Global->bkgrnd_recalc = 1;
  Global->auto_recalc = 1;
  Global->a0 = 0;
  Global->topclear = 0;
  Global->alarm_seconds = 1;
  Global->alarm_active = 1;

  /* From window.c */
  Global->scr_lines = 24;
  Global->scr_cols = 80;
  user_input = 1;
  user_status = 2;
  Global->input = 0;
  Global->status = 1;
  input_rows = 1;
  status_rows = 1;
  default_right_border = 0;
  default_bottom_border = 0;
  nwin = 0;
  cwin = 0;
  wins = 0;
  win_id = 1;

  Global->sylk_a0 = 1;

  Global->user_height_scale = 1.;
  Global->user_width_scale = 1.;
  Global->height_scale = 1.;
  Global->width_scale = 1.;

  Global->cell_font_point_size = 12;
  Global->block_on_getch = 1;

  Global->display_formula_mode = 0;
  Global->auto_motion_direction = magic_down;
  Global->sl_sep = '\t';

  Global->CurrentPrintDriver = &PostScriptPrintDriver;
  Global->zoom = 1.0;

  Global->mouse_id = 0;

  Global->oldLocale = NULL;

  UserPreferences.run_load_hooks = 1;
  /* End initialize */

  __make_backups = 1;
}

void
oleo_catch_signals(void (*h)(int))
{
  /*
   * These probably don't all need to be ifdef, but
   * it is harmless.
   */
#ifdef SIGCONT
  signal (SIGCONT, continue_oleo);
#endif
#ifdef SIGPIPE
  signal (SIGPIPE, h);
#endif

  /*
   * It makes little sense to block all signals when using X
   */
  if ((! using_x) && (! using_gtk) && (! using_motif)) {
#ifdef SIGINT
  signal (SIGINT, h);
#endif
#ifdef SIGQUIT
  signal (SIGQUIT, h);
#endif
#ifdef SIGILL
  signal (SIGILL, h);
#endif
#ifdef SIGEMT
  signal (SIGEMT, h);
#endif
#ifdef SIGBUS
  signal (SIGBUS, h);
#endif
#ifdef SIGSEGV
  signal (SIGSEGV, h);
#endif
#ifdef	SIGHUP
  signal(SIGHUP, h);
#endif
#ifdef	SIGTRAP
  signal(SIGTRAP, h);
#endif
#ifdef	SIGABRT
  signal(SIGABRT, h);
#endif
#ifdef	SIGFPE
  signal(SIGFPE, h);
#endif
#ifdef	SIGSYS
  signal(SIGSYS, h);
#endif
#ifdef	SIGALRM
  signal(SIGALRM, h);
#endif
#ifdef	SIGTERM
  signal(SIGTERM, h);
#endif
#ifdef	SIGXCPU
  signal(SIGXCPU, h);
#endif
#ifdef	SIGVTALRM
  signal(SIGVTALRM, h);
#endif
#ifdef	SIGPROF
  signal(SIGPROF, h);
#endif
#ifdef	SIGUSR1
  signal(SIGUSR1, h);
#endif
#ifdef	SIGUSR2
  signal(SIGUSR2, h);
#endif
  }
}

int ck_color(char *usercolor)
{
  int i;
  int ret;

  ret = 0;
  for(i=0 ; i<ncolors ; ++i)
    if (strcmp(usercolor,colorstr[i]) == 0)
      ret = i;

  return ret;
}
  
int 
main (int argc, char **argv)
{
  volatile int ignore_init_file = 0;
  FILE * init_fp[2];
  char * init_file_names[2];
  volatile int init_fpc = 0;
  int command_line_file = 0;	/* was there one? */

#if 0
  sleep(30);
#endif
#if 1	/* ENABLE_NLS */
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);
#endif

  MdiInitialize();	/* Create initial Global structure */
  PlotInit();
  AllocateDatabaseGlobal();
  InitializeGlobals();

  /* Set up the minimal io handler. */
#if 0
  cmd_graphics ();
#endif

  {
    int opt;

    while (1) {
	opt = getopt_long (argc, argv, short_options, long_options, (int *)0);
	if (opt == EOF)
		break;

#if 0
	if (opt)
		fprintf(stderr, PACKAGE " option %c\n", opt);
	else {
		fprintf(stderr, PACKAGE " optind %d option %s strange ...\n",
			optind, argv[optind]);
	}
#endif

	switch (opt)
	  {
	  case 'v':
	  case 'V':
	    printf(_("%s %s\n"), GNU_PACKAGE, VERSION);
            printf(_("Copyright (c) 1992-2000 Free Software Foundation, Inc.\n"));
            printf(_("%s comes with ABSOLUTELY NO WARRANTY.\n"), GNU_PACKAGE);
            printf(_("You may redistribute copies of %s\n"), PACKAGE);
            printf(_("under the terms of the GNU General Public License.\n"));
            printf(_("For more information about these matters, "));
            printf(_("see the files named COPYING.\n"));
	    exit (0);
	    break;
	  case 'b':
	    default_bg_color_name = colorstr[ck_color(argv[optind])];
//	    default_bg_color_name = colorstr[labs(atoi(argv[optind]))%ncolors];
	    if (strcmp(default_bg_color_name,"black") != 0)
	      default_fg_color_name = colorstr[0];
	    ++optind;
	    break;
	  case 'c':
	    default_bg_color_name = colorstr[ck_color(argv[optind])];
	    ++optind;
	    default_fg_color_name = colorstr[ck_color(argv[optind])];
	    if (strcmp(default_bg_color_name, default_fg_color_name) == 0)
	      if (strcmp(default_bg_color_name, "black") == 0)
	        default_fg_color_name = colorstr[1];
              else
	        default_fg_color_name = colorstr[0];
	    ++optind;
	    break;
	  case 'w':
          #ifndef X_DISPLAY_MISSING
	    Global->scr_cols = atoi(argv[optind]);
          #endif		  
	    ++optind;
          #ifndef X_DISPLAY_MISSING
	    Global->scr_lines = atoi(argv[optind]);
          #endif		  
	    ++optind;
          #ifndef X_DISPLAY_MISSING
	    geom_w = Global->scr_cols * 9;
	    geom_h = Global->scr_lines * 15;
	    sprintf(geom_string,"%dx%d+0+0",geom_w, geom_h);
          #endif		  
	    break;
	  case 'q':
	    spread_quietly = 1;
	    break;
	  case 'f':
	    ignore_init_file = 1;
	    break;
	  case 'x':
	    no_x = 1;
	    break;
          case 't':
            no_gtk = 1;
            no_motif = 1;
            break;
	  case 'h':
	    show_usage ();
	    exit (0);
	    break;
	  case 's':
	    option_separator = argv[optind][0];
#if 0
	fprintf(stderr, PACKAGE " set list file separator to %c\n", option_separator);
#endif
	    optind++;
	    list_set_separator(option_separator);
	    break;
	  case 'S':
	    option_separator = ' ';
	    list_set_separator(option_separator);
	    break;
	  case 'F':
#if 0
	fprintf(stderr, "F: optind %d argv[optind] '%s' optopt %d %c\n",
		optind, argv[optind], optopt, optopt);
#endif
	    option_format = optarg;
	    file_set_default_format(option_format);
	    break;
	  case '-':
		option_filter = 1;
		break;
	  }
      }
  }

  if (argc - optind > 1)
    {
      if (no_motif)
      {
        show_usage ();
        exit (1);
      }
    }

  init_infinity ();
  init_mem ();
  init_eval ();
  init_refs ();
  init_cells ();
  init_fonts ();
  init_info ();

  /* Find the init files. 
   * This is done even if ignore_init_file is true because
   * it effects whether the disclaimer will be shown.
   */
    {
      char *ptr, *home;
      
      home = getenv ("HOME");
      if (home)
	{
	  ptr = mk_sprintf ("%s/%s", home, RCFILE);
	  init_fp[init_fpc] = fopen (ptr, "r");
	  init_file_names[init_fpc] = ptr;
	  if (init_fp[init_fpc])
	    ++init_fpc;
	}
      
      init_fp[init_fpc] = fopen (RCFILE, "r");
      if (init_fp[init_fpc])
	++init_fpc;
    }

  FD_ZERO (&read_fd_set);
  FD_ZERO (&read_pending_fd_set);
  FD_ZERO (&exception_fd_set);
  FD_ZERO (&exception_pending_fd_set);

#if defined(HAVE_LIBGTK) && !defined(HAVE_MOTIF)
   if ((!no_gtk)&&(!no_x)) {

	gtk_init(&argc, &argv);

	gtk_graphics ();
	using_gtk = TRUE;
	no_x = TRUE;
	no_curses = TRUE;
   }  
#endif

#ifdef HAVE_MOTIF
   if ((!no_motif)&&(!no_x)) {

	motif_init(&argc, argv);

	using_gtk = FALSE;
	using_motif = TRUE;
	no_x = TRUE;
	no_curses = TRUE;
   }  
#endif

#ifndef X_DISPLAY_MISSING
  if (!no_x) {
    get_x11_args (&argc, argv);
    if (Global->io_x11_display_name) {
      x11_graphics ();
      using_x = TRUE;
      no_curses = TRUE;
    }
  } 
#endif /* X_DISPLAY_MISSING */
	  
  if (!no_curses) {
	  tty_graphics ();
	  using_curses = TRUE;
	  /* Allow the disclaimer to be read. */
	  /* if (!init_fpc && !spread_quietly)
		  sleep (5); */
  }

  io_open_display ();

  init_graphing ();
  PrintInit();
  OleoSetEncoding(OLEO_DEFAULT_ENCODING);

  if (setjmp (Global->error_exception))
  {
	fprintf (stderr, _("Error in the builtin init scripts (a bug!).\n"));
	io_close_display(69);
	exit (69);
  }
  else
  {
	  init_maps ();
	  init_named_macro_strings ();
	  run_init_cmds ();
  }

  oleo_catch_signals(&got_sig);

  /* Read the init file. */
  {
    volatile int x;
    for (x = 0; x < init_fpc; ++x)
      {
	if (setjmp (Global->error_exception))
	  {
	    fprintf (stderr, _("   error occured in init file %s near line %d."),
		     init_file_names [x], Global->sneaky_linec);
	    io_info_msg(_("   error occured in init file %s near line %d."),
		     init_file_names [x], Global->sneaky_linec);
	  }
	else
	  if (!ignore_init_file)
	    read_cmds_cmd (init_fp[x]);
	fclose (init_fp[x]);
      }
  }


  if (option_filter) {
    if (setjmp(Global->error_exception)) {
	fprintf (stderr, _("   error occured in stdin near line %d."),
	  Global->sneaky_linec);
	io_info_msg(_("   error occured in stdin near line %d."),
	  Global->sneaky_linec);
    } else
	read_file_and_run_hooks(stdin, 0, "stdin");
  } else if (argc - optind == 1) {
      FILE * fp;
      /* fixme: record file name */

    if ((fp = fopen (argv[optind], "r"))) {
	  if (setjmp (Global->error_exception)) {
	    fprintf (stderr, _(", error occured reading '%s'\n"), argv[optind]);
	    io_info_msg(_(", error occured reading '%s'\n"), argv[optind]);
	  } else
	    read_file_and_run_hooks (fp, 0, argv[optind]);
	  fclose (fp);
	  command_line_file = 1;
	  FileSetCurrentFileName(argv[optind]);
    } else {
	fprintf (stderr, _("Can't open %s: %s\n"), argv[optind], err_msg ());
	io_info_msg(_("Can't open %s: %s\n"), argv[optind], err_msg ());
    }

    optind++;
  }

  /* Force the command frame to be rebuilt now that the keymaps exist. */
  {
    struct command_frame * last_of_the_old = the_cmd_frame->next;
    while (the_cmd_frame != last_of_the_old)
      free_cmd_frame (the_cmd_frame);
    free_cmd_frame (last_of_the_old);
  }

  io_recenter_cur_win ();

  Global->display_opened = 1;

#if 0
  /* FIXME - Find better way of doing this */
  /* Display openning Copyright screen */
  if (!command_line_file)
    run_string_as_macro
      ("{pushback-keystroke}{builtin-help _NON_WARRANTY_}");
#endif /* 0 */

#ifdef	HAVE_MOTIF
  if (using_motif) {
    motif_build_gui();
    setjmp (Global->error_exception);
    motif_main_loop();
  } else {
    /* Compile with Motif but don't run with it. */
    while (1)
    {
      setjmp (Global->error_exception);
      command_loop (0, 0);
    }
  }
#else
  while (1)
    {
      setjmp (Global->error_exception);
      command_loop (0, 0);
    }
#endif
  return (0); /* Never Reached! */
}
