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
#include "utils.h"

/* Used by motion commands. */
const int colmagic[] = {0, 0, 1, -1, 1, -1, 1, -1, 0};
const int rowmagic[] = {-1, 1, 0, 0, -1, -1, 1, 1, 0};

/* Low level window operators. */

#define MIN_WIN_HEIGHT(W) (W->bottom_edge_r \
			   + label_rows * (W->flags & WIN_EDGES ? 2 : 1))

#define MIN_WIN_WIDTH(W) (W->right_edge_c \
			  + label_emcols * (W->flags & WIN_EDGES ? 6 : 1))
#define MIN_CWIN_HEIGHT  MIN_WIN_HEIGHT(cwin)
#define MIN_CWIN_WIDTH  MIN_WIN_WIDTH(cwin)


struct tmp_s { int l, r, u, b; };

static void 
do_close_window (int num)
{
	int n;
	struct window *win, *kwin;
	int nlf, nrt, nup, nbl;
	int klo, kho, kld, khd;
	int lo, ho, ld, hd;
	static tmp_s *tmpptr;

	if (nwin == 1)
	{
		raise_error("Attempt to delete sole ordinary window.");
		return;
	}
	tmpptr =  (struct tmp_s*) ck_malloc (sizeof (struct tmp_s) * nwin);

	kwin = &wins[num];
	nlf = nrt = nup = nbl = 0;
	klo = kwin->win_over - kwin->lh_wid;
	kho = kwin->win_over + kwin->numc + kwin->right_edge_c - 1;
	kld = kwin->win_down - (kwin->lh_wid ? label_rows : 0);
	khd = kwin->win_down + kwin->numr + kwin->bottom_edge_r - 1;

	for (win = wins; win < &wins[nwin]; win++)
	{
		lo = win->win_over - win->lh_wid;
		ho = win->win_over + win->numc + win->right_edge_c - 1;
		ld = win->win_down - (win->lh_wid ? label_rows : 0);
		hd = win->win_down + win->numr + win->bottom_edge_r - 1;

		/* Match to the left ? */
		if (lo == kho + 1)
		{
			if (ld >= kld && hd <= khd)
				tmpptr[nrt++].r = win - wins;
			else if (hd >= kld && ld <= khd)
				nrt = nwin;
		}
		else if (ho == klo - 1)
		{
			if (ld >= kld && hd <= khd)
				tmpptr[nlf++].l = win - wins;
			else if (hd >= kld && ld <= khd)
				nlf = nwin;
		}
		else if (ld == khd + 1)
		{
			if (lo >= klo && ho <= kho)
				tmpptr[nbl++].b = win - wins;
			else if (ho >= kho && lo <= kho)
				nbl = nwin;
		}
		else if (hd == kld - 1)
		{
			if (lo >= klo && ho <= kho)
				tmpptr[nup++].u = win - wins;
			else if (ho >= kho && lo <= kho)
				nup = nwin;
		}

	}
	if (nrt == 0)
		nrt = nwin;
	if (nlf == 0)
		nlf = nwin;
	if (nbl == 0)
		nbl = nwin;
	if (nup == 0)
		nup = nwin;
	if (nrt <= nlf && nrt <= nbl && nrt <= nup)
		for (n = 0; n < nrt; n++)
		{
			wins[tmpptr[n].r].numc
				+= kwin->lh_wid + kwin->numc + kwin->right_edge_c;
			wins[tmpptr[n].r].win_over
				-= kwin->lh_wid + kwin->numc + kwin->right_edge_c;
		}
	else if (nlf <= nbl && nlf <= nup)
		for (n = 0; n < nlf; n++)
			wins[tmpptr[n].l].numc
				+= kwin->lh_wid + kwin->numc + kwin->right_edge_c;
	else if (nbl <= nup)
		for (n = 0; n < nbl; n++)
		{
			wins[tmpptr[n].b].numr
				+= kwin->numr + (kwin->lh_wid ? 1 : 0) * label_rows
				+ kwin->bottom_edge_r;

			wins[tmpptr[n].b].win_down
				-= kwin->numr + (kwin->lh_wid ? 1 : 0) * label_rows
				+ kwin->bottom_edge_r;
		}
	else
		for (n = 0; n < nup; n++)
			wins[tmpptr[n].u].numr
				+= kwin->numr + (kwin->lh_wid ? 1 : 0) * label_rows;

	if (kwin == cwin && kwin != wins)
		--cwin;
	if (cwin == &wins[nwin - 1])
		--cwin;
	while (kwin < &wins[nwin])
	{
		*kwin = kwin[1];
		kwin++;
	}
	--nwin;
	io_recenter_all_win ();
	return;
}

int
win_label_cols (struct window * win, CELLREF hr)
{
	int lh;

	if ((win->flags & WIN_EDGES) == 0)
		lh = 0;
#if BITS_PER_CELLREF>8
	else if ((win->flags & WIN_PAG_HZ) || hr >= 10000)
		lh = 7;
	else if (hr >= 1000)
		lh = 6;
	else if (hr >= 100)
		lh = 5;
#else
	else if ((win->flags & WIN_PAG_HZ) || hr >= 100)
		lh = 5;
#endif
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
	return (win->flags & WIN_EDGES) ? label_rows : 0;
}

static void 
set_numcols (struct window *win, CELLREF hr)
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

static void 
recenter_window (struct window *win)
{
	if (win->flags & WIN_PAG_VT)
		page_axis (win->win_curow, get_scaled_height, win->numr,
				&(win->screen.lr), &(win->screen.hr));
	else
		recenter_axis (win->win_curow, get_scaled_height, win->numr,
				&(win->screen.lr), &(win->screen.hr));
	set_numcols (win, win->screen.hr);
	if (win->flags & WIN_PAG_HZ)
		page_axis (win->win_cucol, get_scaled_width, win->numc,
				&(win->screen.lc), &(win->screen.hc));
	else
		recenter_axis (win->win_cucol, get_scaled_width, win->numc,
				&(win->screen.lc), &(win->screen.hc));
}

void 
io_recenter_cur_win (void)
{
	//if(!cwin || !the_cmd_frame || ! curow) return; // maybe running headless
	//if(!the_cmd_frame) return; // maybe running headless

	cwin->win_curow = curow;
	cwin->win_cucol = cucol;
	io_recenter_named_window (cwin);
	io_repaint_win (cwin);
	if (cwin->link > 0)
		io_repaint_win (&wins[cwin->link]);
}

void
io_recenter_all_win(void)
{
	//if(!the_cmd_frame) return; // maybe running headless
	int n;
	if (!nwin)
		return;
	cwin->win_curow = curow;
	cwin->win_cucol = cucol;
	for (n = 0; n < nwin; n++)
		io_recenter_named_window (&wins[n]);
	io_repaint ();
}
void
io_recenter_named_window(struct window *w)
{
	recenter_window(w);
}

/*
 * RESIZE_SCREEN adjusts the windows list after a screen size change.
 * It presumes that Global->scr_lines and Global->scr_cols are the new values.  DR and DC
 * are the changes that just occured to those values.
 */
static void 
resize_screen (int dr, int dc)
{
	int x, n;
	int lines;
	int firstln;
	int ncols;
	int firstcol;
	int old_lines;

	if (!nwin)
		return;

	lines = Global->scr_lines - (!!user_status * status_rows) - input_rows;
	old_lines = lines - dr;
	firstln = (user_input > 0) * input_rows + (user_status > 0) * status_rows;

	/* First, delete windows that will shrink too much. */
	cwin->win_curow = curow;
	cwin->win_cucol = cucol;
	if (dr < 0)
		for (x = 0; x < nwin; x++)
		{
			int rlow =
				(wins[x].win_down - (wins[x].lh_wid ? label_rows : 0) - firstln);
			int rhi = ((wins[x].win_down + wins[x].numr + wins[x].bottom_edge_r)
					- firstln);
			int sqbelow = dr * rlow;
			int sqtohere = dr * rhi;
			sqbelow /= old_lines;
			sqtohere /= old_lines;
			if (wins[x].numr <= sqbelow - sqtohere)
			{
				do_close_window (x);
				x--;
			}
		}
	for (x = 0; x < nwin; ++x)
	{
		int rlow =
			(wins[x].win_down - (wins[x].lh_wid ? label_rows : 0) - firstln);
		int rhi = ((wins[x].win_down + wins[x].numr + wins[x].bottom_edge_r)
				- firstln);
		int sqbelow = dr * rlow;
		int sqtohere = dr * rhi;
		sqbelow /= old_lines;
		sqtohere /= old_lines;
		wins[x].win_down += sqbelow;
		wins[x].numr += sqtohere - sqbelow;
	}

	/* then columns */
	firstcol = 0;
	ncols = Global->scr_cols;
	ncols -= dc;

	/* First, delete windows that will shrink too much. */
	if (dc < 0)
		for (x = 0; x < nwin; x++)
		{
			int clow = (wins[x].win_over - wins[x].lh_wid) - firstcol;
			int chi = (wins[x].win_over + wins[x].numc + wins[x].right_edge_c
					- firstcol);
			int sqbelow = dc * clow;
			int sqtohere = dc * chi;
			sqbelow /= ncols;
			sqtohere /= ncols;
			if (wins[x].numc <= sqbelow - sqtohere)
			{
				do_close_window (x);
				x--;
			}
		}
	for (x = 0; x < nwin; ++x)
	{
		int clow = (wins[x].win_over - wins[x].lh_wid) - firstcol;
		int chi = (wins[x].win_over + wins[x].numc + wins[x].right_edge_c
				- firstcol);
		int sqbelow = dc * clow;
		int sqtohere = dc * chi;
		sqbelow /= ncols;
		sqtohere /= ncols;
		wins[x].win_over += sqbelow;
		wins[x].numc += sqtohere - sqbelow;
	}
	for (n = 0; n < nwin; n++)
		recenter_window (&wins[n]);
	io_repaint ();
}

static void
shift_linked_window (long dn, long ov)
{
	struct window *win;

	win = cwin;
	while (win->link != -1)
	{
		win = &wins[win->link];
		if (win == cwin)		/* Loop check! */
			return;
		if ((win->flags & WIN_LCK_VT) == 0)
			win->win_curow += dn;
		if ((win->flags & WIN_LCK_HZ) == 0)
			win->win_cucol += ov;
		if (win->win_curow < win->screen.lr || win->win_curow > win->screen.hr
				|| win->win_cucol < win->screen.lc || win->win_cucol > win->screen.hc)
			recenter_window (win);
	}
}


static void 
find_nonzero (CELLREF *curp, CELLREF lo, CELLREF hi, int (*get) (CELLREF))
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

static int 
scroll_axis (CELLREF cur, int over, int total, int (*get) (CELLREF), CELLREF *ret1, CELLREF *ret2, int *offp) 
{
	int tot; 

	int inc;
	CELLREF fini;
	int num;
	CELLREF p1, p2;
	int n;

	inc = (over > 0 ? 1 : -1);
	fini = over > 0 ? MAX_ROW : MIN_ROW;
	num = over > 0 ? over : -over;

	if (inc > 0 ? *ret2 == MAX_ROW : *ret1 == MIN_ROW)
		return 1;
	p1 = inc > 0 ? *ret2 + 1 : *ret1 - 1;
	p2 = p1;
	for (;;)
	{
		--num;
		tot = (*get) (p1);
		while (p2 != fini && tot + (n = (*get) (p2 + inc)) <= total)
		{
			p2 += inc;
			tot += n;
		}
		if (!num || p2 == fini)
			break;
	}
	if (num)
		return 1;
	while (tot + (n = (*get) (p1 - inc)) <= total)
	{
		p1 -= inc;
		tot += n;
		if (inc > 0)
			(*offp)++;
	}
	if (p1 > p2)
	{
		*ret1 = p2;
		*ret2 = p1;
	}
	else
	{
		*ret1 = p1;
		*ret2 = p2;
	}
	return 0;
}

static int 
page_scroll_axis (CELLREF cur, int over, int total, int (*get) (CELLREF), CELLREF *ret1, CELLREF *ret2, int *offp)
{
	int n_over;
	CELLREF lo, hi;
	int tot;
	int ww = 0;

	n_over = 0;
	lo = hi = MIN_ROW;
	tot = (*get) (hi);
	for (;;)
	{
		while (hi < MAX_ROW && tot + (ww = (*get) (hi + 1)) <= total)
		{
			hi++;
			tot += ww;
		}
		if (hi >= cur)
			break;
		hi++;
		n_over++;
		lo = hi;
		tot = ww;
	}
	n_over += over;
	if (n_over < 0)
		return 1;

	lo = hi = MIN_ROW;
	tot = (*get) (hi);
	for (;;)
	{
		while (hi < MAX_ROW && tot + (ww = (*get) (hi + 1)) <= total)
		{
			hi++;
			tot += ww;
		}
		if (!n_over || hi == MAX_ROW)
			break;
		--n_over;
		hi++;
		lo = hi;
		tot = ww;
	}
	if (hi == MAX_ROW && n_over)
		return 1;
	*ret1 = lo;
	*ret2 = hi;
	return 0;
}



/* External window interface */

void 
io_set_label_size (int r, int c)
{
	/* fixme */
}

void 
io_set_scr_size (int lines, int cols)
{
	int dl = lines - Global->scr_lines;
	int dc = cols - Global->scr_cols;

	Global->scr_lines = lines;
	Global->scr_cols = cols;

	resize_screen (dl, dc);
}

void 
io_set_input_rows (int n)
{
	input_rows = n;
	io_set_input_status (user_input, user_status, 1);
}

void 
io_set_status_rows (int n)
{
	status_rows = n;
	io_set_input_status (user_input, user_status, 1);
}

void 
io_set_input_status (int inp, int stat, int redraw)
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

			if (grow < 0)
			{
				int x;
re:
				for (x = 0; x < nwin; ++x)
				{
					int top = wins[x].win_down - win_label_rows(&wins[x]);
					if (cell_top == top && (wins[x].numr <= -grow))
					{
						do_close_window (x);
						goto re;
					}
				}
			}

			if (grow)
			{
				int x;
				for (x = 0; x < nwin; ++x)
				{
					int top =
						wins[x].win_down - win_label_rows (&wins[x]);
					if (cell_top == top)
						wins[x].numr -= vchange;
				}
			}
			if (vchange)
			{
				int x;
				for (x = 0; x < nwin; ++x)
					wins[x].win_down += vchange;
			}
			io_repaint ();
		}
		user_input = new_ui;
		user_status = new_us;
		Global->input = new_inp;
		Global->status = new_stat;
	}
}

void 
io_set_cwin (struct window *win)
{
	io_hide_cell_cursor ();
	cwin->win_curow = curow;
	cwin->win_cucol = cucol;
	cwin = win;
	curow = cwin->win_curow;
	cucol = cwin->win_cucol;
	io_display_cell_cursor ();
}


void 
io_pr_cell (CELLREF r, CELLREF c, CELL *cp)
{
	//if(!the_cmd_frame) return; // maybe running headless
	//if(running_headless()) return;
	if(!wins) return; // maybe we're running headless
	struct window *win;

	for (win = wins; win < &wins[nwin]; win++)
	{
		if (r < win->screen.lr || r > win->screen.hr
				|| c < win->screen.lc || c > win->screen.hc)
			continue;
		io_pr_cell_win (win, r, c, cp);
	}
}

void
io_redo_region (struct rng * rng)
{
	for(CELL* cp:get_cells_in_range(rng))
	{
		auto [r, c] = decoord(cp);
		io_pr_cell (r, c, cp);
	}
}

/* Create a new window by splitting the current one. */
void 
io_win_open (int hv, int where)
{
	int tmp;
	struct window *win;

	if (   (!hv
				&& (where < MIN_CWIN_WIDTH
					|| (cwin->numc + cwin->lh_wid + cwin->right_edge_c - where
						< MIN_CWIN_WIDTH)))
			|| (hv
				&& (where < MIN_CWIN_HEIGHT
					|| (cwin->numr + cwin->bottom_edge_r
						+ (cwin->lh_wid ? label_rows : 0) - where
						< MIN_CWIN_HEIGHT))))
	{
		raise_error("Window won't fit!");
		return;
	}

	nwin++;
	tmp = cwin - wins;
	wins = (window *) ck_realloc (wins, nwin * sizeof (struct window));
	win = &wins[nwin - 1];
	cwin = &wins[tmp];
	win->id = win_id++;
	win->bottom_edge_r = cwin->bottom_edge_r;
	win->right_edge_c = cwin->right_edge_c;
	/* set_numcols will take care of fixing win_over if edges are on. */
	win->win_over = cwin->win_over + (hv ? 0 : where) - cwin->lh_wid;
	win->win_down = cwin->win_down + (hv ? where : 0);
	win->flags = cwin->flags;
	win->link = -1;
	win->lh_wid = 0;
	win->win_slops = 0;
	win->numc = cwin->numc + cwin->lh_wid + (hv ? 0 : -where);
	win->numr = cwin->numr + (hv ? -where : 0);
	win->win_curow = curow;
	win->win_cucol = cucol;
	set_numcols (win, curow);
	cwin->numc -= (hv ? 0 : win->numc + win->lh_wid + win->right_edge_c);
	cwin->numr -=
		(hv ? win->numr + (win->lh_wid ? label_rows : 0) + win->bottom_edge_r
		 : 0);
	cwin->win_curow = curow;
	cwin->win_cucol = cucol;
	io_hide_cell_cursor ();
	win = cwin;
	cwin = &wins[nwin - 1];
	recenter_window (cwin);
	recenter_window (win);
	io_display_cell_cursor ();
	io_repaint ();
}

void 
io_win_close (struct window *win)
{
	do_close_window (win - wins);
}

void 
io_move_cell_cursor (CELLREF rr, CELLREF cc)
{
	if (cwin->link != -1)
		shift_linked_window ((long) rr - curow, (long) cc - cucol);
	if (rr < cwin->screen.lr || rr > cwin->screen.hr
			|| cc < cwin->screen.lc || cc > cwin->screen.hc)
	{
		cwin->win_curow = curow = rr;
		cwin->win_cucol = cucol = cc;
		recenter_window (cwin);
		io_repaint_win (cwin);
		if (cwin->link > 0)
			io_repaint_win (&wins[cwin->link]);
	}
	else
	{
		io_hide_cell_cursor ();
		curow = rr;
		cucol = cc;
		io_display_cell_cursor ();
		io_update_status ();
	}
	if (get_scaled_width (cucol) == 0)
		find_nonzero (&cucol, cwin->screen.lc, cwin->screen.hc, get_scaled_width);
	if (get_scaled_height (curow) == 0)
		find_nonzero (&curow, cwin->screen.lr, cwin->screen.hr, get_scaled_height);
}

void 
io_shift_cell_cursor (int dirn, int repeat)
{
	CELLREF c;
	CELLREF r;
	int w = 0;
	int over, down;

	over = colmagic[dirn] * repeat;
	down = rowmagic[dirn] * repeat;
	if (over > 0)
	{
		c = cucol;
		while (c < MAX_COL && over-- > 0)
		{
			c++;
			while ((w = get_scaled_width (c)) == 0 && c < MAX_COL)
				c++;
		}
		if (over > 0 || c == cucol || w == 0)
		{
			raise_error("Can't go right");
			return;
		}
	}
	else if (over < 0)
	{
		c = cucol;
		while (c > MIN_COL && over++ < 0)
		{
			--c;
			while ((w = get_scaled_width (c)) == 0 && c > MIN_COL)
				--c;
		}
		if (over < 0 || c == cucol || w == 0)
		{
			raise_error("Can't go %s", "left");
			return;
		}
	}
	else
		c = cucol;

	if (down > 0)
	{
		r = curow;
		while (r < MAX_ROW && down-- > 0)
		{
			r++;
			while ((w = get_scaled_height (r)) == 0 && r < MAX_ROW)
				r++;
		}
		if (down > 0 || r == curow || w == 0)
		{
			raise_error("Can't go down");
			return;
		}
	}
	else if (down < 0)
	{
		r = curow;
		while (r > MIN_ROW && down++ < 0)
		{
			--r;
			while ((w = get_scaled_height (r)) == 0 && r > MIN_ROW)
				--r;
		}
		if (down < 0 || r == curow || w == 0)
		{
			raise_error("Can't go up");
			return;
		}
	}
	else
		r = curow;

	io_move_cell_cursor (r, c);
}

void 
io_scroll_cell_cursor (int magic, int repeat)
{
	int off_dn, off_rt;

	struct rng s;
	CELLREF cr, cc;
	int over, down;
	int ret;

	over = colmagic[magic];
	down = rowmagic[magic];

	s.lr = cwin->screen.lr;
	s.hr = cwin->screen.hr;
	if (down)
	{
		off_dn = curow - cwin->screen.lr;
		if (cwin->flags & WIN_PAG_VT)
			ret = page_scroll_axis
				(curow, down, cwin->numr, get_scaled_height, &(s.lr), &(s.hr), &off_dn);
		else
			ret = scroll_axis
				(curow, down, cwin->numr, get_scaled_height, &(s.lr), &(s.hr), &off_dn);
		cr = (off_dn > s.hr - s.lr) ? s.hr : s.lr + off_dn;
		if (ret)
			raise_error("Can't scroll that far");
		set_numcols (cwin, s.hr);
	}
	else
		cr = curow;

	off_rt = cucol - cwin->screen.lc;

	s.lc = cwin->screen.lc;
	s.hc = cwin->screen.hc;
	if (over)
	{
		if (cwin->flags & WIN_PAG_HZ)
			ret = page_scroll_axis
				(cucol, over, cwin->numc, get_scaled_width, &(s.lc), &(s.hc), &off_rt);
		else
			ret = scroll_axis (cucol, over, cwin->numc, get_scaled_width, &(s.lc), &(s.hc), &off_rt);
		if (ret)
			raise_error("Can't scroll that far");
		cc = (s.hc - s.lc < off_rt) ? s.hc : s.lc + off_rt;
	}
	else if ((cwin->flags & WIN_PAG_HZ) == 0)
		/* ... */
		cc = cucol;
	else
		cc = cucol;

	/*fixme The original has a big #if 0 here. */
	if (cwin->link != -1)
		shift_linked_window ((long) cr - curow, (long) cc - cucol);

	cwin->screen = s;
	curow = cr;
	cucol = cc;

	if (get_scaled_width (cucol) == 0)
		find_nonzero (&cucol, cwin->screen.lc, cwin->screen.hc, get_scaled_width);
	if (get_scaled_height (curow) == 0)
		find_nonzero (&curow, cwin->screen.lr, cwin->screen.hr, get_scaled_height);

	io_repaint_win (cwin);
	if (cwin->link > 0)
		io_repaint_win (&wins[cwin->link]);
}



void 
io_set_win_flags (struct window *w, int f)
{
	if ((f & WIN_EDGES) && !(w->flags & WIN_EDGES))
	{
		if (w->numr < 2 || w->numc < 6)
			raise_error("Edges wouldn't fit!");
		w->win_down++;
		w->numr--;
		set_numcols (w, w->screen.hr);
	}
	else if (!(f & WIN_EDGES) && (w->flags & WIN_EDGES))
	{
		w->win_over -= w->lh_wid;
		w->numc += w->lh_wid;
		w->lh_wid = 0;
		w->win_down--;
		w->numr++;
	}
	w->flags = f;
}

std::string
io_write_window_config ()
{
	std::ostringstream oss;

	int n;
	char buf[90];
	//struct line scratch;
	//scratch.alloc = 0;
	//scratch.buf = 0;

	cwin->win_curow = curow;
	cwin->win_cucol = cucol;
	//sprint_line (&out, "O;status %d\n", user_status);
	oss << "O;status " << user_status << "\n";
	if (nwin > 1)
	{
		/* ... */ /* fixme ? */
	}
	for (n = 0; n < nwin; n++)
	{
		buf[0] = '\0';
		if (wins[n].flags & WIN_LCK_HZ)
			strcat (buf, ",lockh");
		if (wins[n].flags & WIN_LCK_VT)
			strcat (buf, ",lockv");
		if (wins[n].flags & WIN_PAG_HZ)
			strcat (buf, ",pageh");
		if (wins[n].flags & WIN_PAG_VT)
			strcat (buf, ",pagev");
		if (wins[n].flags & WIN_EDGE_REV)
			strcat (buf, ",standout");
		if ((wins[n].flags & WIN_EDGES) == 0)
			strcat (buf, ",noedges");
		//scratch = *out;
		//out->alloc = 0;
		//out->buf = 0;
		//sprint_line (out, "%sW;N%d;A%u %u;C%d %d %d;O%s\n", scratch.buf, n + 1, wins[n].win_curow, wins[n].win_cucol, 7, 0, 7, buf + 1);
		oss << "W;N" << n+1 << ";A" << wins[n].win_curow << " " << wins[n].win_cucol 
			<< ";C7 0 7;O" << buf+1 << "\n";
		//free (scratch.buf);
	}

	return oss.str();
}

void 
io_read_window_config (char * line)
{
	int wnum = 0;
	char *text;
	CELLREF nrow = NON_ROW, ncol = NON_COL;
	char *split = 0;
	char *opts = 0;
	struct window *win;

	text = line;
	for (;;)
	{
		switch (*text++)
		{
			/* Window Number */
			case 'N':
				wnum = astol (&text);
				break;
				/* Cursor At */
			case 'A':
				nrow = astol (&text);
				ncol = astol (&text);
				break;
				/* JF: Window options */
			case 'O':
				opts = text;
				while (*text && *text != ';')
					text++;
				break;
				/* Split into two windows */
			case 'S':
				split = text;
				while (*text && *text != ';')
					text++;
				break;
				/* Set Colors NOT supported */
			case 'C':
				while (*text && *text != ';')
					text++;
				break;
				/* Alternate border NOT supported. . . */
			case 'B':
				break;
			default:
				--text;
				break;
		}
		if (*text == '\0' || *text == '\n')
			break;
		if (*text != ';')
		{
			char *bad;

			bad = text;
			while (*text && *text != ';')
				text++;
			if (*text)
				*text++ = '\0';
			raise_error("Unknown SYLK window cmd: %s", bad);
			if (!*text)
				break;
		}
		else
			*text++ = '\0';
	}
	if (wnum < 1 || wnum > nwin)
	{
		raise_error("Window %d out of range in SYLK line %s", wnum, line);
		return;
	}
	--wnum;
	win = &wins[wnum];
	if (nrow != NON_ROW)
	{
		win->win_curow = nrow;
		win->win_cucol = ncol;
		if (win == cwin)
		{
			//curow = nrow;
			set_curow(nrow);
			//cucol = ncol;
			set_cucol(ncol);
		}
		recenter_window (win);
	}
	if (split)
	{
		int hv = 0;
		int where;
		int link;
		struct window *neww;

		switch (*split++)
		{
			case 'H':
			case 'h':
				hv = 1;
				break;
			case 'v':
			case 'V':
				hv = 0;
				break;
			case 't':
			case 'T':
				raise_error("Window split titles not supported");
				return;
			default:
				break;
		}
		if (*split == 'L')
		{
			link = wnum;
			split++;
		}
		else
			link = -1;

		where = astol (&split);

		if (hv ? where >= win->numr : where >= win->numc)
			raise_error("Can't split window: screen too small");

		nwin++;
		wins = (window *) ck_realloc (wins, nwin * sizeof (struct window));
		cwin = wins;
		win = &wins[wnum];
		neww = &wins[nwin - 1];

		win->numc -= (hv ? 0 : where);
		win->numr -= (hv ? where : 0);
		win->win_curow = curow;
		win->win_cucol = cucol;

		neww->flags = WIN_EDGES | WIN_EDGE_REV;	/* Mplan defaults */
		neww->lh_wid = 0;		/* For now */
		neww->link = link;

		neww->win_over = win->win_over + (hv ? -win->lh_wid : win->numc);
		neww->win_down = win->win_down + (hv ? win->numr + 1 : 0);
		neww->numc = (hv ? win->numc + win->lh_wid : where);
		neww->numr = (hv ? where - 1 : win->numr);
		neww->win_curow = curow;
		neww->win_cucol = cucol;
		set_numcols (neww, curow);
		recenter_window (win);
		recenter_window (neww);
	}
	if (opts)
	{
		char *np;
		while ((np =(char *) index (opts, ',')))
		{
			*np = '\0';
			set_options (opts);
			*np++ = ';';
			opts = np;
		}
		if ((np = (char *)rindex (opts, '\n')))
			*np = '\0';
		set_options (opts);
	}
}



void 
io_init_windows (int sl, int sc, int ui, int us, int ir, int sr, int lr, int lc) 
{
	//print_width = 80;		/* default ascii print width */
	Global->scr_lines = sl;
	Global->scr_cols = sc;
	input_rows = ir;
	status_rows = sr;
	label_rows = lr;
	label_emcols = lc;
	io_set_input_status (ui, us, 0);
	nwin = 1;
	wins = cwin = (window *) ck_malloc (sizeof (struct window));
	wins->id = win_id++;
	wins->win_over = 0;		/* This will be fixed by a future set_numcols */
	wins->win_down = (label_rows
			+ (user_status > 0) * status_rows
			+ (user_input > 0) * input_rows);
	wins->flags = WIN_EDGES | WIN_EDGE_REV;
	wins->numr = (Global->scr_lines - label_rows - !!user_status * status_rows
			- input_rows - default_bottom_border);
	wins->numc = Global->scr_cols - default_right_border;
	wins->bottom_edge_r = default_bottom_border;
	wins->right_edge_c = default_right_border;
	wins->link = -1;
	wins->lh_wid = 0;
	wins->win_curow = MIN_ROW;
	wins->win_cucol = MIN_COL;
	wins->win_slops = 0;
}
