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
#include "io-abstract.h"
#include "io-utils.h"
#include "io-curses.h"
#include "ref.h"
#include "format.h"
#include "oleofile.h"
#include "sheet.h"
#include "spans.h"
#include "utils.h"

using std::cout;
using std::endl;


#define S (char *)


/* Indexed by MAGIC + 1 */
const int boundrymagic[3] = { MIN_ROW, NON_ROW, MAX_ROW };


/* A very basic command. */

void
noop (void)
{}


/* Commands that inser/delete rows/columns. */

void
insert_row (int repeat)
{
	insert_row_above(curow);
	//recalculate(1); // for things like areas
	io_repaint();
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
	io_repaint();

}


/* Front end to the window functions. */

void 
open_window (char *text)
{
	int hv;
	int where;

	while (*text == ' ')
		text++;

	if (*text == 'h' || *text == 'H')
		hv = 0;
	else if (*text == 'v' || *text == 'V')
		hv = 1;
	else
	{
		io_error_msg ("Open 'h'orizontal or 'v'ertical window, not '%s'", text);
		return;
	}
	where = atoi (text + 1);
	while (isspace (*text))
		++text;
	while (isalnum (*text))
		++text;
	while (isspace (*text))
		++text;
	if (*text == '%')
	{
		where *= (hv
				? (cwin->numr + (cwin->lh_wid ? label_rows : 0))
				: (cwin->numc + cwin->lh_wid));
		where /= 100;
	}
	io_win_open (hv, where);
}

void
hsplit_window (void)
{
	open_window (S "h50%");
}


void
vsplit_window (void)
{
	open_window (S "v50%");
}


void 
close_window (char *text)
{
	int num;

	num = atoi (text) - 1;

	if (num < 0 || num >= nwin)
	{
		io_error_msg ("Window %num?", text);
		return;
	}
	if (nwin == 1)
	{
		io_error_msg ("You can't close the last window!");
		return;
	}
	io_win_close (&wins[num]);
}

void 
delete_window (void)
{
	io_win_close (cwin);
}

void
delete_other_windows (void)
{
	if (nwin > 1)
	{
		CELLREF r = curow;
		CELLREF c = cucol;
		while (nwin > 1)
			io_win_close (cwin);
		io_move_cell_cursor (r, c);
	}
}


int
set_window_option (int set_opt, char *text)
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
	else if (!strincmp (text, "link", 4))
	{
		if (set_opt)
		{
			n = atoi (text + 4) - 1;
			if (n < 0 || n > nwin)
				io_error_msg ("Can't '%s': window # out of range", text);
			else
				cwin->link = n;
		}
		else
			cwin->link = -1;
	}
	else if (set_opt && !stricmp (text, "unlink"))
		cwin->link = -1;
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
suspend_oleo (void)
{
	if (using_curses)
	{
		stop_curses ();
#ifdef SIGTSTP
		kill (getpid (), SIGTSTP);
#else
#ifdef SIGSTOP
		kill (getpid (), SIGSTOP);
#endif
#endif
		cont_curses ();
	}
}

void
recalculate (int all)
{
}


void
kill_oleo (void)
{
	io_close_display(0);
	exit (0);
}


void
kill_all_cmd (void)
{
	clear_spreadsheet ();
	io_repaint ();
}

void
redraw_screen (void)
{
	io_repaint ();
}


/* Motion commands. */

void
shift_cell_cursor (int dir, int repeat)
{
	io_shift_cell_cursor (dir, repeat);
}


void
scroll_cell_cursor (int dir, int repeat)
{
	io_scroll_cell_cursor (dir, repeat);
}



void
goto_region (struct rng *r)
{
	(void) io_move_cell_cursor (r->lr, r->lc);

	if (r->hr != r->lr || r->hc != r->lc) {
		mkrow = r->hr;
		mkcol = r->hc;
	} else if (mkrow != NON_ROW) {
		mkrow = NON_ROW;
	}

	io_update_status ();
}

void
mark_cell_cmd (int popmk)
{
	if (popmk)
	{
		if (mkrow != NON_ROW)
		{
			struct rng rng;
			rng.lr = mkrow;
			rng.lc = mkcol;
			rng.hr = NON_ROW;
			rng.hc = NON_COL;
			goto_region (&rng);
		}
	} else {
		mkrow = curow;
		mkcol = cucol;
		io_update_status ();
	}
}

void
unmark_cmd (void)
{
	mkrow = NON_ROW;
	mkcol = NON_COL;

	io_update_status ();
}



/* Commands used to modify cell formulas. */



void
imove (struct rng * rng, int ch)
{
	//if ((ch > 0) && (ch != 27)) pushed_back_char = ch;

	goto_region (rng);
}

/* Incremental navigation
 *
 * This should be called in edit mode while gathering arguments 
 * for a complex command.  The expected the_cmd_arg.
 */

#define MIN(A,B)	((A) < (B) ? (A) : (B))



/* The commands that move to the extreme of a row[col] may also move
 * forward or backward some number of col[row], according to the prefix
 * arg.  This is the logic of that.  This function returns the new col[row]
 * and operates on the presumption that MIN_ROW == MIN_COL and
 * MAX_ROW == MAX_COL.
 */

static CELLREF
extreme_cmd_orth_motion (int count, CELLREF current)
{
  --count;
  if (count > (MAX_ROW - current))
    count =  (MAX_ROW - current);
  else if (-count > (current - MIN_ROW))
    count = (MIN_ROW - current);
  return current + count;
}


void
beginning_of_row (int count)
{
	struct rng rng;
	rng.lr = extreme_cmd_orth_motion (count, curow);
	rng.lc = MIN_COL;
	rng.hr = mkrow;
	rng.hc = mkcol;
	goto_region (&rng);
}


void
beginning_of_col (int count)
{
	struct rng rng;
	rng.lr = MIN_ROW;
	rng.lc = extreme_cmd_orth_motion (count, cucol);
	rng.hr = mkrow;
	rng.hc = mkcol;
	goto_region (&rng);
}



void
set_region_formula (struct rng * rng, char * str)
{
	CELLREF row, col;

	for (row = rng->lr; row <= rng->hr; ++row)
		for (col = rng->lc; col <= rng->hc; ++col)
		{
			char * error = new_value (row, col, str);
			if (!error)
				Global->modified = 1;
			if (error)
			{
				io_error_msg (error);
				return;
			}
		}
}



/*
 * Extended this to detect the extension of a file and have the right
 * read function process this.
 */
void
read_file_and_run_hooks (FILE * fp, int ismerge, const char * name)
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


void
write_cmd (FILE *fp, const char * name)
{
	if(name)
		FileSetCurrentFileName(name);
	oleo_write_file(fp, 0);
	Global->modified = 0;
}





/* PROT may be `d', `p', or `u'. */

void
set_region_protection (struct rng * rng, int prot)
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
			io_error_msg ("Bad argument to protect-region %c.", prot);
			break;
	}
}

void
set_region_alignment (struct rng * rng, int align)
{
	int fun = chr_to_jst (align);
	if (fun != -1)
		format_region (rng, -1, fun);
	else			/* if (main_map[align]!=BREAK_CMD) */
		io_error_msg ("Unknown Justify '%c'", align);
}

static void set_cell_alignment(char align)
{
	rng_t rng = {curow, cucol, curow, cucol};
	set_region_alignment(&rng, align);
}

void set_cell_alignment_left()
{
	set_cell_alignment('L');
}
void set_cell_alignment_right()
{
	set_cell_alignment('R');
}

/*
 * Lacking more knowledge of Oleo internals, we're hacking this thing to
 *	clean up the mess concerning the mixup of format and precision.
 *
 * The function set_region_format is a pass-through between the command
 *	loop and format_region(). As I don't know how to pass more than
 *	one parameter from str_to_fmt over the command loop into
 *	set_region_format, I'm leaving the mixup mentioned above as it
 *	is in that area. But *only* for passing the information to here.
 * Here the first thing we do is take the two values apart and call the
 *	clean API's with the right values.
 *
 * Sigh.
 * FIX ME
 *
 * Another place turned up that needs this : set_def_format().
 */
void
set_region_format (struct rng * rng, int fmt)
{
	int format = (fmt & FMT_MASK) >> FMT_SHIFT;
	//int precision = fmt & PREC_MASK;

	format_region (rng, format, -1); 
	//precision_region(rng, precision);
}




/* PROT may be `d', `p', or `u'. */

void
set_def_protection (int prot)
{
	if (isupper (prot))
		prot = tolower (prot);
	switch (prot)
	{
		case 'p':
			default_lock = LCK_LCK;
			break;
		case 'u':
			default_lock = LCK_UNL;
			break;
		default:
			io_error_msg ("Bad argument to set-default-protection %c.", prot);
			break;
	}
}

void
set_def_alignment (int align)
{
	int fun = chr_to_jst (align);
	if (fun == -1)
		io_error_msg ("Unknown justification.");

	default_jst = fun;
	io_repaint ();
}

void
set_def_format (int fmt)
{
	int format = (fmt & FMT_MASK) >> FMT_SHIFT;
	int precision = fmt & PREC_MASK;

	default_fmt = format;
	default_prc = precision;

	io_repaint ();
}

void
define_usr_fmt (int fmt, char * pos_h, char * neg_h, char * pos_t,
		char * neg_t, char * zero, char * comma, char * decimal,
		char * precision, char * scale)
{
	char * usr_buf[9];
	if (fmt < 1 || fmt > 16)
	{
		io_error_msg ("Format number out of range %d (should be in [1..16].",
				fmt);
		/* no return */
	}
	/* Vector to an older interface... */
	--fmt;
	usr_buf[0] = pos_h;
	usr_buf[1] = neg_h;
	usr_buf[2] = pos_t;
	usr_buf[3] = neg_t;
	usr_buf[4] = zero;
	usr_buf[5] = comma;
	usr_buf[6] = decimal;
	usr_buf[7] = precision;
	usr_buf[8] = scale;
	set_usr_stats (fmt, usr_buf);
}


void
auto_move (void)
{
	shift_cell_cursor (Global->auto_motion_direction, 1);
}


