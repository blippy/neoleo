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


#include "global.h"

#include "byte-compile.h"
#include "cmd.h"
#include "eval.h"
#include "errors.h"
#include "format.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "io-term.h"
#include "io-utils.h"
#include "sheet.h"
#include "logging.h"
#include "ref.h"
#include "regions.h"
#include "spans.h"
#include "utils.h"
//#include "xcept.h"
#include "parser-2019.h"
using std::cerr;
using std::cout;
using std::endl;

static void log_debug_1(std::string msg)
{
	if constexpr(true)
		log_debug("DBG:cells.cc:" + msg);
}


value_t cell::get_value_2019() const
{
	return value_2019;
}
	
void cell::set_value_2019(value_t newval)
{
	value_2019 = newval;
	sValue(newval);
}

void cell::set_cyclic()
{
	set_value_2019(err_t{CYCLE});
}

void cell::set_error(const ValErr& ve)
{
	set_value_2019(ve.num());
}

cell::cell(coord_t coord) :coord(coord)
{
}

void cell::reparse()
{
	// TODO this doesn't work properly
	// parse_and_compile(this);
}

formula_t cell::get_bytecode()
{ 
	if(!bytecode)
		bytecode = parse_and_compile(this);

	return bytecode;
}

value cell::get_value()
{ 
	value v; 
	v.set_type(get_type()); 
	if(get_type() == TYP_STR) {
		const char* s = x.c_s;
		v.x.c_s = strdup(s);
	} else {
		v.x = x; 
	}
	return v; 
}


bool cell::locked() const
{
	return cell_flags.cell_lock;
}


void cell::recompute_bytecode()
{
	ASSERT_UNCALLED();
}
void cell::invalidate_bytecode()
{
	reset();
}

void cell::set_row(CELLREF r)
{
	this->coord = to_coord(r, get_col(this));
	this->invalidate_bytecode(); // due to relative referencing issues
}

void cell::reset()
{
	clear_bytecode();
}

void cell::clear_bytecode()
{
	free_nonempty_str(&bytecode);
}

void cell::clear_flags()
{
	//bzero((void*) this->cell_flags, sizeof(this->cell_flags));
	this->cell_flags.clear();
}

bool cell::zeroed_1()
{
	return (cell_flags.cell_format == 0)
		&& (cell_flags.cell_precision == 0)
		&& (cell_flags.cell_justify == 0)
		&& (get_type() == 0)
		&& (cell_flags.cell_lock == 0);

}


void cell::erase_predec_deps()
{
	for(auto rc: coords_in_ranges(predecs)) {
		CELL* cp = find_cell(rc);
		if(!cp) continue;
		cp->deps_2019.erase(rc);
	}
}
void cell::insert_predec_deps(coord_t coord)
{
	for(auto rc: coords_in_ranges(predecs)) {
		CELL* cp = find_or_make_cell(rc);
		//if(!cp) continue;
		cp->deps_2019.insert(coord);
	}
}
void cell::set_formula_text(const std::string& str)
{
	if(str ==formula_text) return;
	formula_text = str;

	if(use_parser_2019) {
		// erase_predec_deps(); TODO get this working properly
		predecs.clear();
		CELLREF r, c;
		decoord(this, r, c);
		parse_tree = parse_string(formula_text, predecs, r, c);
		insert_predec_deps(coord);

	} else
		invalidate_bytecode();
}

std::string cell::get_formula_text() const
{
	return formula_text;
}
		
void cell::set_refs(const crefs_t& coords)
{
	// TODO remove deps of old  precs

	prec_cells = coords;

	// this cell's prec becomes the other cells deps.
	for(auto& coord: coords) {
		cell* other = find_or_make_cell(coord);
		other->dep_cells.insert(this->coord);
	}
		
}

void cell::dump_cell()
{
	cout << "Col: " << get_col(this) << "\n";
	cout << "Row: " << get_row(this) << "\n";
	value val = get_value();
	cout << "Val: " << stringify_value_file_style(&val) << "\n";
	cout << "Frm: " << get_formula_text() << "\n";


	auto dump_coords = [](std::string field, crefs_t& coords) {
		for(const auto& coord: coords)
			cout << field << ": R" << get_row(coord) << "C" << get_col(coord) << "\n";
	};
	dump_coords("Dep", dep_cells);
	dump_coords("Prec", prec_cells);

	cout << "\n";
}

cell::~cell()
{
	magic = 0x0DEFACED; // see TR06
	cell::reset();
	//cout <<"X";
}

/*
value_t cell::get_value_t()
{
	return the_value_t;
}
*/


void copy_cell_stuff (cell* src, cell* dest)
{
	dest->cell_flags = src->cell_flags;
	dest->cell_refs_to = src->cell_refs_to;
	dest->set_formula_text(src->get_formula_text());
	dest->cell_cycle = src->cell_cycle;
	dest->set_formula_text(src->get_formula_text());
}

bool 
vacuous(cell* cp)
{
	return (cp == nullptr) || (cp->get_type() == TYP_NUL);
}

void set_cell_input(CELLREF r, CELLREF c, const std::string& new_input)
{
	curow = r;
	cucol = c;
	//edit_cell_str(new_input);
	set_and_eval(r, c, new_input, true);
}

std::string
get_cell_formula_at(int r, int c)
{
	return formula_text(r, c);

	//std::string res = decomp_str(r, c);
	//return res;
}











#define S (char *)
#define T (void (*)())
function_t cells_funs[] =
{

  {0, 0, "", 0, 0},
};

int init_cells_function_count(void) 
{
        return sizeof(cells_funs) / sizeof(function_t) - 1;
}

void edit_cell (const char* input)
{
	CELL* cp = find_or_make_cell(curow, cucol);
	cp->set_formula_text(input);
	new_value(curow, cucol, input);
}


void edit_cell_str (const std::string& new_formula)
{
	edit_cell(new_formula.c_str());
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

RC_t
ws_extent()
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

void
copy_this_cell_formula()
{
	m_copied_cell_formula = formula_text(curow, cucol);
}

void 
paste_this_cell_formula()
{	
	set_cell_input(curow, cucol, m_copied_cell_formula.c_str());
}

//////////////////////////////////////////////////////////////////////
