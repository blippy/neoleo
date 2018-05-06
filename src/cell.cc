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

#define Float	x.c_n
#define String	x.c_s
#define Int	x.c_l
#define Value	x.c_i
#define Rng	x.c_r

static void log_debug_1(std::string msg)
{
	if constexpr(true)
		log_debug("DBG:cells.cc:" + msg);
}


#define ERROR(x)	\
 {			\
	p->Value=x;	\
	p->type=TYP_ERR;\
	return;		\
 }

/*
cell::cell(){
       	cell(0);
}
*/

cell::cell(coord_t coord) :coord(coord)
{
}

unsigned char * cell::get_cell_formula()
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


unsigned char * cell::set_cell_formula( unsigned char * newval)
{ 
	cell_formula = newval ;  
	return cell_formula; 
}

void cell::reset()
{
	if(cell_formula) free(cell_formula);
	cell_formula = 0;
}

cell::~cell()
{
	magic = 0x0DEFACED; // see TR06
	cell::reset();
	//cout <<"X";
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







static void
do_hlookup (struct value *p)
{
	struct rng *rng = &((p)->Rng);
	num_t fltval = (p + 1)->Float;
	long offset = (p + 2)->Int;

	CELL *cell_ptr;
	num_t f;
	CELLREF col;
	CELLREF row;
	char *strptr;

	row = rng->lr;
	for (col = rng->lc; col <= rng->hc; col++)
	{
		if (!(cell_ptr = find_cell (row, col)))
			ERROR (NON_NUMBER);
		switch (GET_TYP (cell_ptr))
		{
			case TYP_FLT:
				if (fltval < cell_ptr->gFlt())
					goto out;
				break;
			case TYP_INT:
				if (fltval < cell_ptr->gInt())
					goto out;
				break;
			case TYP_STR:
				strptr = cell_ptr->gString();
				f = astof (&strptr);
				if (!*strptr && fltval > f)
					goto out;
				else
					ERROR (NON_NUMBER);
			case 0:
			case TYP_BOL:
			case TYP_ERR:
			default:
				ERROR (NON_NUMBER);
		}
	}
out:
	if (col == rng->lc)
		ERROR (OUT_OF_RANGE);
	--col;
	row = rng->lr + offset;
	if (row > rng->hr)
		ERROR (OUT_OF_RANGE);
	cell_ptr = find_cell (row, col);
	if (!cell_ptr)
	{
		p->type = TYP_NUL;
		p->Int = 0;
	}
	else
	{
		p->type = GET_TYP (cell_ptr);
		p->x = cell_ptr->get_c_z();
	}
}

static void
do_vlookup (struct value *p)
{
	struct rng *rng = &((p)->Rng);
	num_t fltval = (p + 1)->Float;
	long offset = (p + 2)->Int;

	CELL *cell_ptr;
	num_t f;
	CELLREF col;
	CELLREF row;
	char *strptr;

	col = rng->lc;
	for (row = rng->lr; row <= rng->hr; row++)
	{
		if (!(cell_ptr = find_cell (row, col)))
			ERROR (NON_NUMBER);
		switch (GET_TYP (cell_ptr))
		{
			case TYP_FLT:
				if (fltval < cell_ptr->gFlt())
					goto out;
				break;
			case TYP_INT:
				if (fltval < cell_ptr->gInt())
					goto out;
				break;
			case TYP_STR:
				strptr = cell_ptr->gString();
				f = astof (&strptr);
				if (!*strptr && fltval > f)
					goto out;
				else
					ERROR (NON_NUMBER);
			case 0:
			case TYP_BOL:
			case TYP_ERR:
			default:
				ERROR (NON_NUMBER);
		}
	}
out:
	if (row == rng->lr)
		ERROR (OUT_OF_RANGE);
	--row;
	col = rng->lc + offset;
	if (col > rng->hc)
		ERROR (OUT_OF_RANGE);

	cell_ptr = find_cell (row, col);
	if (!cell_ptr)
	{
		p->type = TYP_NUL;
		p->Int = 0;
	}
	else
	{
		p->type = GET_TYP (cell_ptr);
		p->x = cell_ptr->get_c_z();
	}
}

static void
do_vlookup_str (struct value *p)
{
	struct rng *rng = &((p)->Rng);
	char * key = (p + 1)->String;
	long offset = (p + 2)->Int;

	CELL *cell_ptr;
	CELLREF col;
	CELLREF row;

	col = rng->lc;
	for (row = rng->lr; row <= rng->hr; row++)
	{
		if (!(cell_ptr = find_cell (row, col)))
			ERROR (NON_NUMBER);
		switch (GET_TYP (cell_ptr))
		{
			case TYP_STR:
				if (!strcmp (key, cell_ptr->gString()))
					goto out;
				break;
			case 0:
			case TYP_FLT:
			case TYP_INT:
			case TYP_BOL:
			case TYP_ERR:
			default:
				ERROR (NON_NUMBER);
		}
	}
out:
	if (row > rng->hr)
		ERROR (OUT_OF_RANGE);
	col = rng->lc + offset;
	if (col > rng->hc)
		ERROR (OUT_OF_RANGE);

	cell_ptr = find_cell (row, col);
	if (!cell_ptr)
	{
		p->type = TYP_NUL;
		p->Int = 0;
	}
	else
	{
		p->type = GET_TYP (cell_ptr);
		p->x = cell_ptr->get_c_z();
	}
}


#define S (char *)
#define T (void (*)())
struct function cells_funs[] =
{
	//{C_FN1 | C_T, X_A1, "S", T do_curcell, S "curcell"},
  //{C_FN1 | C_T, X_A1, "S", T do_my, S "my"},
  //{C_FN3 | C_T, X_A3, "IIS", T do_cell, S "cell"},
  //{C_FN3 | C_T, X_A3, "ISS", T do_varval, S "varval"},

  //{C_FN2, X_A2, "RA", T do_member, S "member"},
  //{C_FN2, X_A2, "RS", T do_smember, S "smember"},
  //{C_FN2, X_A2, "RS", T do_members, S "members"},
  //{C_FN2, X_A2, "RS", T do_pmember, S "pmember"},
  //{C_FN2, X_A2, "RS", T do_memberp, S "memberp"},

  {C_FN3, X_A3, "RFI", T do_hlookup, S "hlookup"},
  {C_FN3, X_A3, "RFI", T do_vlookup, S "vlookup"},
  {C_FN3, X_A3, "RSI", T do_vlookup_str, S "vlookup_str"},


  {0, 0, "", 0, 0},
};

int init_cells_function_count(void) 
{
        return sizeof(cells_funs) / sizeof(struct function) - 1;
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
