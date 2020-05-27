/*
 * $Id: ref.c,v 1.15 2001/02/13 23:38:06 danny Exp $
 *
 * Copyright (c) 1990, 1992, 1993, 2001 Free Software Foundation, Inc.
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

#include <stdlib.h>

#include <iostream>
#include <map>
#include <math.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>


#include "global.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "ref.h"
#include "cmd.h"
#include "sheet.h"
#include "logging.h"

using std::cout;
using std::endl;
//using std::vector;

static void add_ref_fm (struct ref_fm **where, CELLREF r, CELLREF c);
static void flush_ref_fm (struct ref_fm **, CELLREF, CELLREF);
static void flush_range_ref (struct rng *, CELLREF, CELLREF);
extern void shift_formula (int r, int c, int dn, int ov);
static void flush_ref_to (struct ref_to **);
static void flush_fm_ref (struct ref_fm *);

/* More tunable paramaters */

#define FIFO_START	40
#define FIFO_INC	*=2

#define TO_MAGIC(row,col)	(((long)(row)<<BITS_PER_CELLREF)|(col))
#define MAGIC_ROW(magic)	(((magic)>>BITS_PER_CELLREF)&CELLREF_MASK)
#define MAGIC_COL(magic)	((magic)&CELLREF_MASK)

#define BETWEEN(mid,lo,hi)	((mid>=lo)&&(mid<=hi))

static VOIDSTAR moving;

int timer_active = 0;
struct ref_fm *timer_cells;

CELL *my_cell;

#ifdef TEST
extern int debug;
#endif

/* Functions for dealing exclusively with variables */
std::map<std::string, struct var>the_vars_1;


/* For the fifo-buffer */
struct pos {
	CELLREF row;
	CELLREF col;
};

struct cell_buf {
	unsigned int size;
	struct pos *buf;
	struct pos *push_to_here;
	struct pos *pop_frm_here;
};


/* Set the cell ROW,COL to STRING, parsing string as needed */
	void
set_cell (CELLREF row, CELLREF col, const std::string& in_string)
{
	unsigned char *ret;

	cur_row = row;
	cur_col = col;

	std::string s2{in_string};
	while(s2.size() > 0 && s2[0] == ' ') s2.erase(0, 1);

	my_cell = find_cell (cur_row, cur_col);
	my_cell = find_or_make_cell(cur_row, cur_col);

}

extern int default_lock;

/* new_value() calls set_cell, but refuses to change locked cells, and
   updates and prints the results.  It returns an error msg on error. . .
   */
	char *
new_value (CELLREF row, CELLREF col, const char *string)
{
	CELL *cp;

	cp = find_cell (row, col);
	if (((!cp || GET_LCK (cp) == LCK_DEF) && default_lock == LCK_LCK) || (cp && GET_LCK (cp) == LCK_LCK))
	{
		return (char *) "cell is locked";
	}

	set_cell(row, col, string);
	if (my_cell)
	{
		my_cell->update_cell();
		io_pr_cell (row, col, my_cell);
		my_cell = 0;
	}
	Global->modified = 1;
	return 0;
}







/* Used only in regions.c for copy_region. */
	void
copy_cell (CELLREF rf, CELLREF cf, CELLREF rt, CELLREF ct)
{
	CELL *cpf = find_cell (rf, cf);
	cur_row = rt;
	cur_col = ct;
	my_cell = find_cell (cur_row, cur_col);
	if(!cpf) return;
	if (!my_cell)
	{
		my_cell = find_or_make_cell (cur_row, cur_col);
		cpf = find_cell (rf, cf);	/* FOO */
	}

	if (!cpf)
		return;

	my_cell->cell_flags = cpf->cell_flags;



	cpf->set_formula_text(my_cell->get_formula_text());

	io_pr_cell (cur_row, cur_col, my_cell);

	push_refs(my_cell);
	my_cell = 0;
}


/* --------- Routines for dealing with cell references to other cells ------ */


/* Record in the argument cell that cur_row/col depends on it. */

	void
add_ref (CELLREF row, CELLREF col)
{
	CELL *other_cell = find_or_make_cell (row, col);
}

/* like add_ref, except over a range of arguments and with memory
 * management weirdness. 
 */
void add_range_ref (struct rng *rng)
{

	make_cells_in_range (rng);

}

	static void
flush_range_ref (struct rng *rng, CELLREF rr, CELLREF cc)
{
}

#define FM_HASH_NUM 503
#define TO_HASH_NUM 29
#ifdef TEST
static int fm_misses = 0;
static int to_misses = 0;
#endif

static struct ref_fm *fm_list[FM_HASH_NUM];
static struct ref_fm *fm_tmp_ref;
static unsigned fm_tmp_ref_alloc;

static struct ref_to *to_list[TO_HASH_NUM];
static struct ref_to *to_tmp_ref;
static unsigned to_tmp_ref_alloc;

	void
flush_refs (void)
{
	int n;
	struct ref_fm *ftmp, *oftmp;
	struct ref_to *ttmp, *ottmp;

	for (n = 0; n < FM_HASH_NUM; n++)
	{
		for (ftmp = fm_list[n]; ftmp; ftmp = oftmp)
		{
			oftmp = ftmp->refs_next;
			free (ftmp);
		}
		fm_list[n] = 0;
	}
	for (n = 0; n < TO_HASH_NUM; n++)
	{
		for (ttmp = to_list[n]; ttmp; ttmp = ottmp)
		{
			ottmp = ttmp->refs_next;
			free (ttmp);
		}
		to_list[n] = 0;
	}
}

	static struct ref_fm *
find_fm_ref (void)
{
	struct ref_fm *tmp;
	int n;
	unsigned long hash;

#if 1
	for (hash = 0, n = 0; n < fm_tmp_ref->refs_used; n++)
	{
		hash += (n + 1) * (((fm_tmp_ref->fm_refs[n].ref_row) << BITS_PER_CELLREF) +
				fm_tmp_ref->fm_refs[n].ref_col);
	}
	hash %= FM_HASH_NUM;
#else
	hash = fm_tmp_ref->refs_used;
#endif
	for (tmp = fm_list[hash]; tmp; tmp = tmp->refs_next)
	{
		if (tmp->refs_used != fm_tmp_ref->refs_used)
			continue;
		if (!bcmp (tmp->fm_refs, fm_tmp_ref->fm_refs, fm_tmp_ref->refs_used * sizeof (struct ref_array)))
		{
			tmp->refs_refcnt++;
			return tmp;
		}
#ifdef TEST
		else
			fm_misses++;
#endif
	}

	tmp = (ref_fm*) ck_malloc (sizeof (struct ref_fm) + (fm_tmp_ref->refs_used - 1) * sizeof (struct ref_array));
	tmp->refs_next = fm_list[hash];
	fm_list[hash] = tmp;
	tmp->refs_refcnt = 1;
	tmp->refs_used = fm_tmp_ref->refs_used;
	if (tmp->refs_used)
		bcopy (fm_tmp_ref->fm_refs, tmp->fm_refs,
				tmp->refs_used * sizeof (struct ref_array));

	return tmp;
}

	static void 
flush_fm_ref (struct ref_fm *old)
{
	struct ref_fm *tmp;
	int n;
	unsigned long hash;

	--(old->refs_refcnt);

#ifdef DEFER_FREE
	return;
#endif
	if (!old->refs_refcnt)
	{
#if 1
		for (hash = 0, n = 0; n < old->refs_used; n++)
		{
			hash += (n + 1) * (((old->fm_refs[n].ref_row) << BITS_PER_CELLREF) +
					old->fm_refs[n].ref_col);
		}
		hash %= FM_HASH_NUM;
#else
		hash = old->refs_used;
#endif
		if (fm_list[hash] == old)
			fm_list[hash] = old->refs_next;
		else
		{
			for (tmp = fm_list[hash];
					tmp && tmp->refs_next != old;
					tmp = tmp->refs_next)
				;
#ifdef TEST
			if (!tmp)
			{
				io_error_msg ("Old not in refs_list in flush_fm_ref(%p)", old);
				return;
			}
#endif
			if (tmp)
				tmp->refs_next = old->refs_next;
		}
		free (old);
	}
}

/* This adds a from reference to a cells reference list.
 * Note that the ref_fm structures themselves are hash-consed.
 */
	static void
add_ref_fm (struct ref_fm **where, CELLREF r, CELLREF c)
{
	struct ref_fm *from;
	int n;

	from = *where;
	if (!from)
	{
		if (!fm_tmp_ref)
		{
			fm_tmp_ref = (ref_fm*) ck_malloc (sizeof (struct ref_fm));
			fm_tmp_ref_alloc = 1;
		}
		fm_tmp_ref->refs_used = 1;
		fm_tmp_ref->fm_refs[0].ref_row = r;
		fm_tmp_ref->fm_refs[0].ref_col = c;
	}
	else
	{
		if (fm_tmp_ref_alloc <= from->refs_used)
		{
			size_t size = sizeof (struct ref_fm)
				+ (size_t)(from->refs_used) * sizeof (struct ref_array);
			fm_tmp_ref =   (ref_fm *)ck_realloc (fm_tmp_ref, size);
			fm_tmp_ref_alloc = from->refs_used + 1;
		}
		fm_tmp_ref->refs_used = from->refs_used + 1;
		n = 0;
		while (n < from->refs_used
				&& (from->fm_refs[n].ref_row < r
					|| (from->fm_refs[n].ref_row == r && from->fm_refs[n].ref_col <= c)))
		{
			fm_tmp_ref->fm_refs[n] = from->fm_refs[n];
			n++;
		}
		fm_tmp_ref->fm_refs[n].ref_row = r;
		fm_tmp_ref->fm_refs[n].ref_col = c;
		while (n < from->refs_used)
		{
			fm_tmp_ref->fm_refs[n + 1] = from->fm_refs[n];
			n++;
		}
	}
	*where = find_fm_ref ();
	if (from)
		flush_fm_ref (from);
}

	static void
flush_ref_fm (struct ref_fm **where, CELLREF r, CELLREF c)
{
	struct ref_fm *from;
	int n;

	from = *where;
#ifdef TEST
	if (!from)
	{
		io_error_msg ("No refs in flush_ref_fm(%p,%u,%u)", where, r, c);
		return;
	}
#endif
	if (!from)
		return;
	if (from->refs_used == 1)
	{
		*where = 0;
		flush_fm_ref (from);
		return;
	}
	fm_tmp_ref->refs_used = from->refs_used - 1;
	n = 0;
	while (n < from->refs_used
			&& (from->fm_refs[n].ref_row < r
				|| (from->fm_refs[n].ref_row == r && from->fm_refs[n].ref_col < c)))
	{
		fm_tmp_ref->fm_refs[n] = from->fm_refs[n];
		n++;
	}
#ifdef TEST
	if (n == from->refs_used)
	{
		io_error_msg ("No refs from %u,%u in %p in flush_refs_fm", r, c, where);
		return;
	}
#endif
	while (n < fm_tmp_ref->refs_used)
	{
		fm_tmp_ref->fm_refs[n] = from->fm_refs[n + 1];
		n++;
	}
	*where = find_fm_ref ();
	flush_fm_ref (from);
}


	static struct ref_to *
find_to_ref (void)
{
	struct ref_to *tmp;
	int n;
	unsigned long hash;

	/* io_error_msg("find_to_ref %u %u",to_tmp_ref->refs_used,to_tmp_ref->to_refs[0]); */
#if 1
	for (hash = 0, n = 0; n < to_tmp_ref->refs_used; n++)
		hash += (n + 1) * to_tmp_ref->to_refs[n];

	hash %= TO_HASH_NUM;
#else
	hash = to_tmp_ref->refs_used;
#endif
	for (tmp = to_list[hash]; tmp; tmp = tmp->refs_next)
	{
		/* io_error_msg("%p(%u)->%p  %u %u",tmp,tmp->refs_refcnt,
		   tmp->refs_next,tmp->refs_used,tmp->to_refs[0]); */
		if (tmp->refs_used != to_tmp_ref->refs_used)
			continue;
		if (!bcmp (tmp->to_refs, to_tmp_ref->to_refs, to_tmp_ref->refs_used))
		{
			/* io_error_msg("Hit!"); */
			tmp->refs_refcnt++;
			return tmp;
		}
#ifdef TEST
		else
			to_misses++;
#endif
	}

	/* io_error_msg("Miss. .."); */
	tmp = (ref_to*) ck_malloc (sizeof (struct ref_to) + to_tmp_ref->refs_used - 1);
	tmp->refs_next = to_list[hash];
	to_list[hash] = tmp;
	tmp->refs_refcnt = 1;
	tmp->refs_used = to_tmp_ref->refs_used;
	if (tmp->refs_used)
		bcopy (to_tmp_ref->to_refs, tmp->to_refs, tmp->refs_used);

	return tmp;
}

	void
add_ref_to (cell* cp, int whereto)
{
}

	static void
flush_ref_to (struct ref_to **where)
{
	struct ref_to *tmp;
	struct ref_to *old;
	int n;
	unsigned long hash;

#ifdef TEST
	if (!where || !*where)
	{
		io_error_msg ("null flush_ref_to(%p)", where);
		return;
	}
#endif
	old = *where;
	*where = 0;
	--(old->refs_refcnt);

#ifdef DEFER_FREE
	return;
#endif
	if (!old->refs_refcnt)
	{
#if 1
		for (hash = 0, n = 0; n < old->refs_used; n++)
			hash += (n + 1) * old->to_refs[n];

		hash %= TO_HASH_NUM;
#else
		hash = old->refs_used;
#endif
		if (to_list[hash] == old)
			to_list[hash] = old->refs_next;
		else
		{
			for (tmp = to_list[hash]; tmp && tmp->refs_next != old; tmp = tmp->refs_next)
				;
#ifdef TEST
			if (!tmp)
			{
				io_error_msg ("Old not in refs_list in flush_to_ref(%p)", old);
				return;
			}
#endif
			tmp->refs_next = old->refs_next;
		}
		free (old);
	}
}


/* ------------- Routines for dealing with moving cells -------------------- */

static struct rng *shift_fm;
static int shift_ov;
static int shift_dn;

/* This removes all the CELL_REF_FM links associated with a 
 * variable, and adjusts the variables value.
 * After calling this function, one must also call 
 * finish_shift_var to install the new CELL_REF_FM links.
 */
	static void 
start_shift_var (const char *name, struct var *v)
{
	int n;
	int nn;


	n = (BETWEEN (v->v_rng.hc, shift_fm->lc, shift_fm->hc) << 3)
		+ (BETWEEN (v->v_rng.lc, shift_fm->lc, shift_fm->hc) << 2)
		+ (BETWEEN (v->v_rng.hr, shift_fm->lr, shift_fm->hr) << 1)
		+ BETWEEN (v->v_rng.lr, shift_fm->lr, shift_fm->hr);
	switch (n)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 8:
		case 12:
			/* Null intersection, ignore it */
			break;

		case 5:			/* The bottom and right */
		case 6:			/* The bottom and left */
		case 9:			/* The top and right */
		case 10:			/* The top and left */
			/* The var sticks out of the range we're moving */
			/* on two sides.  what should we do? */
			io_error_msg ("'%s' can't be adjusted", v->var_name);
			break;

		case 7:			/* v->hc sticks out the right */
		case 11:			/* v->lc sticks out the left */
		case 13:			/* v->hr sticks out the bottom */
		case 14:			/* v->lr sticks out the top */
			/* It only sticks out on one side.  We can
			   (try to) adjust it */

		case 15:			/* var is completely inside the range */
			if (v->var_ref_fm)
			{
				for (nn = 0; nn < v->var_ref_fm->refs_used; nn++)
				{
					flush_range_ref (&(v->v_rng),
							v->var_ref_fm->fm_refs[nn].ref_row,
							v->var_ref_fm->fm_refs[nn].ref_col);
				}
			}
			if (n != 7)
				v->v_rng.hc += shift_ov;
			if (n != 11)
				v->v_rng.lc += shift_ov;
			if (n != 13)
				v->v_rng.hr += shift_dn;
			if (n != 14)
				v->v_rng.lr += shift_dn;
			v->var_flags = VAR_DANGLING_RANGE;
	}
}


	static void 
finish_shift_var (const char *name, struct var *v)
{
	int n;
	if (v->var_flags != VAR_DANGLING_RANGE)
		return;

	v->var_flags = VAR_RANGE;

	if (!v->var_ref_fm)
		return;

	for (n = 0; n < v->var_ref_fm->refs_used; n++)
	{
		cur_row = v->var_ref_fm->fm_refs[n].ref_row;
		cur_col = v->var_ref_fm->fm_refs[n].ref_col;
		add_range_ref (&(v->v_rng));
	}
}

#define RIGHT	8
#define LEFT	4
#define BOTTOM	2
#define TOP	1

/*
 * This iterates over the region FM, preparing the cells there to be shifted
 * OV(er) and D(ow)N.
 *
 * After this, the ref_fm and ref_to lists of a cell within the region should
 * be appropriate to the location that cell will be shifted to.
 * 
 * Variables and references to variables are also shifted.
 */
	void
shift_outside (struct rng *fm, int dn, int ov)
{
}

/* The formula in cell my_cell has moved by DN down and OV over, adjust
   everything so it'll still work */
	void
shift_formula (int r, int c, int dn, int ov)
{
}


/* ---------------- Routines for dealing with async functions -------------- */


/* This function is called when the alarm has gone off (but not from inside
 * the signal handler!). It schedules timer_cells->fm_refs for recalc. 
 */
	void
cell_alarm (void)
{
}

/* All the timer_cells are going away, 'cuz everything is going away. . . */
	void
flush_all_timers (void)
{
	if (timer_active)
	{
		flush_fm_ref (timer_cells);
		timer_cells = 0;
		timer_active = 0;
	}
}

/* Add CUR_ROW, CUR_COL to the list of active timer-cells, turning on
   the timer_active, if it isn't already */
	void
add_timer_ref (cell*cp, int whereto)
{
	add_ref_to (cp, whereto);
	add_ref_fm (&timer_cells, cur_row, cur_col);
	++timer_active;
}

/* ---------- Routines and vars for dealing with the eval FIFO ------------ */
static struct cell_buf cell_buffer;

/* Start up the FIFO of cells to update */
	void
init_refs (void)
{
	//log_debug("init_refs called");
	cell_buffer.size = FIFO_START;
	cell_buffer.buf = (struct pos *) ck_malloc (cell_buffer.size * sizeof (struct pos));
	bzero (cell_buffer.buf, cell_buffer.size * sizeof (struct pos));
	cell_buffer.push_to_here = cell_buffer.buf;
	cell_buffer.pop_frm_here = cell_buffer.buf;
	//the_vars = hash_new ();
}

/* Push the cells in REF onto the FIFO.  This calls push_cell to do the
   actual work. . . */
void push_refs (cell *cp)
{
}

/* Push a cell onto the FIFO of cells to evaluate, checking for cells
   that are already on the FIFO, etc.

   This does not implement best-order recalculation, since there may be
   intersecting branches in the dependency tree, however, it's close enough
   for most people.
   */
static void cell_buffer_contents (FILE *fp);

void push_cell(coord_t coord)
{
	CELLREF row = get_row(coord);
	CELLREF col = get_col(coord);
}
void push_cell(cell* cp)
{
}

	void
push_cell (CELLREF row, CELLREF col)
{
}

/* Pop a cell off CELL_BUFFER, and evaluate it, displaying the result. . .
   This returns 0 if there are no more cells to update, or if it gets
   an error. */

	int
eval_next_cell (void)
{
	CELL *cp;
	static int loop_counter = 40;

	if (cell_buffer.pop_frm_here == cell_buffer.push_to_here)
		return 0;

	cur_row = cell_buffer.pop_frm_here->row;
	cur_col = cell_buffer.pop_frm_here->col;
	cell_buffer.pop_frm_here++;
	if (cell_buffer.pop_frm_here == cell_buffer.buf + cell_buffer.size)
		cell_buffer.pop_frm_here = cell_buffer.buf;

	if (!(cp = find_cell(cur_row, cur_col)))
		return 0;


	cp->update_cell();
	io_pr_cell (cur_row, cur_col, cp);
	return loop_counter;
}

#if 1
	static void
cell_buffer_contents (FILE *fp)
{
	struct pos *ptr;

	if (!fp)
		fp = stdout;
	if (cell_buffer.pop_frm_here != cell_buffer.push_to_here)
	{
		ptr = cell_buffer.pop_frm_here;
		for (;;)
		{
			/* fprintf (fp, "Ref to %s\r\n", cell_name (ptr->row, ptr->col)); */
			fprintf (fp, " -> %d %d\n", ptr->row, ptr->col);
			if (++ptr == cell_buffer.buf + cell_buffer.size)
				ptr = cell_buffer.buf;
			if (ptr == cell_buffer.push_to_here)
				break;
		}
	}
}

#endif

#if 0
/* ----------------- Routines for dealing with variables ------------------ */

/* Either this needs to be redone as a wrapper for the new new_var_value,
 * or the invocations of it in oleofile.c, sc.c and sylk.c need to be
 * adjusted so it can be deleted altogether.  The new version has 
 * been introduced to improve the interface of new set-var.  The
 * only cost has been the need to introduce and unset-var command,
 * since the '@' argument type now required by set-var will
 * only recognize a valid region.
 */
	char *
old_new_var_value (char *v_name, int v_namelen, char *v_newval)
{
	struct var *var;
	int n;
	int newflag;
	struct rng tmp_rng;

	cur_row = MIN_ROW;
	cur_col = MIN_COL;
	if (v_newval && *v_newval)
	{
		n = parse_cell_or_range (&v_newval, &tmp_rng);
		if (!n)
			return (char *) "Can't parse cell or range";
		if (*v_newval)
			return (char *) "Junk after cell or range";
		newflag = ((n | ROWREL | COLREL) == (R_CELL | ROWREL | COLREL)) ? VAR_CELL : VAR_RANGE;
	}
	else
	{
		tmp_rng.lr = tmp_rng.hr = NON_ROW;
		tmp_rng.lc = tmp_rng.hc = NON_COL;
		newflag = VAR_UNDEF;
	}

	var = find_or_make_var (v_name, v_namelen);

	if (var->var_ref_fm)
	{
		if (var->var_flags != VAR_UNDEF)
		{
			for (n = 0; n < var->var_ref_fm->refs_used; n++)
			{
				flush_range_ref (&(var->v_rng),
						var->var_ref_fm->fm_refs[n].ref_row,
						var->var_ref_fm->fm_refs[n].ref_col);
			}
		}
		var->v_rng = tmp_rng;

		if (var->v_rng.lr != NON_ROW)
		{
			for (n = 0; n < var->var_ref_fm->refs_used; n++)
			{
				cur_row = var->var_ref_fm->fm_refs[n].ref_row;
				cur_col = var->var_ref_fm->fm_refs[n].ref_col;
				add_range_ref (&(var->v_rng));
			}
		}
		for (n = 0; n < var->var_ref_fm->refs_used; n++)
			push_cell (var->var_ref_fm->fm_refs[n].ref_row,
					var->var_ref_fm->fm_refs[n].ref_col);
	}
	else
		var->v_rng = tmp_rng;

	var->var_flags = newflag;

	return 0;
}
#endif

/* This sets the variable V_NAME to V_NEWVAL
 * It returns error msg, or 0 on success.
 * all the appropriate cells have their ref_fm arrays adjusted appropriately
 * This could be smarter; when changing a range var, only the cells that
 * were in the old value but not in the new one need their references flushed,
 * and only the cells that are new need references added.
 * This might also be changed to use add_range_ref()?
 */

	char *
new_var_value (char *v_name, int v_namelen, struct rng *rng)
{
	struct var *var;
	int n = 0;
	int newflag = 0;

	cur_row = MIN_ROW;
	cur_col = MIN_COL;

	//newflag = ((ROWREL | COLREL) == (R_CELL | ROWREL | COLREL)) ? VAR_CELL : VAR_RANGE;

	var = find_or_make_var (v_name, v_namelen);

	if (var->var_ref_fm)
	{
		if (var->var_flags != VAR_UNDEF)
		{
			for (n = 0; n < var->var_ref_fm->refs_used; n++)
			{
				flush_range_ref (&(var->v_rng),
						var->var_ref_fm->fm_refs[n].ref_row,
						var->var_ref_fm->fm_refs[n].ref_col);
			}
		}
		var->v_rng = *rng;

		if (var->v_rng.lr != NON_ROW)
		{
			for (n = 0; n < var->var_ref_fm->refs_used; n++)
			{
				cur_row = var->var_ref_fm->fm_refs[n].ref_row;
				cur_col = var->var_ref_fm->fm_refs[n].ref_col;
				add_range_ref (&(var->v_rng));
			}
		}
		for (n = 0; n < var->var_ref_fm->refs_used; n++)
			push_cell (var->var_ref_fm->fm_refs[n].ref_row,
					var->var_ref_fm->fm_refs[n].ref_col);
	}
	else
		var->v_rng = *rng;

	var->var_flags = newflag;

	return 0;
}

	void
for_all_vars (void (*func) (const char *, struct var *))
{
	for(auto it = the_vars_1.begin(); it != the_vars_1.end() ; ++it) {
		auto s1{it->first};
		const char* s3 = s1.c_str();
		func(s3, &(it->second));
	}
}

struct var *find_var_1(const char* str)
{
	auto it = the_vars_1.find(str);
	if(it != the_vars_1.end())
		return &(it ->second);
	else
		return nullptr;
}

/* Find a variable in the list of variables, or create it if it doesn't
   exist.  Takes a name and a length so the name doesn't have to be
   null-terminated
   */
	struct var *
find_or_make_var(const char *string, int len)
{
	log_debug("find_or_make_var called");
	assert(strlen(string) >= len);
	std::string varname;
	for(int i=0; i<len; ++i) varname += string[i];

	struct var *ret = find_var_1(varname.c_str());
	if(ret) return ret;

	struct var new_var;
	new_var.var_name = varname;
	the_vars_1[varname] = new_var;
	return find_var_1(varname.c_str());
}

/* Like find-or-make-var except returns 0 if it doesn't exist */
	struct var *
find_var (char *string, int len)
{
	//log_debug("find_var called");
	int ch;
	struct var *ret;

	ch = string[len];
	string[len] = '\0';
	ret = find_var_1(string);
	string[len] = ch;
	return ret;
}



/* Free up all the variables, and (if SPLIT_REFS) the ref_fm structure
   associated with each variable.  Note that this does not get rid of
   the struct var *s in cell expressions, so it can only be used when all
   the cells are being freed also
   */
	void
flush_variables (void)
{
	the_vars_1.clear();
}
