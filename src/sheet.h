#pragma once
/*
 * Copyright © 1992, 1993, 1999 Free Software Foundation, Inc.
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

//class cell;

#include <deque>

#include "cell.h"
#include "neotypes.h"

//typedef std::map<coord_t, cell_t*> cellmap_t;

typedef std::deque<cell_t*> celldeq_t;

void decoord(CELL* cp, CELLREF& r, CELLREF& c);
void flush_cols();
struct cell* find_cell(CELLREF row, CELLREF col);
struct cell* find_or_make_cell(CELLREF row, CELLREF col);


void init_cells();
celldeq_t get_cells_in_range(struct rng *r);

void make_cells_in_range(struct rng *r);
void no_more_cells();

coord_t to_coord(coord_t row, coord_t col);
int get_col(coord_t coord);
int get_row(coord_t coord);
int get_col(CELL* cp);
int get_row(CELL* cp);
CELLREF max_row();
CELLREF max_col();
CELLREF highest_row();
CELLREF highest_col();

void dump_sheet();
