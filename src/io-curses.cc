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


//void 		io_move_cell_cursor (CELLREF rr, CELLREF cc);
bool 		curses_input ();


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


static std::string status_line (int wid)
{
	std::string result = std::format("r{}c{} ", curow, cucol);
	std::string cvs{cell_value_string(curow, cucol, 1)};
	result += cvs;
	if(result.size() > COLS) {
		return pad_right(result, COLS-3, true) + "...";
	}

	std::string form = get_formula_text(curow, cucol);
	if(result.size() + form.size() + 3 > COLS) {
		form = pad_right(form, COLS- result.size()-6, true) + "...";
	}
	result += " [" + form + "]";

	return result;
}
static void cur_io_update_status (void) // FN
{

	move (status, 0);
	//int wid = cwin->screen.hc; // columns - 2;
	int wid = COLS;
	win_print(status_line(wid));
	clrtoeol();
}




static void cur_io_repaint ()
{
	//io_recenter_cur_win();
	CELLREF cc, rr;
	int n, n1;
	window_c *win = cwin;

	erase();
	win->update(COLS, LINES);
	//clear();
	show_menu();
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
			if (cc != cucol) standout();

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

			standend ();
		} while (cc++ < win->screen.hc);

		// print row labels
		rr = win->screen.lr;
		n = win->win_down;
		do {
			if (rr != curow) standout();
			n1 = get_height (rr);
			if (!n1) continue;
			move (n, win->win_over - win->lh_wid());
			printw ("R%-*d", win->lh_wid() - 1, rr);
			n += n1;
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
	int cwid = std::min(the_cwin.numc, get_width (cucol));
	standout ();
	for (int n = cwid; n; n--)
		addch (inch () | A_STANDOUT);
	standend ();

	cur_io_update_status ();
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
