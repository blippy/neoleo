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


 /*
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "convert.h"
#include "window.h"
#include "io-utils.h"
#include "io-term.h"
#include "cmd.h"
#include "sheet.h"
#include "regions.h"
#include "spans.h"
#include "io-curses.h"
*/

#include <string>
#include <ncurses.h>


// these should be nice generic functions
void win_print(WINDOW *w, const char* str) // FN
{
	waddstr(w, str);
}

void win_print(const char* str) // FN
{
	addstr(str);
}

void win_print(const std::string& str) // FN
{
	win_print(str.c_str());
}
void win_print(int y, int x, const std::string& str) // FN 
{
	move(y, x);
	win_print(str);
}




