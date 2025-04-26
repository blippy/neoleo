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
#include "regions.h"
#include "io-utils.h"
#include "format.h"
#include "oleofile.h"
#include "sheet.h"
#include "spans.h"
#include "utils.h"
#include "logging.h"

using std::cout;
using std::endl;

import errors;

//#define S (char *)



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
	ext = strrchr((char*)  name, '.');
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

