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

extern void flush_everything (void);
struct cell * find_cell (CELLREF row, CELLREF col);
struct cell *find_or_make_cell (CELLREF row, CELLREF col);
extern void init_cells (void);
extern void find_cells_in_range (struct rng *r);
extern void make_cells_in_range (struct rng *r);
extern struct cell * next_cell_in_range (void);
extern struct cell * next_row_col_in_range (CELLREF *rowp, CELLREF *colp);
extern void no_more_cells (void);
extern CELLREF max_row (CELLREF col);
extern CELLREF max_col (CELLREF row);
extern CELLREF highest_row (void);
extern CELLREF highest_col (void);

struct cf
{
	struct cf *next;
	struct find *rows, *cols;
	int make;
};

struct list
{
	CELLREF lo, hi;
	struct list *next;
	char mem[1];
};
