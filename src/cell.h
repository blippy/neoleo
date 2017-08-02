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

#ifdef __cplusplus
//extern "C" {
#endif

#include "global.h"
#include "numeric.h"
//#include "font.h"

union vals
  {
	  num_t c_n;
	  //double c_d;
	  char *c_s;
	  long c_l;
	  int c_i;
	  struct rng c_r;
  };

struct value
  {
	  int type;
	  union vals x;
  };

/* An actual cell structure.  These cannot be variable-length, since they are
   allocated as a variable-length array on a col structure. */

    struct cell_flags_s {
	unsigned int	cell_unused:	1;	/* Was 2 */
	unsigned int	cell_lock:	2;
	unsigned int	cell_type:	3;
	unsigned int	cell_justify:	2;
	unsigned int	cell_format:	4;	/* Was 3 */
	unsigned int	cell_precision:	4;
    }; 
struct cell
  {
    /* char *cell_string; */
    struct cell_flags_s cell_flags;
    unsigned short cell_cycle;
    //struct font_memo *cell_font;
    struct ref_fm *cell_refs_from;
    struct ref_to *cell_refs_to;
    unsigned char *cell_formula;
    union vals c_z;
  };

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

typedef struct cell CELL;

#define VAR_UNDEF 1
#define VAR_CELL 2
#define VAR_RANGE 3
/* A var is only of this type between calls to start_shift_var and 
 * finish_shift_var 
 */
#define VAR_DANGLING_RANGE 4

/* Shorthand for the cell union */
//#define cell_flt	c_z.c_d
#define cell_flt	c_z.c_n
#define cell_str	c_z.c_s
#define cell_int	c_z.c_l
#define cell_bol	c_z.c_i
#define cell_err	c_z.c_i

#define	GET_LCK(p)	((p)->cell_flags.cell_lock)
#define SET_LCK(p,x)	((p)->cell_flags.cell_lock = (x))

#define LCK_DEF		0
#define LCK_UNL		1
#define LCK_LCK		2

/* The type of a cell, or of a eval_expression() value */
#define GET_TYP(p)	((p)->cell_flags.cell_type)
#define SET_TYP(p,x)	((p)->cell_flags.cell_type = (x))

#define TYP_FLT		1	/* Float */
#define TYP_INT		2	/* Integer */
#define TYP_STR		3	/* String */
#define TYP_BOL		4	/* Boolean */
#define TYP_ERR		5	/* Error */

#define TYP_RNG		7	/* This for the expression evaluator:
				   NO cell should be this type */

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

extern void edit_cell_at(CELLREF row, CELLREF col, const char* new_formula);
void copy_cell_formula();
void paste_cell_formula();

#ifdef __cplusplus
//}
#endif
