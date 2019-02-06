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

#include "cell.h"
#include "mem.h"

formula_t parse_and_compile (cell* cp);
formula_t parse_and_compile(cell* cp, const std::string& str);
formula_t parse_and_compile (cell* cp, const char *string);
formula_t parse_and_compile (cell* cp, const char *string, mem_c& the_mem);
extern void byte_free (unsigned char *form);
extern int is_constant (const unsigned char *bytes);


