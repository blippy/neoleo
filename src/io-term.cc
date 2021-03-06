/*
 * Copyright (c) 1990, 1992, 1993, 1999, 2000, 2001 Free Software Foundation, Inc.
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

#include <assert.h>
#include <errno.h>
#include <iostream>
#include <libintl.h>
#include <ctype.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "global.h"
#include "utils.h"
#include "basic.h"
#include "cell.h"
#include "convert.h"
#include "format.h"
#define DEFINE_IO_VARS 1
#include "io-abstract.h"
#include "io-headless.h"
#include "io-2019.h"
#include "io-curses.h"
#include "io-term.h"
#include "io-utils.h"

using std::cerr;
using std::endl;


#include "sheet.h"
#include "oleofile.h"
#include "ref.h"
#include "regions.h"
#include "window.h"

using std::cout;
using std::endl;


const int base_default_jst = JST_RGT;

/* These are the hooks used to do file-io. */
//void (*read_file) (FILE *, int) = oleo_read_file;
void (*write_file) (FILE *, struct rng *) = oleo_write_file;
int (*set_file_opts) (int, char *) = oleo_set_options;


	EXTERN void
fairly_std_main_loop(void)
{
loop:
	try {
		main_command_loop_for2019();
	} catch (OleoJmp& e) { }
	goto loop;
}



/*
 * Simple table of variables to set
 *	All these variables belong in a UserPreferences structure.
 */

struct UserPreferences {
	int	a0,
		auto_recalc,
		backup,
		backup_copy,
		ticks,
		print;
	char	*file_type,
		*bgcolor;
	int	run_load_hooks;
	char	*encoding;
};

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
static struct pref {
	const char	*name;
	void	*var;
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
				ptr += strlen(Preferences[i].name) + 1;
				//((char *)Preferences[i].var) = strdup(ptr); mcarter
			} else if (Preferences[i].var)
				*((int *)Preferences[i].var) = Preferences[i].value;

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
		astol (&ptr);
		return 0;
	}
	if (set_opt && !strincmp ("print ", ptr, 6))
	{
		ptr += 6;
		//print_width = astol (&ptr);
		return 0;
	}
	if (set_opt && !strincmp ("file ", ptr, 5))
	{
		ptr += 5;
		if (!stricmp ("oleo", ptr))
		{
			//read_file = oleo_read_file;
			write_file = oleo_write_file;
			set_file_opts = oleo_set_options;
			//show_file_opts = oleo_show_options;
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
set_options (char * ptr)
{
	if (do_set_option (ptr))
		io_recenter_cur_win ();
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
		buf[i] = 0; // TODO Audit this for correctness.
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
	fprintf (fp, "O;%sauto;%sbackground;%sa0\n",
			Global->auto_recalc ? "" : "no",
			Global->bkgrnd_recalc ? "" : "no",
			Global->a0 ? "" : "no");
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

/* set an adapter stub that does nothing */
	void
_do_nothing_const_char_s(const char *s)
{
}

void
_do_nothing() { }; /* stub */

void _io_do_button_nothing(int r, int c, char *lbl, char *cmd) {};

void _io_append_message_nothing(bool beep, char *fmt, ...) {};

void _io_update_width_nothing(int col, int wid) {};

	void 
InitializeGlobals(void)
{
	FileSetCurrentFileName("unnamed.oleo");

	/* From window.c */
	user_input = 1;
	user_status = 2;
	input_rows = 1;
	status_rows = 1;
	default_right_border = 0;
	default_bottom_border = 0;
	nwin = 0;
	cwin = 0;
	wins = 0;
	win_id = 1;

	Global->auto_motion_direction = magic_down;

	__make_backups = 1;

	io_set_window_name = _do_nothing_const_char_s;
	io_run_main_loop = _do_nothing;
	io_do_button = _io_do_button_nothing;
	io_append_message = _io_append_message_nothing;
	io_update_width = _io_update_width_nothing;

}


	void
choose_display(bool force_cmd_graphics)
{
	using_curses = false;

	if(force_cmd_graphics || user_wants_headless) {
		headless_graphics();
		return;
	}

	tty_graphics ();
	using_curses = true;
}

