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



#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <vector>

#include "utils.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using namespace std::string_literals;

//#include <ncurses.h>
//#include <curses.h>
#include <menu.h>
#include <panel.h>

#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#undef NULL
#include "global.h"
#include "cell.h"
#include "cmd.h"
#include "io-curses.h"
#include "io-2019.h"
#include "io-term.h"
#include "io-utils.h"
#include "sheet.h"
#include "regions.h"
#include "spans.h"
#include "window.h"
#include "input.h"
#include <term.h>
#include "logging.h"
#include "ref.h"
#include "menu-2025.h"



void wprint(WINDOW *w, const char* str)
{
	waddstr(w, str);
}

void wprint(const char* str)
{
	addstr(str);
}

void wprint(const std::string& str)
{
	wprint(str.c_str());
}
void wprint(int y, int x, const std::string& str)
{
	move(y, x);
	wprint(str);
}


#define MIN_WIN_HEIGHT	(cwin->flags&WIN_EDGES ? 2 : 1)
#define MIN_WIN_WIDTH	(cwin->flags&WIN_EDGES ? 6 : 1)

#define VOIDSTAR void*

static int redrew = 0;
static int term_cursor_claimed = 0;

static void move_cursor_to (struct window *, CELLREF, CELLREF, int);
void cur_io_pr_cell_win (struct window *win, CELLREF r, CELLREF c, CELL *cp);



static int curses_metric (char * str, int len)
{
	return len;
}

static struct input_view input_view  = {0, curses_metric, curses_metric, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static void _io_redraw_input (void)
{
	int pos;
	int row = (input_view.current_info ? 0 : Global->input);

	if (input_view.info_redraw_needed)
	{
		input_view.info_redraw_needed = 0;
		_io_repaint ();
		return;
	}

	if (input_view.redraw_needed == NO_REDRAW)
		return;

	if (input_view.redraw_needed == FULL_REDRAW)
	{
		/* Redraw	the prompt. */ 
		move (row, 0);
		if (input_view.expanded_keymap_prompt)
		{
			addstr (input_view.expanded_keymap_prompt);
			clrtoeol ();
			input_view.redraw_needed = NO_REDRAW;
			return;
		}
		if (input_view.prompt_wid)
			addstr (input_view.prompt);
		pos = input_view.visibility_begin;
	}
	else
	{
		pos = input_view.redraw_needed;
		move (row,
				input_view.prompt_wid + pos - input_view.visibility_begin);
	}

	if (   input_view.input_area
			&& (input_view.visibility_end >= input_view.visibility_begin)
			&& (1))
	{
		int x;
		for (x = pos; x <= input_view.visibility_end; ++x) {
			//addch (input_view.input_area->buf[x]);
		}
	}
	clrtoeol ();
	input_view.redraw_needed = NO_REDRAW;
}





static void
_io_fix_input (void)
{
	iv_fix_input (&input_view);
}



void cur_io_display_cell_cursor (void)
{
	int cell_cursor_row;
	int cell_cursor_col;
	int cc;
	int rr;
	int cwid;
	int n;
	int x, y;

	if (input_view.current_info)
		return;

	if (   (curow < cwin->screen.lr)
			|| (cucol < cwin->screen.lc)
			|| (curow > cwin->screen.hr)
			|| (cucol > cwin->screen.hc))
		return;

	getyx (stdscr, y, x);
	cell_cursor_col = cwin->win_over;
	for (cc = cwin->screen.lc; cc < cucol; cc++)
		cell_cursor_col += get_width (cc);
	cell_cursor_row = cwin->win_down;
	for (rr = cwin->screen.lr; rr < curow; rr++)
		cell_cursor_row += get_height (rr);
	cwid = get_width (cucol);
	if (cwid > cwin->numc)
		cwid = cwin->numc;
	move (cell_cursor_row, cell_cursor_col);
	standout ();
	for (n = cwid; n; n--)
#ifdef A_STANDOUT
		addch (inch () | A_STANDOUT);
#else
	addch (inch ());
#endif
	standend ();
	move (y, x);
}

void win_io_hide_cell_cursor (void)
{
	int cc;
	int rr;
	int cell_cursor_row;
	int cell_cursor_col;
	int cwid;
	int n;
	int x, y;

	if (input_view.current_info)
		return;
	if (   (curow < cwin->screen.lr)
			|| (cucol < cwin->screen.lc)
			|| (curow > cwin->screen.hr)
			|| (cucol > cwin->screen.hc))
		return;
	getyx (stdscr, y, x);
	cell_cursor_col = cwin->win_over;
	for (cc = cwin->screen.lc; cc < cucol; cc++)
		cell_cursor_col += get_width (cc);
	cell_cursor_row = cwin->win_down;
	for (rr = cwin->screen.lr; rr < curow; rr++)
		cell_cursor_row += get_height (rr);
	cwid = get_width (cucol);
	if (cwid > cwin->numc)
		cwid = cwin->numc;
	move (cell_cursor_row, cell_cursor_col);
	for (n = cwid; n; n--)
#ifdef A_STANDOUT
		addch (inch () & ~A_STANDOUT);
#else
	addch (inch ());
#endif
	move (y, x);
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

class curses_display {
	public:
		curses_display();
		void activate();
		~curses_display();
		void cdstandout();
	private:
		bool m_activated = false;
};

static curses_display s_display;

void cur_io_open_display() { s_display.activate(); }

curses_display::curses_display()
{
}


curses_display::~curses_display()
{
	log_debug("TODO Calling curses_display::~curses_display()");

}



/* called by _io_open_display() */
void curses_display::activate()
{
	if(m_activated) return;
	m_activated = true;
	initscr ();
	scrollok (stdscr, 0);
	crmode ();
	raw ();
	noecho ();
	nonl ();
	start_color();

	io_init_windows();
	info_rows = 1;

}

void curses_display::cdstandout()
{
	this->activate(); // ASAN complains otherwise
	standout(); 
}


void cont_curses(void)
{
	crmode ();
	raw ();
	noecho ();
	nonl ();
}

static void _io_redisp (void)
{
	if (!term_cursor_claimed)
	{
		_io_redraw_input();
		if (!(input_view.current_info || input_active ||
					input_view.expanded_keymap_prompt))
			move_cursor_to (cwin, curow, cucol, 0);
		else
			move ((input_view.current_info ? 0 : Global->input), 
					input_view.prompt_wid + input_view.input_cursor -
					input_view.visibility_begin); 
	}
	{
		struct rng * rng = &cwin->screen;
		if (   (curow > rng->hr)
				|| (curow < rng->lr)
				|| (cucol > rng->hc)
				|| (cucol < rng->lc))
			io_recenter_cur_win ();
	}
	refresh ();
}

void stop_curses(void)
{
	nocrmode ();
	noraw ();
	echo ();
	nl ();
	_io_redisp ();
}



void win_io_repaint_win (struct window *win)
{
	_io_repaint ();
}

static char* col_to_str (CELLREF col)
{
	static char buf[10];
	string str = "";
	while(col>=MIN_COL) {
		char ch = 'A' + (col-MIN_COL) % 26 ;
		str = string{ch} + str;
		col = (col-MIN_COL)/26;
	}
	strncpy(buf, str.c_str(), sizeof(buf)-1);
	return buf;
}


int run_bug44_tests ()
{
	auto f = [](CELLREF c, const char* expected) { 
		const char* out = col_to_str(c); 
		if(strcmp(out, expected) == 0) {
			cout << "PASS ";
		} else {
			cout << "FAIL ";
		}
		cout << c << " " << expected << " vs " << col_to_str(c) << "\n";
	};

	f(MIN_COL, "A");
	f(26, "Z");
	f(27, "AA");
	f(600, "WB");
	f(MIN_COL + 702 -1, "ZZ");
	f(MIN_COL + 702 , "AAA");
	f(1000, "ALL");
	f(MIN_COL + 18278-1, "ZZZ");
	f(MIN_COL + 18278, "AAAA");
	f(20000, "ACOF");
	return 1;
}

void cur_io_update_status (void) // FN
{
	const char *ptr;
	int wid;
	int plen;
	int yy, xx;

	if (!user_status || input_view.current_info)
		return;
	getyx (stdscr, yy, xx);
	move (Global->status, 0);
	wid = columns - 2;

	if (mkrow != NON_ROW)
	{
		struct rng r;

		addch ('*');
		--wid;
		set_rng (&r, curow, cucol, mkrow, mkcol);
		ptr = range_name (&r);
	}
	else
		ptr = cell_name (curow, cucol);

	addstr (ptr);
	wid -= strlen (ptr);


	std::string dec = formula_text(curow, cucol);
	const std::string& cvs = cell_value_string(curow, cucol, 1);
	ptr = cvs.c_str();

	plen = strlen (ptr);

	int dlen = dec.size();
	if(dlen>0)
	{
		wid -= 4;
		if (dlen + plen > wid)
		{
			if (plen + 3 > wid)
				printw (" %.*s... [...]", wid - 6, ptr);
			else
				printw (" %s [%.*s...]", ptr, wid - plen - 3, dec.c_str());
		}
		else
			printw (" %s [%s]", ptr, dec.c_str());
	}
	else if (plen)
	{
		--wid;
		if (plen > wid)
			printw (" %.*s...", wid - 3, ptr);
		else
			printw (" %s", ptr);
	}

	clrtoeol ();
	move (yy, xx);
}


void _io_repaint (void)
{
	
	CELLREF cc, rr;
	int n, n1;
	CELL *cp;
	struct window *win = cwin;

	clear ();
	_io_fix_input ();
	redrew++;
	show_menu();
	
	if(input_view.current_info) return;

	if (win->lh_wid)
	{
		move (win->win_down - 1, win->win_over - win->lh_wid);
		//static_assert(std::is_same<decltype(win), void*>::value, "printw() might be wrong");
		static_assert(sizeof(win) == sizeof(void*), "printw() might be wrong");
		static_assert(sizeof(win) == sizeof(long int), "printw() might be wrong");
		printw ("#%*ld ", win->lh_wid - 2, (long int)1);
		if (win_flags & WIN_EDGE_REV)
			s_display.cdstandout();
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

				if (Global->a0)
					ptr = col_to_str (cc);
				else
				{
					sprintf (buf, "C%u", cc);
					ptr = buf;
				}
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

		rr = win->screen.lr;
		n = win->win_down;
		do
		{
			n1 = get_height (rr);
			if (n1)
			{
				move (n, win->win_over - win->lh_wid);
				if (Global->a0)
					printw ("%-*d ", win->lh_wid - 1, rr);
				else
					printw ("R%-*d", win->lh_wid - 1, rr);
				n += n1;
			}
		}
		while (rr++ < win->screen.hr);

		if (win_flags & WIN_EDGE_REV)
			standend ();
	}
	flush_slops();
	for(CELL* cp: get_cells_in_range(&(win->screen))) {
		decoord(cp, rr, cc);
		if (cp->get_type() != TYP_NUL)
			cur_io_pr_cell_win(win, rr, cc, cp);
	}
	
	if (!(cp = find_cell (curow, cucol)) || (cp->get_type() == TYP_NUL))
		cur_io_display_cell_cursor ();
	input_view.redraw_needed = FULL_REDRAW;
	_io_redraw_input();
	cur_io_update_status ();
}





static void move_cursor_to (struct window *win, CELLREF r, CELLREF c, int dn)
{
	int cc;
	int cell_cursor_col;
	int rr;
	int cell_cursor_row;

	cell_cursor_col = win->win_over;
	for (cc = win->screen.lc; cc < c; cc++)
		cell_cursor_col += get_width (cc);
	cell_cursor_row = win->win_down + dn;
	for (rr = win->screen.lr; rr < r; rr++)
		cell_cursor_row += get_height (rr);
	move (cell_cursor_row, cell_cursor_col);
}


extern int auto_recalc;



void cur_io_pr_cell (CELLREF r, CELLREF c, CELL *cp) // FN
{
	cur_io_pr_cell_win(cwin, r, c, cp);
}

void cur_io_pr_cell_win (struct window *win, CELLREF r, CELLREF c, CELL *cp) // FN
{
	//log("_io_pr_cell_win:", cp);
	int glowing;
	int lenstr;
	int j;
	int wid, wwid;
	int hgt;
	int yy, xx;

	if (input_view.current_info)
		return;

	wid = get_width (c);
	if (!wid)
		return;
	if (wid > win->numc)
		wid = win->numc;
	hgt = get_height (r);
	if (!hgt)
		return;
	if (hgt > win->numr)
		hgt = win->numr;

	getyx (stdscr, yy, xx);
	glowing = (r == curow && c == cucol && win == cwin);
	
	const std::string& str = print_cell(cp);
	char ptr1[str.size()+1];
	char* ptr = strcpy(ptr1, str.c_str());

	move_cursor_to (win, r, c, 0);
	if (glowing) standout ();

	bool is_bold = cp->cell_flags.bold;
	if(is_bold) wattr_on(stdscr, WA_BOLD, 0);

	bool is_italic = cp->cell_flags.italic;
	if(is_italic) wattr_on(stdscr, WA_ITALIC, 0);



	j = GET_JST (cp);
	if (j == JST_DEF)
		j = default_jst;
	lenstr = strlen (ptr);

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
						move_cursor_to (win, r, ccdh, 0);
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
					move_cursor_to (win, r, cch, 0);
					printw ("%*s", wwid, "");
				}
			io_pr_cell (r, ccl, find_cell (r, ccl));
		}
	}
	else
	{
		CELLREF cc = c;
		CELL *ccp;
		CELLREF ccl, cch;

		for (wwid = wid; lenstr > wwid - 1; wwid += get_width (cc))
		{
			if (++cc > win->screen.hc
					|| ((ccp = find_cell (r, cc))
						&& (ccp->get_type() != TYP_NUL)
						&& (GET_FORMAT (ccp) != FMT_HID
							|| (GET_FORMAT (ccp) == FMT_DEF
								&& default_fmt != FMT_HID))))
			{
				--cc;
				break;
			}
		}

		if (lenstr > wwid - 1) { 
			if (cp->get_type() == TYP_FLT)
				ptr = adjust_prc (ptr, cp, wwid - 1, wid - 1, j);
			else if (cp->get_type() == TYP_INT)
				ptr = (char *) numb_oflo;
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

		if (find_slop(r, c, &ccl, &cch))
		{
			change_slop(r, ccl, cch, c, cc);
			for (; cch > cc; --cch)
				if ((wwid = get_width (cch)))
				{
					move_cursor_to (win, r, cch, 0);
					printw ("%*s", wwid, "");
				}
			for (cch = c - 1; cch > ccl; --cch)
				if ((wwid = get_width (cch)))
				{
					move_cursor_to (win, r, cch, 0);
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
	move (yy, xx);
}
