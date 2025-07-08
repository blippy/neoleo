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

module;

#include <ncurses.h>



//#include <string>

export module win;

import std;


// FN win_print .
// these should be nice generic functions
export void win_print(WINDOW *w, const char* str) // FN
{
	waddstr(w, str);
}

export void win_print(const char* str) // FN
{
	addstr(str);
}

export void win_print(WINDOW *w, const std::string& str) // FN
{
	win_print(w, str.c_str());
}

export void win_print(const std::string& str) // FN
{
	win_print(str.c_str());
}

export void win_print(int y, int x, const std::string& str) // FN
{
	move(y, x);
	win_print(str);
}
// FN-END

export std::tuple<int, int> win_getyx(WINDOW *win = stdscr)
{
	int y, x;
	getyx(win, y, x);
	//log("win_getyx ", y)
	return std::make_tuple(y, x);

}


// FN win_set .
export void win_set_line(WINDOW *w, const std::string& str)
{
	wmove(w, 0, 0);
	win_print(w, str);
	wclrtoeol(w);
	
}
// FN-END