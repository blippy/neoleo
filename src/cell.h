#pragma once
/*
 * $Id: cell.h,v 1.14 2001/02/13 23:38:05 danny Exp $
 *
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
#include <string>
#include <cstdint>

#include "global.h"
#include "numeric.h"


union vals {
	num_t c_n;
	char *c_s;
	long c_l;
	int c_i;
	struct rng c_r;
};

enum ValType { TYP_NUL=0, // taken to mean 'undefined'
	TYP_FLT=1, TYP_INT=2, TYP_STR=3, TYP_BOL=4, TYP_ERR=5, TYP_RNG=7 };

class value {
	public:
		value();
		~value();
		ValType type = TYP_NUL;
		union vals x;
		long gLong() { assert(type == TYP_INT); return x.c_l; };
		char *gString() { assert(type == TYP_STR); return x.c_s; };
		void sInt(int newval) { type = TYP_INT; x.c_i = newval; };
		void sLong(long newval) { type = TYP_INT; x.c_l = newval; };
		void sString(char* newval) { type = TYP_STR; x.c_s = newval;};
};

/* An actual cell structure.  These cannot be variable-length, since they are
   allocated as a variable-length array on a col structure. */

    struct cell_flags_s {
	unsigned int	cell_unused:	1;	/* Was 2 */
	unsigned int	cell_lock:	2;
	//unsigned int	cell_type:	3;
	ValType		cell_type:	TYP_STR;
	unsigned int	cell_justify:	2;
	unsigned int	cell_format:	4;	/* Was 3 */
	unsigned int	cell_precision:	4;
    }; 
class cell
{
	private:
		union vals c_z;
		unsigned char *cell_formula = nullptr;
		uint64_t magic = 0x000FF1CE; // class construction check see TR06

	public:
		cell();
		~cell();
		/* char *cell_string; */
		struct cell_flags_s cell_flags;
		//ValType cell_type = TYP_STR;
		ValType get_cell_type() { return cell_flags.cell_type;}
		void set_cell_type(ValType t) { cell_flags.cell_type = t;}
		int get_cell_jst() { return cell_flags.cell_justify; }
		unsigned short cell_cycle = 0;
		//struct font_memo *cell_font;
		struct ref_fm *cell_refs_from = nullptr;
		struct ref_to *cell_refs_to = nullptr;

		unsigned char * get_cell_formula() { return cell_formula; } ; 
		unsigned char * set_cell_formula( unsigned char * newval) { cell_formula = newval ;  return cell_formula; } ; 


		void sInt(int newval); // set integer value
		char * cell_str() { 
			//assert(magic == 0x000FF1CE);
			assert(get_cell_type() == TYP_STR); return c_z.c_s ;};
		char * get_cell_str() { 
			//assert(magic == 0x000FF1CE);
			return cell_str();};
		void set_cell_str(char* newval) { c_z.c_s = newval;};
		long cell_int() { assert(get_cell_type() == TYP_INT); return c_z.c_l ;};
		long get_cell_int() { 
			//assert(magic == 0x000FF1CE);
			return cell_int();
		};
		void set_cell_int(long newval) { c_z.c_l = newval; } ; 
		int cell_err() { return c_z.c_i ;};
		void set_cell_err(int newval) { c_z.c_i = newval ;};
		num_t cell_flt() { return c_z.c_n ;};
		num_t get_cell_flt() { return c_z.c_n ;};
		void set_cell_flt(num_t newval) { c_z.c_n = newval; };
		int cell_bol() { return c_z.c_i ;};
		void set_cell_bol(int newval) { c_z.c_i = newval; };
		//void set_cell_bol(int newval) { c_z.c_i = newval;};
		vals get_c_z() { return c_z; }; // ugly compilation hack. TODO eliminate
		void set_c_z(vals newval) { c_z = newval; } ; // TODO more ugly hackery
};

bool vacuous(cell* cp);
std::string get_cell_formula_at(int r, int c);

struct var
  {
    struct var *var_next;

    short var_flags;
    struct rng v_rng;

    /* This is a list of the cells that reference this variable.  If the 
     * variable changes, all the cells in the vars new range must be given
     * ref_froms that point to these variables
     */
    struct ref_fm *var_ref_fm;

    /* A variable sized array that holds the var-name. */
    char var_name[1];
  };

//typedef struct cell CELL;
typedef cell CELL;

#define VAR_UNDEF 1
#define VAR_CELL 2
#define VAR_RANGE 3
/* A var is only of this type between calls to start_shift_var and 
 * finish_shift_var 
 */
#define VAR_DANGLING_RANGE 4

/* Shorthand for the cell union */
//#define cell_flt	c_z.c_n
//#define cell_str	c_z.c_s
//#define cell_int	c_z.c_l
//#define cell_bol	c_z.c_i
//#define cell_err	c_z.c_i

#define	GET_LCK(p)	((p)->cell_flags.cell_lock)
#define SET_LCK(p,x)	((p)->cell_flags.cell_lock = (x))

#define LCK_DEF		0
#define LCK_UNL		1
#define LCK_LCK		2

/* The type of a cell, or of a eval_expression() value */
#define GET_TYP(p)	((p)->get_cell_type())
#define SET_TYP(p,x)	((p)->set_cell_type(x))

#define GET_JST(p)	((p == 0) ? JST_DEF : ((p)->cell_flags.cell_justify))
#define SET_JST(p,x)	((p)->cell_flags.cell_justify = (x))
#define JST_DEF		0
#define JST_LFT		1
#define JST_RGT		2
#define JST_CNT		3
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

void edit_cell_at(CELLREF row, CELLREF col, std::string new_formula);
extern void edit_cell_at(CELLREF row, CELLREF col, const char* new_formula);
void copy_this_cell_formula();
void paste_this_cell_formula();


typedef struct point_t {int r; int c;} point_t;
typedef point_t RC_t;
