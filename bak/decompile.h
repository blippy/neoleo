#pragma once
/*
 * $Id: decompile.h,v 1.4 2000/08/10 21:02:50 danny Exp $
 *
 * Copyright � 1993 Free Software Foundation, Inc.
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

#include <string>

#include "cell.h"

const char * backslash_a_string (char *string, int add_quote);
std::string decomp_formula(CELL* cell);
std::string decomp_str(const CELLREF r, const CELLREF c);
std::string decompile(const coord_t coord);
//std::string decomp_str(const CELLREF r, const CELLREF c, CELL *cell);
std::string decomp_formula(const CELLREF r, const CELLREF c, CELL *cell, int tog);
std::string decompile(); // decompile current cell

