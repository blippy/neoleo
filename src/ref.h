#pragma once
/*
 * $Id: ref.h,v 1.6 2000/08/10 21:02:51 danny Exp $
 * Copyright � 1992, 1993 Free Software Foundation, Inc.
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

#include <string>
#include "global.h"
#include "cell.h"

struct var * find_or_make_var(const char *string, int len);

/* Operations that add or remove cells to the sparse array
 * may cause the (memory) addresses of other cells to change.
 * Some cell pointers are privileged enough to be relocated.
 * This is one.  The others are part of a stack of iteration 
 * states created by nested calls to find_range etc.
 *
 * When this pointer is not NULL, the variables cur_row and cur_col
 * MUST contain the (spreadsheet) address of my_cell.
 */
extern cell * my_cell;

extern void set_cell(CELLREF row, CELLREF col, const std::string& in_string);
extern char * new_value (CELLREF row, CELLREF col, const char *string);
extern void move_cell (CELLREF rf, CELLREF cf, CELLREF rt, CELLREF ct);
extern void copy_cell (CELLREF rf, CELLREF cf, CELLREF rt, CELLREF ct);
extern void flush_old_value (void);
extern void add_ref (CELLREF row, CELLREF col);
extern void add_range_ref (struct rng *rng);
extern void flush_refs (void);
extern void add_ref_to (cell* cp, int whereto);
extern void shift_outside (struct rng *fm, int dn, int ov);
extern void shift_formula (int r, int c, int dn, int ov);
extern void cell_alarm (void);
extern void flush_all_timers (void);
extern void add_timer_ref (cell* cp, int whereto);
extern void init_refs (void);
extern void push_refs (struct ref_fm *ref);
extern void push_cell (CELLREF row, CELLREF col);
void push_cell(cell* cp);
extern int eval_next_cell (void);
extern char * old_new_var_value (char *v_name, int v_namelen, char *v_newval);
extern char * new_var_value (char *v_name, int v_namelen, struct rng *tmp_rng);
void for_all_vars (void (*func) (const char *, struct var *));
extern struct var * find_var (char *string, int len);
extern void add_var_ref (void * vvar);
extern void flush_variables (void);
void push_refs (cell *cp);
