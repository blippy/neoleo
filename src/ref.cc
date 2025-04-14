/*
 * Copyright (c) 1990, 1992, 1993, 2001 Free Software Foundation, Inc.
 * 
 * This file is part of Oleo, the GNU Spreadsheet.
 * 
 * Oleo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * Oleo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Oleo; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>

#include <iostream>
#include <map>
#include <math.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>


#include "global.h"
#include "io-abstract.h"
#include "ref.h"
#include "cmd.h"
#include "sheet.h"
#include "logging.h"

using std::cout;
using std::endl;




/* Set the cell ROW,COL to STRING, parsing string as needed */
	static CELL*
set_cell (CELLREF row, CELLREF col, const std::string& in_string)
{
	cur_row = row;
	cur_col = col;

	std::string s2{in_string};
	while(s2.size() > 0 && s2[0] == ' ') s2.erase(0, 1);

	//my_cell = find_cell (cur_row, cur_col);
	return find_or_make_cell(cur_row, cur_col);

}

extern int default_lock;

/* new_value() calls set_cell, but refuses to change locked cells, and
   updates and prints the results.  It returns an error msg on error. . .
   */

	char *
new_value (CELLREF row, CELLREF col, const char *string)
{
	CELL *cp = find_cell (row, col);
	if (((!cp || GET_LCK (cp) == LCK_DEF) && default_lock == LCK_LCK) || (cp && GET_LCK (cp) == LCK_LCK))
	{
		return (char *) "cell is locked";
	}

	cp = set_cell(row, col, string);
	io_pr_cell (row, col, cp);
	Global->modified = 1;
	return 0;
}



