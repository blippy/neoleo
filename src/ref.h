#ifndef REFH
#define REFH
/*
 * $Id: ref.h,v 1.6 2000/08/10 21:02:51 danny Exp $
 * Copyright © 1992, 1993 Free Software Foundation, Inc.
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
#include "global.h"
#include "cell.h"

/* Operations that add or remove cells to the sparse array
 * may cause the (memory) addresses of other cells to change.
 * Some cell pointers are privileged enough to be relocated.
 * This is one.  The others are part of a stack of iteration 
 * states created by nested calls to find_range etc.
 *
 * When this pointer is not NULL, the variables cur_row and cur_col
 * MUST contain the (spreadsheet) address of my_cell.
 */
extern struct cell * my_cell;

extern void set_cell (CELLREF row, CELLREF col, char *string);
extern char * new_value (CELLREF row, CELLREF col, char *string);
extern char * quote_new_value (CELLREF row, CELLREF col, char *string);
extern char * set_new_value (CELLREF row, CELLREF col, int type, union vals *value);
extern char * read_new_value (CELLREF row, CELLREF col, char *form, char *val);
extern void move_cell (CELLREF rf, CELLREF cf, CELLREF rt, CELLREF ct);
extern void copy_cell (CELLREF rf, CELLREF cf, CELLREF rt, CELLREF ct);
extern void flush_old_value (void);
extern void add_ref (CELLREF row, CELLREF col);
extern void add_range_ref (struct rng *rng);
extern void flush_refs (void);
extern void add_ref_to (int whereto);
extern void add_ref_to (int whereto);
extern void shift_outside (struct rng *fm, int dn, int ov);
extern void shift_formula (int r, int c, int dn, int ov);
extern void cell_alarm (void);
extern void flush_all_timers (void);
extern void add_timer_ref (int whereto);
extern void init_refs (void);
extern void push_refs (struct ref_fm *ref);
extern void push_cell (CELLREF row, CELLREF col);
extern int eval_next_cell (void);
extern char * old_new_var_value (char *v_name, int v_namelen, char *v_newval);
extern char * new_var_value (char *v_name, int v_namelen, struct rng *tmp_rng);
extern void for_all_vars (void (*func) (char *, struct var *));
extern struct var * find_or_make_var (char *string, int len);
extern struct var * find_var (char *string, int len);
extern void add_var_ref (void * vvar);
extern void flush_variables (void);

#endif
