#pragma once
/*
 * Copyright (c) 1990-1999, 2001, Free Software Foundation, Inc.
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

/* Various structures and stuff for the spreadsheet */

/* A union of possible values for a location in the spreadsheet
   (or a location to evaluate to:  This includes c_r, which
   a VAR, etc may evaluate to, but which no cell can ever contain */

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <set>
#include <string>
#include <variant>

//typedef uint32_t coord_t;
//using coord_t = uint32_t;
#include "neotypes.h"
#include "parser-2019.h"
//#include "utils.h"


typedef std::set<coord_t> crefs_t;




/* Cell size paramaters. */
inline const unsigned int default_width = 8;
inline unsigned int default_height = 1;
inline unsigned int saved_default_height = 1;




#define FMT_MAX 15
/* An actual cell structure.  These cannot be variable-length, since they are
   allocated as a variable-length array on a col structure. */

struct cell_flags_s {
	unsigned int	cell_unused:	1;	/* Was 2 */
	unsigned int	cell_lock = 	0;
	enum jst		cell_justify =	jst::def;
	unsigned int	cell_format = 	FMT_DEF;	
	unsigned int	cell_precision:	4;	
	bool			bold = false;
	bool			italic = false;
}; 

#define LCK_DEF		0
#define LCK_UNL		1
#define LCK_LCK		2
inline int default_lock = LCK_UNL;



typedef unsigned char* formula_t;

class cell
{
	private:
		uint64_t magic = 0x000FF1CE; // class construction check see TR06
		coord_t coord;
		value_t value_2019;
		std::string formula_text;

	public:
		cell(coord_t coord);
		~cell();

		// coord stuff
		coord_t get_coord() const;
		void set_coord(coord_t coord);
		void set_row(CELLREF r);


		// value stuff
		value_t get_value_2019() const;
		void set_value_2019(value_t newval);
		num_t to_num();

		void set_formula_text(const std::string& str);

		void dump_cell();

		std::string get_formula_text() const;
		struct cell_flags_s cell_flags;
		enum jst get_cell_jst() { return cell_flags.cell_justify; }
		void set_jst(enum jst j);
		void set_prec(int p);
		int width();



		Expr parse_tree;
	
		bool locked() const;
		ranges_t predecs; // the ranges that the cell needs to evaluate
		crefs_t deps_2019;
		void erase_predec_deps();
		void insert_predec_deps(coord_t coord);
		void eval_dependents();
		void set_cyclic();
};

bool vacuous(cell* cp);
std::string get_cell_formula_at(int r, int c);


typedef cell CELL;
typedef cell cell_t;



#define	GET_LCK(p)	((p)->cell_flags.cell_lock)
#define SET_LCK(p,x)	((p)->cell_flags.cell_lock = (x))




/*
 * Actually get/set both Format *and* precision
 */
#define	FMT_MASK	0xF0
#define	PREC_MASK	0x0F
#define	FMT_SHIFT	4

#define GET_FORMAT(p)	((p)->cell_flags.cell_format)
#define SET_FORMAT(p,x)	((p)->cell_flags.cell_format = (x))

#define GET_PRECISION(p)	((p)->cell_flags.cell_precision)
#define SET_PRECISION(p,x)	((p)->cell_flags.cell_precision = (x))

#define FLOAT_PRECISION	11

void set_precision(CELL* cp, int precision);


/* README README README
 *
 * The _make_ functions may cause the addresses of cells previously returned by
 * find_ functions to change.  By extention, any function that calls a make_
 * function can have that effect.  This is particularly nasty because pointers
 * to cells are stored in the global my_cell, and in various stack frames.
 * Several bugs have been traced to this questionable design -- please be
 * careful not to add new ones.
 */

int init_cells_function_count(void);
extern void no_more_cells (void);

void set_cell_input(CELLREF r, CELLREF c, const std::string& new_input);
void edit_cell (const char* input);
void copy_this_cell_formula();
void paste_this_cell_formula();
char * new_value (CELLREF row, CELLREF col, const char *string);



typedef point_t RC_t;
RC_t ws_extent();
std::string formula_text(CELLREF r, CELLREF c);

bool is_nul(CELL* cp);
bool is_flt(CELL* cp);
bool is_range(CELL* cp);

