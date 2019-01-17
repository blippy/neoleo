/*
 * $Id: cells.c,v 1.12 2000/08/10 21:02:50 danny Exp $
 *
 * Copyright © 1990, 1992, 1993 Free Software Foundation, Inc.
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

#include "funcdef.h"
#include <iostream>
#include <string.h>
#include <string>


#include "global.h"

#include "byte-compile.h"
#include "cell.h"
#include "cmd.h"
#include "decompile.h"
#include "eval.h"
#include "errors.h"
#include "format.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "io-term.h"
#include "sheet.h"
#include "logging.h"
#include "ref.h"
#include "spans.h"
#include "utils.h"

using std::cerr;
using std::cout;
using std::endl;

static void log_debug_1(std::string msg)
{
	if constexpr(true)
		log_debug("DBG:cells.cc:" + msg);
}



/*
cell::cell(){
       	cell(0);
}
*/

cell::cell(coord_t coord) :coord(coord)
{
}

formula_t cell::get_cell_formula()
{ 
	return cell_formula; 
}

void cell::set_omnival(struct value* v)
{
	auto& v1 = v->x;
	switch(v->type) {
		case TYP_FLT:
			omnival = v1.c_n;
			break;	
		case TYP_INT:
			omnival = v1.c_i;
			break;
		case TYP_STR:
			omnival = std::string(v1.c_s);
			break;
		case TYP_ERR:
			omnival = Generic<Err>(v1.c_s);
			break;
		default:
			cerr << "TODO:set_omnival:type:" << v->type << endl;
			assert(false);
	}
	
}


formula_t cell::set_cell_formula(formula_t newval)
{ 
	cell_formula = newval ;  
	return cell_formula; 
}

void cell::reset()
{
	if(cell_formula) free(cell_formula);
	cell_formula = 0;
}

void cell::clear_flags()
{
	//bzero((void*) this->cell_flags, sizeof(this->cell_flags));
	this->cell_flags.clear();
}

cell::~cell()
{
	magic = 0x0DEFACED; // see TR06
	cell::reset();
	//cout <<"X";
}


void copy_cell_stuff (cell* src, cell* dest)
{
	dest->cell_flags = src->cell_flags;
	dest->cell_refs_to = src->cell_refs_to;
	dest->set_cell_formula(src->get_cell_formula());
	dest->cell_cycle = src->cell_cycle;
	dest->set_c_z(src->get_c_z());
}

bool 
vacuous(cell* cp)
{
	return (cp == nullptr) || (cp->get_type() == TYP_NUL);
}

void set_cell_input(CELLREF r, CELLREF c, const std::string& new_input)
{
	//log_debug_1("set_cell_input:r:" + std::to_string(r) + ":c:" + std::to_string(c) + ":new_input:" + new_input);
	new_value(r, c, new_input.c_str());
}

std::string
get_cell_formula_at(int r, int c)
{

	std::string res = decomp_str(r, c);
	return res;
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

void edit_cell(const char* input)
{
	new_value(curow, cucol, input);
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


//////////////////////////////////////////////////////////////////////
// for copying and pasting cells


static std::string m_copied_cell_formula = "";

void
copy_this_cell_formula()
{
	m_copied_cell_formula = decomp_str(curow, cucol);
}

void 
paste_this_cell_formula()
{	
	//edit_cell_at(curow, cucol, m_copied_cell_formula.c_str());
	set_cell_input(curow, cucol, m_copied_cell_formula.c_str());
}

//////////////////////////////////////////////////////////////////////
