/*
 * Copyright (c) 1990, 1992, 1993 Free Software Foundation, Inc.
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


#include <functional>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "io-utils.h"
#include "sheet.h"
#include "regions.h"
#include "basic.h"
#include "spans.h"

using std::cout;
using std::endl;

crefs_t coords_in_range (const rng_t& rng)
{
	crefs_t res;
	for(CELLREF r = rng.lr; r <= rng.hr; ++r) 
		for(CELLREF c = rng.lc; c <= rng.hc; ++c) 
			res.insert(to_coord(r, c));
	return res;

}




/* Turn on/off the locked bits in a region */
void lock_region (const struct rng&  where, int locked)
{
	Global_modified = 1;
	make_cells_in_range (where);
	for(CELL* cp:get_cells_in_range(where))
		SET_LCK (cp, locked);
}


void change_region(const struct rng& a_rng, std::function<void(CELL*)> fn)
{

	Global_modified = 1;
	make_cells_in_range (a_rng);
	for(CELL* cp:get_cells_in_range(a_rng))
	{
		CELLREF r, c;
		decoord(cp, r, c);
		fn(cp);
		//io_pr_cell (r, c, cp);		
	}
}
void format_region (const struct rng& where, int fmt, int just) // FN
{
	auto fn = [=](CELL* cp) {
		if (fmt != -1) 
			SET_FORMAT (cp, fmt);	/* Only the format, not the precision !! */
		if (just != -1)
			SET_JST (cp, just);
	};

	change_region(where, fn);

}


void precision_region (const struct rng& where, int precision)
{
	auto fn = [=](CELL* cp) {
		if (precision != -1)
			SET_PRECISION (cp, precision);
	};

	change_region(where, fn);
}



