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



//#include <format>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
//#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <format>

#include <ncurses.h>
//#include <menu.h>
//#include <panel.h>


#include "cell.h"
#include "io-2019.h"
#include "io-curses.h"
#include "menu-2025.h"
#include "neotypes.h"
#include "regions.h"
#include "sheet.h"
#include "wsht.hh"
#include "spans.h"
#include "vidi.h"
#include "win.h"


//import std;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using namespace std::string_literals;





inline window_c the_cwin;
inline window_c* cwin = &the_cwin;
#define	win_id		Global->win_id
#define get_scaled_width the_wsht.get_scaled_width


//void 		io_move_cell_cursor (CELLREF rr, CELLREF cc);
bool 		curses_input ();

void mouse_button1_clicked(int mousex, int mousey)
{
	cwin->set_cursor_from_mouse(mousex, mousey);
}


// FN page_down
void page_down ()
{
	curow = std::min(cwin->screen.hr+1, (int) MAX_ROW);
	cwin->io_move_cell_cursor(curow, cucol);
}
// FN-END

// FN page_up
void page_up ()
{
	curow = std::max(1, cwin->screen.lr-1);
	cwin->io_move_cell_cursor(curow, cucol);
}
// FN-END


static std::string formula_line (int wid)
{
	std::string result = std::format("r{}c{} ", curow, cucol);
	std::string cvs{cell_value_string(curow, cucol, 1)};
	result += cvs;
	if(result.size() > COLS) {
		return pad_right(result, COLS-3, true) + "...";
	}

	std::string form = get_formula_text(curow, cucol);
	int remaining_cols =  COLS- result.size()-6; // reserve 6 for " [...]"
	if(remaining_cols <= 0) return result;
	//log("remaining_cols:", remaining_cols);
	if(form.size()  > remaining_cols) {
		form = form.substr(0, std::max(0,remaining_cols)) + "...";
		//log("shortened formula:", form);
	}
	result += " [" + form + "]";

	return result;
}



static void cur_io_repaint () // FN
{
	//io_recenter_cur_win();
	CELLREF cc, rr;
	int n, n1;
	window_c *win = cwin;

	erase();
	win->update(COLS, LINES);
	//clear();
	show_menu();

	// show formula line
	move (status, 0);
	int wid = COLS;
	win_print(formula_line(wid));
	clrtoeol();

	show_status();

	if (win->lh_wid())
	{
		move (win->win_down - 1, win->win_over - win->lh_wid());
		//static_assert(std::is_same<decltype(win), void*>::value, "printw() might be wrong");
		static_assert(sizeof(win) == sizeof(void*), "printw() might be wrong");
		static_assert(sizeof(win) == sizeof(long int), "printw() might be wrong");
		printw ("#%*ld ", win->lh_wid() - 2, (long int)1);

		// draw column labels
		cc = win->screen.lc;
		do
		{
			standout();
			if (cc == cucol) wattr_on(stdscr, WA_BOLD, 0);

			n = win->get_vid_col_width (cc);
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

			wattr_off(stdscr, WA_BOLD, 0);
			standend ();
		} while (cc++ < win->screen.hc);

		// print row labels
		rr = win->screen.lr;
		n = win->win_down;
		do {
			//if (rr != curow) standout();
			standout();
			if(rr == curow) wattr_on(stdscr, WA_BOLD, 0);
			n1 = get_height (rr);
			if (!n1) continue;
			move (n, win->win_over - win->lh_wid());
			printw ("R%-*d", win->lh_wid() - 1, rr);
			n += n1;
			wattr_off(stdscr, WA_BOLD, 0);
			standend ();
		} while (rr++ < win->screen.hr);



	}


	// show all the cells in the display
	for(const auto& vc : the_cwin.get_vidi_cells()) {
		if(vc.cell_flags.bold) wattr_on(stdscr, WA_BOLD, 0);
		if(vc.cell_flags.italic) wattr_on(stdscr, WA_ITALIC, 0);
		move(vc.cursr, vc.cursc);
		win_print(vc.str);
		wattr_off(stdscr, WA_BOLD, 0);
		wattr_off(stdscr, WA_ITALIC, 0);
	}



	// display cell cursor
	assert(inside(curow, cucol, cwin->screen));
	auto [y, x] = the_cwin.get_cursor(curow, cucol);
	move(y, x);
	int cwid = win->get_vid_col_width (cucol);
	standout ();
	for (int n = cwid; n; n--)
		addch (inch () | A_STANDOUT);
	standend ();


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

	cwin->io_move_cell_cursor (r, c);
}



static void close_curses () // FN
{
	endwin();
	//cout << "close_curses called\n";
}

void curses_main () // FN
{

	setlocale(LC_ALL, ""); // helpful for unicode
	initscr ();
	defer d{close_curses};
	scrollok (stdscr, 0);
	crmode ();
	raw ();
	noecho ();
	nonl ();
	start_color();
	init_pair(BL_ON_CY, COLOR_BLACK, COLOR_CYAN);
	init_pair(GR_ON_BL, COLOR_GREEN, COLOR_BLACK);
	curs_set(0); // turn the cursor off
	mousemask(ALL_MOUSE_EVENTS, NULL);

	cwin->update(COLS, LINES);
	cwin->recenter_window();


	// Tell ncurses to interpret "special keys". It means
	// that KEY_DOWN etc. will work, but ESC won't be
	// read separately
	keypad(stdscr, TRUE);

	show_menu();
	while(!Global_definitely_quit) {
		try {
			cur_io_repaint();
			curses_input();
		} catch (OleoJmp& e) {
			set_status(e.what());
		}
	}
}
