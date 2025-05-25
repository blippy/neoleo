#pragma once
/*
 * Copyright (c) 1992, 1993, 1999 Free Software Foundation, Inc.
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

// MANAGE THE COLLECTION OF CELLS, WHICH WE CALL A `SHEET'


//#include <deque>
#include <generator>

#include "cell.h"
#include "neotypes.h"


typedef std::deque<cell_t*> celldeq_t;

void 	clear_spreadsheet (void);
void 	copy_row(CELLREF dst_row, CELLREF src_row);
std::tuple<CELLREF, CELLREF> decoord(const CELL* cp);
void 	decoord(const CELL* cp, CELLREF& r, CELLREF& c);
void 	delete_all_cells();
void 	delete_sheet_row(coord_t row);
void 	dump_sheet();
cell* 	find_cell(CELLREF row, CELLREF col);
cell* 	find_cell(coord_t coord);
cell* 	find_or_make_cell(CELLREF row, CELLREF col);
cell_t* find_or_make_cell (coord_t coord);
cell_t* find_or_make_cell ();
std::generator<CELL*>	get_cells_in_range(const rng_t& a_rng);
int 	get_col(coord_t coord);
int 	get_col(const CELL* cp);
int 	get_row(const CELL* cp);
int 	get_row(coord_t coord);
CELLREF highest_col();
CELLREF highest_row();
void 	insert_col_left (coord_t col = cucol);
void 	insert_row_above(coord_t row);
bool 	inside(int val, int lo, int hi);
bool 	inside(int r, int c, const struct rng& a_rng);
bool 	inside(cell* cp, const struct rng& a_rng);
void 	make_cells_in_range(const struct rng& a_rng);
CELLREF max_col();
CELLREF max_row();
void 	no_more_cells();
coord_t to_coord(coord_t row, coord_t col);
