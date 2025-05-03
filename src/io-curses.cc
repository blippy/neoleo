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



#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <term.h>
#include <menu.h>
#include <panel.h>


#include "cell.h"
#include "global.h"
#include "io-2019.h"
#include "io-curses.h"
#include "io-utils.h"
#include "menu-2025.h"
#include "regions.h"
#include "sheet.h"
#include "spans.h"
#include "utils.h"


import std;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using namespace std::string_literals;

import errors;
import logging;
import win;



static int scr_lines = 24, scr_cols = 80;



// TODO get rid of this
struct OleoGlobal {
	int 		input_rows = 1, status_rows = 1;
	const int 	user_input =1, user_status = 2;
	const int	input = 0, status = 1;
	const int	label_rows = 1;
	//const int	default_right_border = 0, default_bottom_border = 0;
	const int	win_id = 1;
};

inline OleoGlobal global;
inline struct OleoGlobal *Global = &global;

/* These control the layout of input and status lines. */
#define	user_input	Global->user_input
#define	user_status	Global->user_status
#define	input_rows	Global->input_rows
#define	status_rows	Global->status_rows

/* These control the layout of edge labels. */
#define	label_rows	Global->label_rows
#define	label_emcols	Global->label_emcols










/* The tty windows datastructures: */


struct window
{
  /* Do not change these directly. */
  const int id = 1; // a window id
  int win_over = 0;			// x-posiition Where the data in this window starts. Can change due to row number
  const int win_down = 3;	// y-position where data grid starts
  struct rng screen{0};		/* Cells visible. recenter_* updates this. */

  /* Number of lines of spreadsheet that can fit in this window.
     This only changes when the screen is resized,
     win->flags&WIN_EDGES changes, or a window is either
     created or destroyed */
  int numr;

  /* Number of text columns that can fit in this window.
     This changes when the screen is resized,
     win->flags&WIN_EDGES changes, a window is created or
     destoryed, or win->lh_wid changes.  In the last case
     win->numc+win->lh_wid remains a constant. */
  int numc;

  /*
   * Number of columns and rows for right and bottom edges.
   * As this changes, numc and numr change accordingly.
   */
  int bottom_edge_r;
  int right_edge_c;


  /* Number of columns taken up by the row numbers at the
     left hand edge of the screen.  Zero if edges is
     win->flags&WIN_EDGES is off (by definition).  Seven (or
     five) if win->flags&WIN_PAG_HZ (to make things easier).
     Ranges between three "R9 " to seven "R32767 " depending on
     the number of the highest row on the screen.  */
  int lh_wid;

};


inline window the_cwin;
inline window* cwin = &the_cwin;
#define	win_id		Global->win_id


static void move_cursor_to (struct window *, CELLREF, CELLREF);
void 		cur_io_pr_cell_win (struct window *win, CELLREF r, CELLREF c, CELL *cp);
void 		io_pr_cell (CELLREF r, CELLREF c, CELL *cp);
void  		io_init_windows ();
bool 		curses_loop ();





void cur_io_display_cell_cursor (void)
{
	if(!inside(curow, cucol, cwin->screen)) return;

	int cell_cursor_col = cwin->win_over;
	for (int cc = cwin->screen.lc; cc < cucol; cc++)
		cell_cursor_col += get_width (cc);
	int cell_cursor_row = cwin->win_down;
	for (int rr = cwin->screen.lr; rr < curow; rr++)
		cell_cursor_row += get_height (rr);
	int cwid = std::min(cwin->numc, get_width (cucol));
	move (cell_cursor_row, cell_cursor_col);
	standout ();
	for (int n = cwid; n; n--)
		addch (inch () | A_STANDOUT);
	standend ();
}

void win_io_hide_cell_cursor (void)
{
	if(!inside(curow, cucol, cwin->screen)) return;

	int cell_cursor_col = cwin->win_over;
	for (int cc = cwin->screen.lc; cc < cucol; cc++)
		cell_cursor_col += get_width (cc);
	int cell_cursor_row = cwin->win_down;
	for (int rr = cwin->screen.lr; rr < curow; rr++)
		cell_cursor_row += get_height (rr);
	int cwid = std::min(cwin->numc, get_width (cucol));
	move (cell_cursor_row, cell_cursor_col);
	for (int n = cwid; n; n--)
		addch (inch () & ~A_STANDOUT);
}

/* Functions, etc for dealing with cell contents being displayed
	on top of other cells. */

typedef struct slop { CELLREF row, clo, chi; } slop_t;
typedef vector<slop_t> slops_t;
slops_t the_slops;

static void flush_slops ()
{
	the_slops.clear();
}


static int find_slop (CELLREF r, CELLREF c, CELLREF *cclp, CELLREF *cchp)
{

	for(auto &s : the_slops) {
		if(s.row == r && s.clo <= c && s.chi >= c) {
			*cclp = s.clo;
			*cchp = s.chi;
			return 1;
		}
	}
	return 0;
}

static void kill_slop (CELLREF r, CELLREF clo, CELLREF chi)
{
	for(auto it = the_slops.begin(); it != the_slops.end() ; ++it)  {
		if(it->row == r && it->clo == clo && it->chi == chi) {
			the_slops.erase(it);
			return;
		}
	}
}

static void set_slop (CELLREF r, CELLREF clo, CELLREF chi)
{
	slop_t s{r, clo, chi};
	the_slops.push_back(s);
}

static void change_slop (CELLREF r, CELLREF olo, CELLREF ohi, CELLREF lo, CELLREF hi)
{
	for(auto &s : the_slops) {
		if(s.row == r && s.clo == olo && s.chi == ohi) {
			s.clo = lo;
			s.chi = hi;
			return;
		}

	}
}


int win_label_cols (struct window * win, CELLREF hr)
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
	return lh;
}

int win_label_rows (struct window * win)
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

static void recenter_axis (CELLREF cur, int (*get) (CELLREF), int total, CELLREF *loP, CELLREF *hiP)
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

static void page_axis (CELLREF cur, int (*get) (CELLREF), int total, CELLREF *loP, CELLREF *hiP)
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


void  recenter_window (struct window *win = cwin) // FN
{
	//if(!win) win = cwin;
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



std::string status_line(int wid)
{
	//log("status_line called");
	const char *ptr = std::format("r{}c{}", curow, cucol).c_str();
	addstr (ptr);
	wid -= strlen (ptr);


	std::string dec = formula_text(curow, cucol);
	const std::string& cvs = cell_value_string(curow, cucol, 1);
	ptr = cvs.c_str();

	int plen = strlen (ptr);

	int dlen = dec.size();
	if(dlen>0)
	{
		wid -= 4;
		if (dlen + plen > wid)
		{
			if (plen + 3 > wid)
				return string_format(" %.*s... [...]", wid - 6, ptr);
			else
				return string_format(" %s [%.*s...]", ptr, wid - plen - 3, dec.c_str());
		}
		else
			return string_format(" %s [%s]", ptr, dec.c_str());
	}
	else if (plen)
	{
		--wid;
		if (plen > wid)
			return string_format(" %.*s...", wid - 3, ptr);
		else
			return string_format(" %s", ptr);
	}

	return "";

}
void cur_io_update_status (void) // FN
{

	move (Global->status, 0);
	//int wid = cwin->screen.hc; // columns - 2;
	int wid = COLS;
	win_print(status_line(wid));
	clrtoeol();
}


void cur_io_repaint ()
{
	//io_recenter_cur_win();
	CELLREF cc, rr;
	int n, n1;
	struct window *win = cwin;

	erase();
	//clear();
	show_menu();
	
	if (win->lh_wid)
	{
		move (win->win_down - 1, win->win_over - win->lh_wid);
		//static_assert(std::is_same<decltype(win), void*>::value, "printw() might be wrong");
		static_assert(sizeof(win) == sizeof(void*), "printw() might be wrong");
		static_assert(sizeof(win) == sizeof(long int), "printw() might be wrong");
		printw ("#%*ld ", win->lh_wid - 2, (long int)1);
		if (win_flags & WIN_EDGE_REV) standout(); // s_display.cdstandout();
		cc = win->screen.lc;
		do
		{
			n = get_width (cc);
			if (n > win->numc)
				n = win->numc;
			if (n > 1)
			{
				char *ptr;
				char buf[30];


				sprintf (buf, "C%u", cc);
				ptr = buf;

				--n;
				n1 = strlen (ptr);
				if (n < n1)
					printw ("%.*s ", n, "###############");
				else
				{
					n1 = (n - n1) / 2;
					printw ("%*s%-*s ", n1, "", n - n1, ptr);
				}
			}
			else if (n == 1)
				addstr ("#");
		}
		while (cc++ < win->screen.hc);

		// print row labels
		rr = win->screen.lr;
		n = win->win_down;
		do {
			n1 = get_height (rr);
			if (!n1) continue;
			move (n, win->win_over - win->lh_wid);
			printw ("R%-*d", win->lh_wid - 1, rr);
			n += n1;
		} while (rr++ < win->screen.hr);


		if (win_flags & WIN_EDGE_REV) standend ();
	}
	flush_slops();
	for(CELL* cp: get_cells_in_range(win->screen)) {
		decoord(cp, rr, cc);
		if (!is_nul(cp)) cur_io_pr_cell_win(win, rr, cc, cp);
	}
	
	cur_io_display_cell_cursor ();
	cur_io_update_status ();
}





static void move_cursor_to (struct window *win, CELLREF r, CELLREF c)
{
	int cell_cursor_col = win->win_over;
	for (int cc = win->screen.lc; cc < c; cc++)
		cell_cursor_col += get_width (cc);

	int cell_cursor_row = win->win_down;
	for (int rr = win->screen.lr; rr < r; rr++)
		cell_cursor_row += get_height (rr);

	move (cell_cursor_row, cell_cursor_col);
}



void cur_io_pr_cell_win (struct window *win, CELLREF r, CELLREF c, CELL *cp) // FN
{
	//log("_io_pr_cell_win:", cp);
	int wwid;


	int wid = get_width (c);
	if (wid == 0) return;
	wid = std::min(wid, win->numc);

	int hgt = get_height (r);
	if (hgt == 0) return;
	hgt = std::min(hgt, win->numr);


	move_cursor_to (win, r, c);

	//int yy, xx;
	//getyx(stdscr, yy, xx);
	// defer d{move, yy, xx};

	assert(win == cwin);
	int glowing = (r == curow && c == cucol && win == cwin);
	if (glowing) standout ();

	bool is_bold = cp->cell_flags.bold;
	if(is_bold) wattr_on(stdscr, WA_BOLD, 0);

	bool is_italic = cp->cell_flags.italic;
	if(is_italic) wattr_on(stdscr, WA_ITALIC, 0);



	int j = GET_JST (cp);
	if (j == JST_DEF) j = default_jst;

	const std::string& str = print_cell(cp);
	char ptr1[str.size()+1];
	char* ptr = strcpy(ptr1, str.c_str());
	int lenstr = strlen (ptr);

	if (lenstr <= wid - 1)
	{
		CELLREF ccl, cch;

		if (j == JST_LFT)
			printw ("%-*.*s", wid, wid - 1, ptr);
		else if (j == JST_RGT)
			printw ("%*.*s ", wid - 1, wid - 1, ptr);
		else if (j == JST_CNT)
		{
			wwid = (wid - 1) - lenstr;
			printw ("%*s%*s ", (wwid + 1) / 2 + lenstr, ptr, wwid / 2, "");
		}

		if (glowing)
			standend ();

		if (lenstr == 0 && c > win->screen.lc
				&& find_slop(r, c - 1, &ccl, &cch))
		{
			CELLREF ccdl, ccdh;

			if (find_slop(r, c, &ccdl, &ccdh) && ccdl == c)
			{
				kill_slop(r, ccdl, ccdh);
				for (; ccdh != ccdl; --ccdh)
					if (ccdh != c && (wwid = get_width (ccdh)))
					{
						move_cursor_to (win, r, ccdh);
						printw ("%*s", wwid, "");
					}
			}
			kill_slop(r, ccl, cch);
			io_pr_cell (r, ccl, find_cell (r, ccl));
		}
		else if (find_slop(r, c, &ccl, &cch))
		{
			kill_slop(r, ccl, cch);
			for (; cch != ccl; --cch)
				if (cch != c && (wwid = get_width (cch)))
				{
					move_cursor_to (win, r, cch);
					printw ("%*s", wwid, "");
				}
			io_pr_cell (r, ccl, find_cell (r, ccl));
		}
	}
	else
	{

		CELLREF cc = c;
		CELL *ccp = nullptr;

		for (wwid = wid; lenstr > wwid - 1; wwid += get_width (cc))
		{
			if (++cc > win->screen.hc
					|| ((ccp = find_cell (r, cc))
						&& (!is_nul(ccp))
						&& (GET_FORMAT (ccp) != FMT_HID
							|| (GET_FORMAT (ccp) == FMT_DEF
								&& default_fmt != FMT_HID))))
			{
				--cc;
				break;
			}
		}


		if (lenstr > wwid - 1) { 
			if (is_flt(cp)) ptr = adjust_prc (ptr, cp, wwid - 1, wid - 1, j);
		}

		if (wwid == 1)
		{
			addch (' ');
			if (glowing)
				standend ();
		}
		else if (wwid == wid)
		{
			printw ("%-*.*s ", wwid - 1, wwid - 1, ptr);
			if (glowing)
				standend ();
		}
		else if (glowing)
		{
			printw ("%.*s", wid, ptr);
			standend ();
			printw ("%-*.*s ", wwid - wid - 1, wwid - wid - 1, ptr + wid);
		}
		else if (r == curow && (cucol > c && cucol <= cc))
		{
			CELLREF ctmp;
			int w_left;
			int w_here;

			w_left = wid;
			for (ctmp = c + 1; ctmp < cucol; ctmp++)
				w_left += get_width (ctmp);
			printw ("%.*s", w_left, ptr);
			standout ();
			w_here = get_width (cucol);
			if (wwid > w_left + w_here)
			{
				printw ("%-*.*s", w_here, w_here, ptr + w_left);
				standend ();
				printw ("%-*.*s ",
						wwid - (w_left + w_here) - 1, wwid - (w_left + w_here) - 1,
						ptr + w_left + w_here);
			}
			else
			{
				printw ("%-*.*s", w_here, w_here - 1, ptr + w_left);
				standend ();
			}
		}
		else
			printw ("%-*.*s ", wwid - 1, wwid - 1, ptr);

		CELLREF ccl, cch;
		if (find_slop(r, c, &ccl, &cch))
		{
			change_slop(r, ccl, cch, c, cc);
			for (; cch > cc; --cch)
				if ((wwid = get_width (cch)))
				{
					move_cursor_to (win, r, cch);
					printw ("%*s", wwid, "");
				}
			for (cch = c - 1; cch > ccl; --cch)
				if ((wwid = get_width (cch)))
				{
					move_cursor_to (win, r, cch);
					printw ("%*s", wwid, "");
				}
			if (ccl != c)
				io_pr_cell (r, ccl, find_cell (r, ccl));
		}
		else
			set_slop(r, c, cc);
	}

	if(is_bold) wattr_off(stdscr, WA_BOLD, 0);
	if(is_italic) wattr_off(stdscr, WA_ITALIC, 0);
	if (glowing) cur_io_update_status ();

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






void io_pr_cell (CELLREF r, CELLREF c, CELL *cp)
{
	if(cwin && inside(r, c, cwin->screen))
		cur_io_pr_cell_win (cwin, r, c, cp);
}



void io_move_cell_cursor (CELLREF rr, CELLREF cc)
{
	if(inside(rr, cc, cwin->screen)) {
		//win_io_hide_cell_cursor(); // 25/5 apparently unnecessary
		curow = rr;
		cucol = cc;
	} else 	{
		curow = rr;
		cucol = cc;
		recenter_window(cwin);
	}

	if (get_scaled_width(cucol) == 0)
		find_nonzero(&cucol, cwin->screen.lc, cwin->screen.hc, get_scaled_width);
	if (get_scaled_height (curow) == 0)
		find_nonzero(&curow, cwin->screen.lr, cwin->screen.hr, get_scaled_height);
}

void io_shift_cell_cursor (dirn way, int repeat) // FN
{
	CELLREF c = cucol;
	CELLREF r = curow;
	int w = 0;
	int over;
	int down;

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




void curses_main () // FN
{

	//cur_io_open_display();
	initscr ();
	defer d{endwin};
	//defer d;
	scrollok (stdscr, 0);
	crmode ();
	raw ();
	noecho ();
	nonl ();
	start_color();
	curs_set(0); // turn the cursor off

	io_init_windows();
	recenter_window();
	//cur_io_repaint();


	// Tell ncurses to interpret "special keys". It means
	// that KEY_DOWN etc. will work, but ESC won't be
	// read separately
	keypad(stdscr, TRUE);

	show_menu();
	//doupdate();

	bool quit = false;
	while(!quit) {
		try {
			cur_io_repaint();
			quit = curses_loop();
		} catch (OleoJmp& e) {
			write_status(e.what());
		}
	}

	//delwin(main_menu);
	//endwin();
}


void  io_init_windows ()
{

	//io_set_input_status (1, 2, 0);
	//cwin->id = win_id++;
	//cwin->win_over = 0;		/* This will be fixed by a future set_numcols */
	//cwin->win_down = (label_rows + (user_status > 0) * status_rows + (user_input > 0) * input_rows);

	//auto [y, x] = win_getyx(stdscr);
	//int y, x;
	//getyx(stdscr, y, x);
	scr_lines = LINES;
	scr_cols = COLS;
	cwin->numr = (scr_lines - label_rows - !!user_status * status_rows - input_rows );
	cwin->numc = scr_cols;
	cwin->bottom_edge_r = 0;
	cwin->right_edge_c = 0;
	cwin->lh_wid = 0;

	/* at the end of this process
	 	 (gdb) p the_cwin
		$3 = {id = 1, win_over = 0, win_down = 3, screen = {lr = 0, lc = 0, hr = 0,
    	hc = 0}, numr = 21, numc = 80, bottom_edge_r = 0, right_edge_c = 0,
  	  lh_wid = 0}
		(gdb) n
	 */
}
