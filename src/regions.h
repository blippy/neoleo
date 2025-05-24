#pragma once
/*
 * Copyright (c) 1992, 1993 Free Software Foundation, Inc.
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

//#include "global.h"
#include "cell.h"


void lock_region (const struct rng& where, int locked);
crefs_t coords_in_range (const rng_t& rng);
const inline struct rng all_rng = {MIN_ROW, MIN_COL, MAX_ROW, MAX_COL};
void format_region (const struct rng& where, int fmt, enum jst just);
void set_rng (const struct rng& r, CELLREF r1, CELLREF c1, CELLREF r2, CELLREF c2);
