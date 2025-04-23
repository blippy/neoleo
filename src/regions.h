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

#include "global.h"
#include "cell.h"

extern struct rng all_rng;

void set_rng (struct rng *r, CELLREF r1, CELLREF c1, CELLREF r2, CELLREF c2);
void lock_region (struct rng *where, int locked);
void format_region (struct rng *where, int fmt, int just);
void move_region (struct rng *fm, struct rng *to);
void copy_region (struct rng *fm, struct rng *to);
void copy_values_region (struct rng *fm, struct rng *to);
crefs_t coords_in_range (const rng_t& rng);
crefs_t coords_in_ranges (const ranges_t& rng);
