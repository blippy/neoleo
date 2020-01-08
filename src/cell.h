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

typedef uint32_t coord_t;
//using coord_t = uint32_t;
#include "neotypes.h"
#include "global.h"
#include "numeric.h"
#include "parser-2019.h"
//#include "sheet.h"
#include "utils.h"
#include "value.h"


typedef std::set<coord_t> crefs_t;


constexpr auto JST_DEF = 0;
#define JST_LFT		1
#define JST_RGT		2
#define JST_CNT		3

#define FMT_DEF		0	/* Default */
#define FMT_HID		1	/* Hidden */
//#define FMT_GPH		2	/* Graph */
#define FMT_DOL		3	/* Dollar */
#define FMT_CMA		4	/* Comma */
#define FMT_PCT		5	/* Percent */
#define FMT_USR		6	/* User defined */
#define FMT_FXT		7
#define FMT_EXP		8
#define FMT_GEN		9
#define	FMT_DATE	10	/* Date */

#define FMT_MAX 15
/* An actual cell structure.  These cannot be variable-length, since they are
   allocated as a variable-length array on a col structure. */

struct cell_flags_s {
	unsigned int	cell_unused:	1;	/* Was 2 */
	unsigned int	cell_lock = 	0;
	unsigned int	cell_justify =	JST_DEF;
	unsigned int	cell_format = 	FMT_DEF;	
	unsigned int	cell_precision:	4;
	void clear() { 
		cell_unused = 0;
		cell_lock = 0;
		cell_justify = 0;
		cell_format = 0;
		cell_precision = 0;
	}
}; 

typedef unsigned char* formula_t;
//typedef std::byte* formula_t;

class cell //: public value
{
	private:
		uint64_t magic = 0x000FF1CE; // class construction check see TR06
		formula_t bytecode = nullptr;
		std::string formula_text;
		crefs_t prec_cells; // the cells to which to formula points // TODO only considers ranges, needs to include cells
		crefs_t dep_cells; // other formula locations pointing to this cell

	public:
		value_t value_2019;
		unsigned short cell_cycle = 0;
		//struct ref_fm *cell_refs_from = nullptr;
		//struct ref_to *cell_refs_to = nullptr;
		num_t to_num();
		ValType get_type();
		bool_t gBol();
		std::string gString();
		err_t  gErr();
		num_t gFlt();
		void update_cell();
		void dump_cell();
		void reparse();

		coord_t coord;
		void set_refs(const crefs_t& coords);
		void set_row(CELLREF r);

		void set_formula_text(const std::string& str);
		std::string get_formula_text() const;
		cell(coord_t coord);
		~cell();
		void reset();
		struct cell_flags_s cell_flags;
		void clear_flags();
		int get_cell_jst() { return cell_flags.cell_justify; }


		void recompute_bytecode();
		void invalidate_bytecode();

		Expr parse_tree;

		formula_t get_bytecode(); 
		void clear_bytecode();
		bool zeroed_1();
		bool locked() const;
		value_t get_value_2019() const;
		void set_value_2019(value_t newval);
		ranges_t predecs; // the ranges that the cell needs to evaluate
		crefs_t deps_2019;
		void erase_predec_deps();
		void insert_predec_deps(coord_t coord);
		void eval_dependents();
		void set_error(const ValErr& ve);
		void set_cyclic();
};

bool vacuous(cell* cp);
std::string get_cell_formula_at(int r, int c);


typedef cell CELL;
typedef cell cell_t;

#define VAR_UNDEF 1
#define VAR_CELL 2
#define VAR_RANGE 3
/* A var is only of this type between calls to start_shift_var and 
 * finish_shift_var 
 */
#define VAR_DANGLING_RANGE 4

typedef struct var
{
	//struct var *var_next;

	short var_flags = VAR_UNDEF;
	struct rng v_rng{0, 0 ,0 ,0};

	/* This is a list of the cells that reference this variable.  If the 
	 * variable changes, all the cells in the vars new range must be given
	 * ref_froms that point to these variables
	 */
	struct ref_fm *var_ref_fm = nullptr;

	/* A variable sized array that holds the var-name. */
	//char var_name[1];
	std::string var_name;
} var_t;


#define	GET_LCK(p)	((p)->cell_flags.cell_lock)
#define SET_LCK(p,x)	((p)->cell_flags.cell_lock = (x))

#define LCK_DEF		0
#define LCK_UNL		1
#define LCK_LCK		2

/* The type of a cell, or of a eval_expression() value */
//#define GET_TYP(p)	((p)->get_type())

#define GET_JST(p)	((p == 0) ? JST_DEF : ((p)->cell_flags.cell_justify))
#define SET_JST(p,x)	((p)->cell_flags.cell_justify = (x))
extern const int base_default_jst;

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
void edit_cell_str (const std::string& new_formula);
void copy_this_cell_formula();
void paste_this_cell_formula();


typedef struct point_t {int r; int c;} point_t;
typedef point_t RC_t;
RC_t ws_extent();
void copy_cell_stuff (cell* src, cell* dest);
std::string formula_text(CELLREF r, CELLREF c);
