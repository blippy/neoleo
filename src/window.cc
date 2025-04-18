/*
 * Copyright (c) 1992, 1993, 1999, 2001 Free Software Foundation, Inc.
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


#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "convert.h"
#include "window.h"
#include "io-abstract.h"
#include "io-utils.h"
#include "io-term.h"
#include "cmd.h"
#include "sheet.h"
#include "regions.h"
#include "spans.h"
#include "io-curses.h"


/* Low level window operators. */

#define MIN_WIN_HEIGHT(W) (W->bottom_edge_r \
			   + label_rows * (W->flags & WIN_EDGES ? 2 : 1))

#define MIN_WIN_WIDTH(W) (W->right_edge_c \
			  + label_emcols * (W->flags & WIN_EDGES ? 6 : 1))
#define MIN_CWIN_HEIGHT  MIN_WIN_HEIGHT(cwin)
#define MIN_CWIN_WIDTH  MIN_WIN_WIDTH(cwin)


struct tmp_s { int l, r, u, b; };

static void win_io_repaint()
{

}

static void 
do_close_window (int num)
{
	if (nwin == 1)
	{
		raise_error("Attempt to delete sole ordinary window.");
		return;
	}
	io_recenter_all_win ();
	return;
}

int
win_label_cols (struct window * win, CELLREF hr)
{
	int lh;

	if ((win_flags & WIN_EDGES) == 0)
		lh = 0;

	else if ((win_flags & WIN_PAG_HZ) || hr >= 100)
		lh = 5;
	else if (hr > 10)
		lh = 4;
	else
		lh = 3;
	lh *= label_emcols;
	return lh;
}

int
win_label_rows (struct window * win)
{
	return (win_flags & WIN_EDGES) ? label_rows : 0;
}

void set_numcols (struct window *win, CELLREF hr)
{
	int lh = win_label_cols (win, hr);
	win->win_over -= win->lh_wid - lh;
	win->numc += win->lh_wid - lh;
	win->lh_wid = lh;
}


static void 
page_axis (CELLREF cur, int (*get) (CELLREF), int total, CELLREF *loP, CELLREF *hiP)
{
	CELLREF lo, hi;
	int w, ww;

	lo = hi = MIN_ROW;
	w = (*get) (hi);
	for (;;)
	{
		ww = (*get) (hi + 1);
		while (w + ww <= total && hi < MAX_ROW)
		{
			hi++;
			w += ww;
			ww = (*get) (hi + 1);
		}
		if (hi >= cur)
			break;
		hi++;
		lo = hi;
		w = ww;
	}
	if (lo > cur || hi > MAX_ROW)
		raise_error("Can't find a non-zero-sized cell page_axis");
	*loP = lo;
	*hiP = hi;
}


static void 
recenter_axis (CELLREF cur, int (*get) (CELLREF), int total, CELLREF *loP, CELLREF *hiP)
{
	CELLREF lo, hi;
	int tot;
	int n;
	int more;

	lo = hi = cur;
	n = tot = (*get) (cur);
	do
	{
		if (lo > MIN_ROW && tot + (n = (*get) (lo - 1)) <= total)
		{
			--lo;
			tot += n;
			more = 1;
		}
		else
			more = 0;
		if (hi < MAX_ROW && tot + (n = (*get) (hi + 1)) <= total)
		{
			hi++;
			tot += n;
			more++;
		}
	}
	while (more);
	*loP = lo;
	*hiP = hi;
}

// FN recenter_window 
void  recenter_window (struct window *win)
{
	if (win_flags & WIN_PAG_VT)
		page_axis (curow, get_scaled_height, win->numr,
				&(win->screen.lr), &(win->screen.hr));
	else
		recenter_axis (curow, get_scaled_height, win->numr,
				&(win->screen.lr), &(win->screen.hr));
	set_numcols (win, win->screen.hr);
	if (win_flags & WIN_PAG_HZ)
		page_axis (cucol, get_scaled_width, win->numc,
				&(win->screen.lc), &(win->screen.hc));
	else
		recenter_axis (cucol, get_scaled_width, win->numc,
				&(win->screen.lc), &(win->screen.hc));
}
// FN-END

void io_recenter_cur_win (void)
{
	io_recenter_named_window (cwin);
	win_io_repaint_win(cwin);
}

void io_recenter_all_win(void)
{
	if (!nwin) return;
	io_recenter_named_window (cwin);
	win_io_repaint ();
}
void io_recenter_named_window(struct window *w)
{
	recenter_window(w);
}


static void find_nonzero (CELLREF *curp, CELLREF lo, CELLREF hi, int (*get) (CELLREF))
{
	CELLREF cc;
	int n;

	cc = *curp;

	if (cc < hi)
	{
		cc++;
		while ((n = (*get) (cc)) == 0)
		{
			if (cc == hi)
				break;
			cc++;
		}
		if (n)
		{
			*curp = cc;
			return;
		}
	}
	if (cc > lo)
	{
		--cc;
		while ((n = (*get) (cc)) == 0)
		{
			if (cc == lo)
				break;
			--cc;
		}
		if (n)
		{
			*curp = cc;
			return;
		}
	}
}



/* External window interface */





void io_set_input_status (int inp, int stat, int redraw)
{
	int inpv = inp < 0 ? -inp : inp;
	int inpsgn = inp == inpv ? 1 : -1;
	int statv = stat < 0 ? -stat : stat;
	int statsgn = stat == statv ? 1 : -1;
	int new_ui;
	int new_us;
	int new_inp;
	int new_stat;

	if (inpv == 0 || inpv > 2)
		raise_error("Bad input location %d; it should be +/- 1, or 2", inp);
	else if (statv > 2)
		raise_error("Bad status location %d; it should be +/- 0, 1, or 2",
				inp);
	else
	{
		new_ui = inp;
		new_us = stat;
		if (inpsgn != statsgn)
		{
			if (inpsgn > 0)
			{
				new_inp = 0;
				new_stat = Global->scr_lines - status_rows;
			}
			else
			{
				new_inp = Global->scr_lines - input_rows;
				new_stat = 0;
			}
		}
		else
		{
			if (inpv > statv)
			{
				new_inp = new_us ? status_rows : 0;
				new_stat = 0;
			}
			else
			{
				new_inp = 0;
				new_stat = input_rows;
			}
			if (inpsgn < 0)
			{
				new_stat = Global->scr_lines - new_stat - status_rows;
				new_inp = Global->scr_lines - new_inp - input_rows;
			}
		}
		if (redraw)
		{
			int vchange =
				(((new_ui > 0 ? input_rows : 0)
				  + (new_us > 0 ? status_rows : 0))
				 - ((user_input > 0 ? input_rows : 0)
					 + (user_status > 0 ? status_rows : 0)));
			int grow = (user_status
					? (new_us ? 0 : status_rows)
					: (new_us ? -status_rows : 0));
			int cell_top =
				((user_status > 0 ? status_rows : 0)
				 + (user_input > 0 ? input_rows : 0));

			assert(grow>=0) ;
#if 0
			 if (grow < 0)
			{
				int x;
re:
				for (x = 0; x < nwin; ++x)
				{
					int top = cwin->win_down - win_label_rows(cwin);
					if (cell_top == top && (cwin->numr <= -grow))
					{
						do_close_window (x);
						goto re;
					}
				}
			}
#endif 

			if (grow)
			{
				//int x;
				//for (x = 0; x < nwin; ++x)
				//{
					int top = cwin->win_down - win_label_rows (cwin);
					if (cell_top == top) 
						cwin->numr -= vchange;
				//}
			}
			if (vchange)
			{
				//int x;
				//for (x = 0; x < nwin; ++x)
					cwin->win_down += vchange;
			}
			win_io_repaint ();
		}
		user_input = new_ui;
		user_status = new_us;
		Global->input = new_inp;
		Global->status = new_stat;
	}
}


void io_pr_cell (CELLREF r, CELLREF c, CELL *cp)
{
	//if(!the_cmd_frame) return; // maybe running headless
	//if(running_headless()) return;
	if(cwin == 0) return; // maybe we're running headless
	//struct window *win;

	//for (win = wins; win < &wins[nwin]; win++)
	//{
		if (r < cwin->screen.lr || r > cwin->screen.hr || c < cwin->screen.lc || c > cwin->screen.hc) return;
		cur_io_pr_cell_win (cwin, r, c, cp);
	//}
}


void io_win_close (struct window *win)
{
	do_close_window (0); // 25/4
}

void io_move_cell_cursor (CELLREF rr, CELLREF cc)
{
	if (rr < cwin->screen.lr || rr > cwin->screen.hr
			|| cc < cwin->screen.lc || cc > cwin->screen.hc)
	{
		curow = rr;
		cucol = cc;
		recenter_window (cwin);
		win_io_repaint_win (cwin);
	}
	else
	{
		win_io_hide_cell_cursor ();
		curow = rr;
		cucol = cc;
		cur_io_display_cell_cursor ();
		cur_io_update_status ();
	}
	if (get_scaled_width (cucol) == 0)
		find_nonzero (&cucol, cwin->screen.lc, cwin->screen.hc, get_scaled_width);
	if (get_scaled_height (curow) == 0)
		find_nonzero (&curow, cwin->screen.lr, cwin->screen.hr, get_scaled_height);
}

void io_shift_cell_cursor (dirn way, int repeat) // FN
{
	CELLREF c = cucol;
	CELLREF r = curow;
	int w = 0;
	int over;
	int down;

	//over = colmagic[dirn] * repeat;
	//down = rowmagic[dirn] * repeat;
	switch (way) {
		case dirn::right:
		over = repeat;
		while (c < MAX_COL && over-- > 0)
		{
			c++;
			while ((w = get_scaled_width (c)) == 0 && c < MAX_COL) c++;
		}
		if (over > 0 || c == cucol || w == 0) raise_error("Can't go right");
		break;
		
		case dirn::left :
		over = -repeat;		
		while (c > MIN_COL && over++ < 0)
		{
			--c;
			while ((w = get_scaled_width (c)) == 0 && c > MIN_COL) --c;
		}
		if (over < 0 || c == cucol || w == 0) raise_error("Can't go %s", "left");
		break;
		
		case dirn::down:
		down = repeat;
		while (r < MAX_ROW && down-- > 0)
		{
			r++;
			while ((w = get_scaled_height (r)) == 0 && r < MAX_ROW) r++;
		}
		if (down > 0 || r == curow || w == 0) raise_error("Can't go down");
		break;

		case dirn::up:
		down = -repeat;
		while (r > MIN_ROW && down++ < 0)
		{
			--r;
			while ((w = get_scaled_height (r)) == 0 && r > MIN_ROW) --r;
		}
		if (down < 0 || r == curow || w == 0) raise_error("Can't go up");
		break;
	}

	io_move_cell_cursor (r, c);
}



void io_set_win_flags (struct window *w, int f)
{
	if ((f & WIN_EDGES) && !(win_flags & WIN_EDGES))
	{
		if (w->numr < 2 || w->numc < 6)
			raise_error("Edges wouldn't fit!");
		w->win_down++;
		w->numr--;
		set_numcols (w, w->screen.hr);
	}
	else if (!(f & WIN_EDGES) && (win_flags & WIN_EDGES))
	{
		w->win_over -= w->lh_wid;
		w->numc += w->lh_wid;
		w->lh_wid = 0;
		w->win_down--;
		w->numr++;
	}
	win_flags = f;
}




window the_cwin;

void  io_init_windows () 
{
	//print_width = 80;		/* default ascii print width */

	io_set_input_status (1, 2, 0);
	//nwin = 1;
	cwin = &the_cwin;
	cwin->id = win_id++;
	cwin->win_over = 0;		/* This will be fixed by a future set_numcols */
	cwin->win_down = (label_rows
			+ (user_status > 0) * status_rows
			+ (user_input > 0) * input_rows);
	win_flags = WIN_EDGES | WIN_EDGE_REV;
	cwin->numr = (Global->scr_lines - label_rows - !!user_status * status_rows
			- input_rows - default_bottom_border);
	cwin->numc = Global->scr_cols - default_right_border;
	cwin->bottom_edge_r = default_bottom_border;
	cwin->right_edge_c = default_right_border;
	cwin->lh_wid = 0;

}
