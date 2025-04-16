/*
 * Copyright (c) 1993, 2000 Free Software Foundation, Inc.
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

#include <assert.h>
#include <ctype.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "basic.h"
#include "convert.h"
#include "cmd.h"
#include "regions.h"
#include "window.h"
#include "io-term.h"
//#include "io-abstract.h"
#include "io-utils.h"
#include "io-curses.h"
#include "ref.h"
#include "format.h"
#include "oleofile.h"
#include "sheet.h"
#include "spans.h"
#include "utils.h"
#include "logging.h"

using std::cout;
using std::endl;


#define S (char *)



/* A very basic command. */

void
noop (void)
{}


/* Commands that inser/delete rows/columns. */

void
insert_row (int repeat)
{
	insert_row_above(curow);
}

void
insert_1row()
{
	insert_row(1);
}


void
delete_row (int repeat)
{
	delete_sheet_row(curow);
}


/* Front end to the window functions. */



int set_window_option (int set_opt, char *text)
{
	int stat;
	int n;
	static struct opt
	{
		char *text;
		int bits;
	}
	opts[] =
	{
		{ S "reverse", WIN_EDGE_REV } ,
		{ S "standout", WIN_EDGE_REV } ,
		{ S "page", WIN_PAG_HZ | WIN_PAG_VT } ,
		{ S "pageh", WIN_PAG_HZ } ,
		{ S "pagev", WIN_PAG_VT } ,
		{ S "lockh", WIN_LCK_HZ } ,
		{ S "lockv", WIN_LCK_VT } ,
		{ S "edges", WIN_EDGES }
	};
	if ((stat = (!strincmp (text, "status", 6) && isspace (text[6])))
			|| (!strincmp (text, "input", 5) && isspace (text[5])))
	{
		int n = set_opt ? atoi (text + 6 + stat) : 0;	/* A little pun. */
		int new_inp = stat ? user_input : n;
		int new_stat = stat ? n : user_status;
		io_set_input_status (new_inp, new_stat, 1);
	}
	else if (set_opt && !strincmp (text, "row ", 4))
	{
		text += 4;
		curow = astol (&text);
	}
	else if (set_opt && !strincmp (text, "col ", 4))
	{
		text += 4;
		cucol = astol (&text);
	}
	else
	{
		constexpr int nopts = sizeof (opts) / sizeof (struct opt);
		for (n = 0; n < nopts; n++)
			if (!stricmp (text, opts[n].text))
			{
				if (set_opt)
					cwin->flags |= opts[n].bits;
				else
					cwin->flags &= ~opts[n].bits;
				break;
			}

		if (n == nopts)
			return 0;
	}
	return 1;
}


void
recenter_window (void)
{
	io_recenter_cur_win ();
}
/* Trivial front-end commands. */




void
recalculate (int all)
{
}




/*
 * Extended this to detect the extension of a file and have the right
 * read function process this.
 */
void read_file_and_run_hooks (FILE * fp, int ismerge, const char * name)
{
	char	*ext = NULL;
	if (!ismerge)
	{
		FileSetCurrentFileName(name); // callee duplicates string
	}
	ext = strrchr(S name, '.');
	if (! ext) {
		read_file_generic(fp, ismerge, NULL, name);
	} else {
		ext++;
		read_file_generic(fp, ismerge, ext, name);
	}

}


void write_cmd (FILE *fp, const char * name)
{
	if(name) FileSetCurrentFileName(name);
	oleo_write_file(fp, 0);
	Global->modified = 0;
}





/* PROT may be `d', `p', or `u'. */

void set_region_protection (struct rng * rng, int prot)
{
	if (isupper (prot))
		prot = tolower (prot);
	switch (prot)
	{
		case 'd':
			lock_region (rng, LCK_DEF);
			break;
		case 'p':
			lock_region (rng, LCK_LCK);
			break;
		case 'u':
			lock_region (rng, LCK_UNL);
			break;
		default:
			raise_error("Bad argument to protect-region %c.", prot);
			break;
	}
}

void set_region_alignment (struct rng * rng, int align)
{
	int fun = chr_to_jst (align);
	if (fun != -1)
		format_region (rng, -1, fun);
	else			/* if (main_map[align]!=BREAK_CMD) */
		raise_error("Unknown Justify '%c'", align);
}

static void set_cell_alignment(char align)
{
	rng_t rng = {curow, cucol, curow, cucol};
	set_region_alignment(&rng, align);
}


// NB probably belongs somewhere else
void set_cell_toggle_bold()
{	
	auto cp = find_or_make_cell();
	cp->cell_flags.bold = ! cp->cell_flags.bold;
	Global->modified = true;
}

void set_cell_toggle_percent()
{
	auto cp = find_or_make_cell();

	auto &fmt = cp->cell_flags.cell_format;
	if(fmt ==  FMT_PCT)
		fmt = FMT_DEF;
	else
		fmt = FMT_PCT;

	Global->modified = true;
}

void set_cell_toggle_italic()
{
	
	auto cp = find_or_make_cell();
	cp->cell_flags.italic = ! cp->cell_flags.italic;
	Global->modified = true;
}



void set_cell_alignment_left()
{
	set_cell_alignment('L');
}
void set_cell_alignment_right()
{
	set_cell_alignment('R');
}




void
set_def_format (int fmt)
{
	int format = (fmt & FMT_MASK) >> FMT_SHIFT;
	int precision = fmt & PREC_MASK;

	default_fmt = format;
	default_prc = precision;

}

