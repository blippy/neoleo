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
#include "io-curses.h"
#include "io-2019.h"
#include "io-utils.h"
#include "sheet.h"
#include "regions.h"
#include "spans.h"
#include "window.h"
#include <term.h>
#include "logging.h"
#include "ref.h"
#include "menu-2025.h"



CELLREF mkrow = NON_ROW;
CELLREF mkcol = NON_COL;
const int input_active = 0;

#define MIN_WIN_HEIGHT	(cwin->flags&WIN_EDGES ? 2 : 1)
#define MIN_WIN_WIDTH	(cwin->flags&WIN_EDGES ? 6 : 1)
//#define MIN_WIN_HEIGHT(W) (W->bottom_edge_r + label_rows * (W->flags & WIN_EDGES ? 2 : 1))

//#define MIN_WIN_WIDTH(W) (W->right_edge_c + label_emcols * (W->flags & WIN_EDGES ? 6 : 1))
#define MIN_CWIN_HEIGHT  MIN_WIN_HEIGHT(cwin)
#define MIN_CWIN_WIDTH  MIN_WIN_WIDTH(cwin)


static int redrew = 0;
static int term_cursor_claimed = 0;

static void move_cursor_to (struct window *, CELLREF, CELLREF);
void cur_io_pr_cell_win (struct window *win, CELLREF r, CELLREF c, CELL *cp);


/* Display-generic updating logic for the input area. */
typedef int (*text_measure) (char * str, int len);

/* These are for the field REDRAW_NEEDED */
#define NO_REDRAW               -2
#define FULL_REDRAW      	-1

struct input_view
{
        /* If this is less than 0, see the #defines above.
         * >= 0, this is the index of a character in the 
         * input string.  All characters at that index and 
         * greater need to be redrawn.
         */
        int redraw_needed;


        /* If the currently mapped keymap has a prompt, the display of that
         * prompt takes precedence.
         */
        char * expanded_keymap_prompt;

        /* This is the width of either the keymap_prompt or the input text
         * prompt, whichever is current (0 if neither is).
         */

        int prompt_wid;

        /* The parameters below are a cache.  If this flag is true,
         * the cache is known to be wrong.
         */

        struct line * input_area;       /* The text editted in the input area or 0. */
        char * prompt;
        int visibility_begin;           /* Index of first visible char or 0. */
        int visibility_end;             /* Index of last visible char or 0. */
        int input_cursor;               /* Index of the cursor position or 0. */
        int vis_wid;                    /* This is the width of the visible text 
                                         * with extra space for the cursor, if it 
                                         * happens to be past the end of the string.
                                         */

        /* A command_arg can specify an info buffer which should be displayed 
         * while prompting for that arg.
         */
        struct info_buffer * current_info;
        int info_redraw_needed; /* != 0 if redraw needed */
};






static int curses_metricXXX (char * str, int len)
{
	return len;
}

static struct input_view input_view{0};

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



#if 0

static void
_io_fix_input (void)
{
	iv_fix_input (&input_view);
}
#endif


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
			move_cursor_to (cwin, curow, cucol);
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
	//_io_fix_input ();
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





static void move_cursor_to (struct window *win, CELLREF r, CELLREF c)
{
	int cc;
	int cell_cursor_col;
	int rr;
	int cell_cursor_row;

	cell_cursor_col = win->win_over;
	for (cc = win->screen.lc; cc < c; cc++)
		cell_cursor_col += get_width (cc);
	cell_cursor_row = win->win_down;
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

	move_cursor_to (win, r, c);
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
	move (yy, xx);
}



/* Low level window operators. */



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
	lh *= label_emcols;
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



void  recenter_window (struct window *win) // FN
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

void io_recenter_cur_win (void)
{
	io_recenter_named_window (cwin);
	win_io_repaint_win(cwin);
}

void io_recenter_all_win(void)
{
	if (!nwin) return;
	io_recenter_named_window (cwin);
	//win_io_repaint ();
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








void io_set_input_statusXXX (int inp, int stat, int redraw)
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
			//win_io_repaint ();
		}
		user_input = new_ui;
		user_status = new_us;
		Global->input = new_inp;
		Global->status = new_stat;
	}
}

void  io_init_windows () 
{

	//io_set_input_status (1, 2, 0);
	cwin->id = win_id++;
	cwin->win_over = 0;		/* This will be fixed by a future set_numcols */
	cwin->win_down = (label_rows + (user_status > 0) * status_rows + (user_input > 0) * input_rows);
	cwin->numr = (Global->scr_lines - label_rows - !!user_status * status_rows - input_rows - default_bottom_border);
	cwin->numc = Global->scr_cols - default_right_border;
	cwin->bottom_edge_r = default_bottom_border;
	cwin->right_edge_c = default_right_border;
	cwin->lh_wid = 0;

	/* at the end of this process
	 	 (gdb) p the_cwin
		$3 = {id = 1, win_over = 0, win_down = 3, screen = {lr = 0, lc = 0, hr = 0,
    	hc = 0}, numr = 21, numc = 80, bottom_edge_r = 0, right_edge_c = 0,
  	  lh_wid = 0}
		(gdb) n
	 */
}
