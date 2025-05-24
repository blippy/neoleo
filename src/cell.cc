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

#include <iostream>
#include <string.h>
#include <string>

#include "neotypes.h"
#include "cell.h"
#include "regions.h"
#include "sheet.h"
#include "spans.h"

import value;
using std::cerr;
using std::cout;
using std::endl;
using std::get;




void set_precision(CELL* cp, int precision)
{
	cp->cell_flags.cell_precision = precision;
}

bool is_nul(CELL* cp)
{
	if(!cp) return true;
	return std::holds_alternative<std::monostate>(cp->get_value_2019());
}

bool is_flt(CELL* cp)
{
	return (cp && std::holds_alternative<num_t>(cp->get_value_2019()));
}

bool is_range(CELL* cp)
{
	return (cp && std::holds_alternative<rng_t>(cp->get_value_2019()));
}

int cell::width()
{
	return get_width(get_col(this->coord));
}

num_t cell::to_num()
{
	return std::get<num_t>(value_2019);
}

value_t cell::get_value_2019() const
{
	return value_2019;
}
	
void cell::set_value_2019(value_t newval)
{
	value_2019 = newval;
}

void cell::set_cyclic()
{
	set_value_2019(err_t{CYCLE});
}


cell::cell(coord_t coord) :coord(coord)
{
}

void cell::reparse()
{
	// TODO this doesn't work properly
	// parse_and_compile(this);
}



bool cell::locked() const
{
	return cell_flags.cell_lock;
}



void cell::set_row(CELLREF r)
{
	this->coord = to_coord(r, get_col(this));
}


coord_t cell::get_coord() const { return coord;} 

void cell::set_coord(coord_t coord) {	this->coord = coord; }

#if 1
static crefs_t coords_in_ranges(const ranges_t& ranges)
{
	crefs_t coords;
	for(const auto& rng: ranges)
		for(auto coord: coords_in_range(rng))
			coords.insert(coord);
	return coords;

}
#endif

void cell::erase_predec_deps()
{
#if 0
	for(const auto& rng: predecs) {
		for(auto rc: coords_in_range(rng)) {
			if(CELL *cp = find_cell(rc)) cp->deps_2019.erase(rc);
		}

	}
#endif
#if 1
	for(auto rc: coords_in_ranges(predecs)) {
		CELL* cp = find_cell(rc);
		if(!cp) continue;
		cp->deps_2019.erase(rc);
	}
#endif
}
void cell::insert_predec_deps(coord_t coord)
{
	for(auto rc: coords_in_ranges(predecs)) {
		CELL* cp = find_or_make_cell(rc);
		cp->deps_2019.insert(coord);
	}
}
void cell::set_formula_text(const std::string& str)
{
	if(str ==formula_text) return;
	formula_text = str;

	// erase_predec_deps(); TODO get this working properly
	predecs.clear();
	CELLREF r, c;
	decoord(this, r, c);
	parse_tree = parse_string(formula_text, predecs, r, c);
	insert_predec_deps(coord);

}

std::string cell::get_formula_text() const
{
	return formula_text;
}
	


void cell::dump_cell()
{
	cout << "Col: " << get_col(this) << "\n";
	cout << "Row: " << get_row(this) << "\n";
	cout << "Frm: " << get_formula_text() << "\n";
	cout << "\n";
}

cell::~cell()
{
	magic = 0x0DEFACED; // see TR06
}



bool vacuous(cell* cp)
{
	return is_nul(cp);
}

void set_cell_input(CELLREF r, CELLREF c, const std::string& new_input)
{
	curow = r;
	cucol = c;
	set_and_eval(r, c, new_input, true);
}

std::string get_cell_formula_at(int r, int c)
{
	return formula_text(r, c);
}

void edit_cell (const char* input)
{
	CELL* cp = find_or_make_cell(curow, cucol);
	cp->set_formula_text(input);
}





/*
 * highest_row() and highest_col() should be expected to 
 * overallocate the number of rows and columns in the 
 * spreadsheet. The true number of rows and columns must 
 * be calculated
 *
 * TODO this is likely to be a very useful function
 *
 */

RC_t ws_extent()
{
	int capacity_r = highest_row(), capacity_c = highest_col();
	//cell* m[capacity_r][capacity_c] = {};
	int size_r = 0, size_c = 0;
	for(int r=0; r<capacity_r; ++r)
		for(int c=0; c < capacity_c; ++c){
			cell * cp = find_cell(r+1, c+1);
			if(vacuous(cp)) continue;
			size_r = std::max(size_r, r+1);
			size_c = std::max(size_c, c+1);
			//m[r][c] = cp;
		}
	return RC_t{size_r, size_c};
}


std::string formula_text(CELLREF r, CELLREF c){
	CELL* cp = find_cell(r, c);
	if(cp==nullptr) return "";
	return cp->get_formula_text();
}
//////////////////////////////////////////////////////////////////////
// for copying and pasting cells


static std::string m_copied_cell_formula = "";

void copy_this_cell_formula()
{
	m_copied_cell_formula = formula_text(curow, cucol);
}

void paste_this_cell_formula()
{	
	set_cell_input(curow, cucol, m_copied_cell_formula.c_str());
}

//////////////////////////////////////////////////////////////////////
