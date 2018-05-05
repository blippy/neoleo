#pragma once
/*
 * Copyright © 1990-1999, 2001, Free Software Foundation, Inc.
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

#include <assert.h>
#include <cstdint>
#include <string>
#include <variant>

#include "global.h"
#include "numeric.h"
#include "utils.h"
#include "value.h"



template<typename T>
struct Generic { 
	Generic(const std::string& s) : s(s) {}
	std::string s; 
};

struct Err {};

using omnival_t = std::variant<std::string, num_t, Generic<Err>>;


constexpr auto JST_DEF = 0;
#define JST_LFT		1
#define JST_RGT		2
#define JST_CNT		3

#define FMT_DEF		0	/* Default */
#define FMT_HID		1	/* Hidden */
#define FMT_GPH		2	/* Graph */
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
	unsigned int	cell_lock:	2;
	unsigned int	cell_justify =	JST_DEF;
	unsigned int	cell_format = 	FMT_DEF;	
	unsigned int	cell_precision:	4;
}; 

class cell : public value
{
	private:
		unsigned char *cell_formula = nullptr; // (unsigned char*) dupe("");
		uint64_t magic = 0x000FF1CE; // class construction check see TR06
		omnival_t omnival;

	public:
		unsigned short cell_cycle = 0;
		struct ref_fm *cell_refs_from = nullptr;
		struct ref_to *cell_refs_to = nullptr;

		cell();
		~cell();
		void reset();
		struct cell_flags_s cell_flags;
		int get_cell_jst() { return cell_flags.cell_justify; }

		unsigned char* get_cell_formula(); 
		unsigned char* set_cell_formula( unsigned char * newval);
		void set_omnival(struct value* v);



		/* mcarter 02-May-2018 issue#37
		 * This is a potential source of bugs, because set_c_z() does not
		 * set the type as well. So you might be mixing up types. I encountered
		 * this when I juiced up class cell to derive from class value, and
		 * tried the copy-region function. set_c_z() is used in other places,
		 * so there are likely to be other bugs lurking in the code
		 */
		union vals get_c_z() { return x; }; // ugly compilation hack. TODO eliminate
		void set_c_z(union vals newval) { x = newval; } ; // TODO more ugly hackery
};

bool vacuous(cell* cp);
std::string get_cell_formula_at(int r, int c);


typedef cell CELL;

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
#define GET_TYP(p)	((p)->get_type())
#define SET_TYP(p,x)	((p)->set_type(x))

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

//void edit_cell_at(CELLREF row, CELLREF col, std::string new_formula);
//extern void edit_cell_at(CELLREF row, CELLREF col, const char* new_formula);
void set_cell_input(CELLREF r, CELLREF c, const std::string& new_input);
void edit_cell(const char* input);
void copy_this_cell_formula();
void paste_this_cell_formula();


typedef struct point_t {int r; int c;} point_t;
typedef point_t RC_t;
RC_t ws_extent();
