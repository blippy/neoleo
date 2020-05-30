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


/* Functions for dealing exclusively with variables */
std::map<std::string, struct var>the_vars_1;



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
	cp->update_cell();
	io_pr_cell (row, col, cp);
	Global->modified = 1;
	return 0;
}


/* --------- Routines for dealing with cell references to other cells ------ */


/* like add_ref, except over a range of arguments and with memory
 * management weirdness. 
 */
void add_range_ref (struct rng *rng)
{
	make_cells_in_range (rng);
}


	void
add_ref_to (cell* cp, int whereto)
{
}




/* ---------- Routines and vars for dealing with the eval FIFO ------------ */


void push_refs (cell *cp)
{
}


	void
push_cell (CELLREF row, CELLREF col)
{
	ASSERT_UNCALLED();
}




/* This sets the variable V_NAME to V_NEWVAL
 * It returns error msg, or 0 on success.
 * all the appropriate cells have their ref_fm arrays adjusted appropriately
 * This could be smarter; when changing a range var, only the cells that
 * were in the old value but not in the new one need their references flushed,
 * and only the cells that are new need references added.
 * This might also be changed to use add_range_ref()?
 */


	void
for_all_vars (void (*func) (const char *, struct var *))
{
	for(auto it = the_vars_1.begin(); it != the_vars_1.end() ; ++it) {
		auto s1{it->first};
		const char* s3 = s1.c_str();
		func(s3, &(it->second));
	}
}


/* Free up all the variables, and (if SPLIT_REFS) the ref_fm structure
   associated with each variable.  Note that this does not get rid of
   the struct var *s in cell expressions, so it can only be used when all
   the cells are being freed also
   */
	void
flush_variables (void)
{
	the_vars_1.clear();
}
