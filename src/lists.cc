/*
 * $Id: lists.c,v 1.13 2000/11/22 19:33:01 danny Exp $
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

#include <stdlib.h>
#include <iostream>

using std::cout;

#include "config.h"

#include "global.h"
#include "lists.h"
#include "logging.h"
#include "funcdef.h"
#include "utils.h"

#include "sysdef.h"
#ifndef _DEBUG_MALLOC_INC
#define local_free free
#else 
static void
local_free (p)
     void * p;
{
  free (p);
}
#endif /* def _DEBUG_MALLOC_INC */
#define obstack_chunk_alloc ck_malloc
#define obstack_chunk_free local_free
#include "obstack.h"
#include "global.h"
#include "cell.h"
#include "ref.h"
#include "window.h"
#include "io-term.h"
#include "io-abstract.h"

#define ROW_BUF 3
#define COL_BUF 2
#define MAX MAX_ROW
#define MIN MIN_ROW
static struct obstack find_stack;


using FPTR = find *;
using IPTR = int *;
using LPTR = list*;
using LLPTR = list**;

struct find* alloc_find_stack()
{
	return (struct find *)obstack_alloc (&find_stack, sizeof (struct find));
}

void free_find_stack(struct find* f)
{
	obstack_free (&find_stack, f);
}

static void
flush (struct list *ptr)
{
	struct list *nxt;

	while (ptr)
	{
		nxt = ptr->next;
		free (ptr);
		ptr = nxt;
	}
}

static void
resync (struct list *tofree, struct list *newl, int ele)
{
	struct find *findp;

	if (ele == sizeof (struct cell)
			&& my_cell
			&& (char *) my_cell >= tofree->mem
			&& (char *) my_cell <= tofree->mem + ele * (1 + tofree->hi - tofree->lo))
		my_cell = (struct cell *) (newl->mem + ele * (cur_row - newl->lo));

	for (findp = Global->finds; findp; findp = findp->next)
	{
		if (tofree == findp->curptr)
		{
			CELLREF hi;
			findp->curptr = newl;
			findp->ret = newl->mem + (findp->cur - newl->lo) * ele;
			hi = (findp->hi < newl->hi ? findp->hi : newl->hi);
			if (findp->cur < hi)
				findp->left = hi - findp->cur;
		}
	}

	free (tofree);
}

static void *
find (CELLREF pos, struct list *ptr, int ele)
{
	for (; ptr; ptr = ptr->next)
	{
		if (ptr->lo > pos)
			break;
		if (ptr->hi >= pos)
			return ptr->mem + (pos - ptr->lo) * ele;
	}
	return 0;
}

static void *
make (CELLREF pos, struct list **prevp, int ele, int buf)
{
	//cout << "M" ;
	CELLREF lo, hi;
	size_t size;
	struct list *ptr;

	while (*prevp && (*prevp)->next && (*prevp)->next->lo < pos)
		prevp = &((*prevp)->next);

	/* Was it easy? */
	if (*prevp && (*prevp)->lo <= pos && (*prevp)->hi >= pos)
		return (*prevp)->mem + (pos - (*prevp)->lo) * ele;

	lo = (pos < MIN + buf) ? MIN : pos - buf;
	hi = (pos > MAX - buf) ? MAX : pos + buf;

	if (!*prevp
			|| ((*prevp)->hi < lo - 1
				&& (!(*prevp)->next
					|| (*prevp)->next->lo - 1 > hi)))
	{
		/* Allocate a whole new structure */
		size = (1 + hi - lo) * ele;
		ptr = (LPTR) ck_malloc (sizeof (struct list) + size);
		ptr->lo = lo;
		ptr->hi = hi;
		if (*prevp && (*prevp)->hi < lo)
		{
			ptr->next = (*prevp)->next;
			(*prevp)->next = ptr;
		}
		else
		{
			ptr->next = *prevp;
			*prevp = ptr;
		}
		bzero (ptr->mem, size);
	}
	else if ((*prevp)->lo > lo)
	{
		/* Stretch one down a bit to fit */
		hi = (*prevp)->hi;
		size = (1 + hi - lo) * ele;
		ptr = (LPTR) ck_malloc (sizeof (struct list) + size);
		ptr->lo = lo;
		ptr->hi = hi;
		ptr->next = (*prevp)->next;
		bcopy ((*prevp)->mem, ptr->mem + ((*prevp)->lo - ptr->lo) * ele, (1 + (*prevp)->hi - (*prevp)->lo) * ele);
		bzero (ptr->mem, ((*prevp)->lo - ptr->lo) * ele);
		resync (*prevp, ptr, ele);
		*prevp = ptr;
	}
	else if ((*prevp)->hi < hi && (*prevp)->next && (*prevp)->next->lo <= hi)
	{
		/* Merge this one and the one after it */
		size = (1 + (*prevp)->next->hi - (*prevp)->lo) * ele;
		ptr = (LPTR) ck_malloc (sizeof (struct list) + size);
		ptr->lo = (*prevp)->lo;
		ptr->hi = (*prevp)->next->hi;
		ptr->next = (*prevp)->next->next;
		bcopy ((*prevp)->mem, ptr->mem, (1 + (*prevp)->hi - (*prevp)->lo) * ele);
		bzero (ptr->mem + (1 + (*prevp)->hi - ptr->lo) * ele, ((*prevp)->next->lo - (*prevp)->hi) * ele);
		bcopy ((*prevp)->next->mem,
				ptr->mem + ((*prevp)->next->lo - ptr->lo) * ele,
				(1 + (*prevp)->next->hi - (*prevp)->next->lo) * ele);
		resync ((*prevp)->next, ptr, ele);
		resync (*prevp, ptr, ele);
		*prevp = ptr;
	}
	else if ((*prevp)->hi < hi)
	{
		/* stretch this one up a bit */
		size = (1 + hi - (*prevp)->lo) * ele;
		ptr = (LPTR) ck_malloc (sizeof (struct list) + size);
		ptr->lo = (*prevp)->lo;
		ptr->hi = hi;
		ptr->next = (*prevp)->next;
		bcopy ((*prevp)->mem, ptr->mem, (1 + (*prevp)->hi - (*prevp)->lo) * ele);
		bzero (ptr->mem + (1 + (*prevp)->hi - ptr->lo) * ele, (hi - (*prevp)->hi) * ele);
		resync (*prevp, ptr, ele);
		*prevp = ptr;
	}
	else
		ptr = *prevp;
#ifdef TEST
	if (ptr->lo > pos || ptr->hi < pos)
		panic ("Make at %u not in %u %u", pos, ptr->lo, ptr->hi);
#endif

	return ptr->mem + (pos - ptr->lo) * ele;
}

static void *
find_rng (struct list **start, CELLREF lo, CELLREF hi, int ele)
{
	struct list *ptr;
	struct find *f;

	f = alloc_find_stack();
	f->lo = lo;
	f->hi = hi;
	f->ele = ele;
	f->start = start;
	for (ptr = *start; ptr; ptr = ptr->next)
		if (ptr->hi >= lo)
			break;
	if (ptr && ptr->lo <= hi)
	{
		f->cur = (ptr->lo > lo ? ptr->lo : lo);
		f->curptr = ptr;
		f->ret = ptr->mem + (f->cur - ptr->lo) * ele;
		f->left = 1 + (f->hi < ptr->hi ? f->hi : ptr->hi) - f->cur;
		f->fini = 0;
	}
	else
		f->fini = 1;
	f->next = Global->finds;
	Global->finds = f;
	return f;
}

static void *
make_rng (struct list **start, CELLREF lo, CELLREF hi, int ele, int buf)
{
	struct list **prevp;
	struct list *ptr;
	size_t size;
	struct find *f;

	f = alloc_find_stack();
	f->lo = f->cur = lo;
	f->hi = hi;
	f->left = 1 + hi - lo;
	f->fini = 0;
	f->ele = ele;
	f->start = start;

	lo = lo <= MIN + buf ? MIN : lo - buf;
	hi = hi >= MAX - buf ? MAX : hi + buf;

	for (prevp = start; *prevp && (*prevp)->hi < lo - 1; prevp = &((*prevp)->next))
		;
	ptr = *prevp;
	if (!*prevp || (*prevp)->lo - 1 > hi)
	{
		/* Allocate the whole thing */
		size = (1 + hi - lo) * ele;
		ptr = (LPTR) ck_malloc (sizeof (struct list) + size);
		ptr->lo = lo;
		ptr->hi = hi;
		ptr->next = *prevp;
		bzero (ptr->mem, size);
		if (*prevp && (*prevp)->hi < lo)
		{
			ptr->next = (*prevp)->next;
			(*prevp)->next = ptr;
		}
		else
		{
			ptr->next = *prevp;
			*prevp = ptr;
		}
		*prevp = ptr;
	}
	else
	{
		if ((*prevp)->lo > lo)
		{
			/* Stretch this one down a bit */
			size = (1 + (*prevp)->hi - lo) * ele;
			ptr = (LPTR) ck_malloc (sizeof (struct list) + size);
			ptr->lo = lo;
			ptr->hi = (*prevp)->hi;
			ptr->next = (*prevp)->next;
			bcopy ((*prevp)->mem,
					ptr->mem + ((*prevp)->lo - ptr->lo) * ele,
					(1 + (*prevp)->hi - (*prevp)->lo) * ele);
			bzero (ptr->mem, ((*prevp)->lo - lo) * ele);
			resync (*prevp, ptr, ele);
			*prevp = ptr;
		}
		while ((*prevp)->hi < hi && (*prevp)->next && (*prevp)->next->lo <= hi)
		{
			/* Merge this one and the one after it */
			/* Repeat as needed */
			size = (1 + (*prevp)->next->hi - (*prevp)->lo) * ele;
			ptr = (LPTR) ck_malloc (sizeof (struct list) + size);
			ptr->lo = (*prevp)->lo;
			ptr->hi = (*prevp)->next->hi;
			ptr->next = (*prevp)->next->next;
			bcopy ((*prevp)->mem, ptr->mem, (1 + (*prevp)->hi - (*prevp)->lo) * ele);
			bzero (ptr->mem + (1 + (*prevp)->hi - ptr->lo) * ele, ((*prevp)->next->lo - (*prevp)->hi) * ele);
			bcopy ((*prevp)->next->mem,
					ptr->mem + ((*prevp)->next->lo - ptr->lo) * ele,
					(1 + (*prevp)->next->hi - (*prevp)->next->lo) * ele);
			resync ((*prevp)->next, ptr, ele);
			resync (*prevp, ptr, ele);
			*prevp = ptr;
		}
		if ((*prevp)->hi < hi)
		{
			/* stretch this one up a bit */
			size = (1 + hi - (*prevp)->lo) * ele;
			ptr = (LPTR) ck_malloc (sizeof (struct list) + size);
			ptr->lo = (*prevp)->lo;
			ptr->hi = hi;
			ptr->next = (*prevp)->next;
			bcopy ((*prevp)->mem, ptr->mem, (1 + (*prevp)->hi - (*prevp)->lo) * ele);
			bzero (ptr->mem + (1 + (*prevp)->hi - ptr->lo) * ele, (hi - (*prevp)->hi) * ele);
			resync (*prevp, ptr, ele);
			*prevp = ptr;
		}
	}
#ifdef TEST
	if (ptr->lo > f->lo || ptr->hi < f->hi)
		panic ("Vector of %u-%u not big enough for %u-%u", (*prevp)->lo, (*prevp)->hi, f->lo, f->hi);
#endif
	f->curptr = ptr;
	f->ret = ptr->mem + (f->cur - ptr->lo) * ele;
	f->next = Global->finds;
	Global->finds = f;
	return f;
}

static void *
next_rng (struct find *f, CELLREF *posp)
{
	void *ret;
	struct find *next;

	if (!f)
		return 0;
	if (!f->fini)
	{
		if (f->left)
		{
			--(f->left);
fini:
			if (posp)
				*posp = f->cur;
			f->cur++;
			ret = f->ret;
			f->ret = (char *) (f->ret) + f->ele;
			return ret;
		}
		if (f->curptr->hi < f->hi)
		{
			f->curptr = f->curptr->next;
			if (f->curptr && f->curptr->lo <= f->hi)
			{
				f->ret = f->curptr->mem;
				f->left = (f->hi < f->curptr->hi ? f->hi : f->curptr->hi) - f->curptr->lo;
				f->cur = f->curptr->lo;
				goto fini;
			}
		}
	}
	next = f->next;
	//obstack_free (&find_stack, f);
	free_find_stack(f);
	Global->finds = next;
	return 0;
}


void 
init_cells ()
{
	obstack_begin (&find_stack, sizeof (struct find) * 15);
	Global->the_cols = 0;
	Global->wids = 0;
	Global->hgts = 0;
}

void 
flush_everything (void)
{
	struct list *ptr, *nxt;
	int n;

	flush_variables ();
	for (ptr = Global->the_cols; ptr; ptr = nxt)
	{
		nxt = ptr->next;
		for (n = 0; n <= ptr->hi - ptr->lo; n++)
			flush (*(struct list **) (ptr->mem + (n * sizeof (struct list *))));
		free (ptr);
	}
	Global->the_cols = 0;
	flush (Global->wids);
	Global->wids = 0;
	flush (Global->hgts);
	Global->hgts = 0;
	//flush_fonts ();
}

struct cell *
find_cell (CELLREF row, CELLREF col)
{
	//if(use_alt_cells)
	//	return alt_find_cell(row, col);
	void **v;

	v = (void**) find (col, Global->the_cols, sizeof (void *));
	return v ? (cell*) find (row, (LPTR) *v, sizeof (struct cell)) :  0;
}

struct cell *
find_or_make_cell (CELLREF row, CELLREF col)
{
	struct list **v;

	v = (LLPTR) make (col, &Global->the_cols, sizeof (struct list *), COL_BUF);
	return (cell*) make (row, v, sizeof (struct cell), ROW_BUF);
}

void
find_cells_in_range (struct rng *r)
{

	struct cf *newc = (struct cf *)obstack_alloc (&find_stack, sizeof (struct cf));
	newc->make = 0;
	newc->next = Global->fp;
	Global->fp = newc;
	newc->rows = (FPTR) find_rng (&Global->the_cols, r->lc, r->hc, sizeof (void *));
	struct list **firstcol = (LLPTR) next_rng (newc->rows, 0);
	if (firstcol)
		newc->cols = (FPTR) find_rng (firstcol, r->lr, r->hr, sizeof (struct cell));
	else
		newc->cols = 0;
}

void
make_cells_in_range (struct rng *r)
{

	struct cf *newc =  (struct cf *)obstack_alloc (&find_stack, sizeof (struct cf));
	newc->make = 1;
	newc->next = Global->fp;
	Global->fp = newc;
	newc->rows = (FPTR) make_rng (&Global->the_cols, r->lc, r->hc, sizeof (void *), ROW_BUF);
	struct list **firstcol = (LLPTR) next_rng (newc->rows, 0);
	newc->cols = (FPTR) make_rng (firstcol, r->lr, r->hr, sizeof (struct cell), COL_BUF);
}

struct cell *
next_cell_in_range (void)
{
	struct cell *ret;
	void *new_row;

	for (;;)
	{
		if ((ret = (cell *) next_rng (Global->fp->cols, 0)))
			return ret;
		new_row = next_rng (Global->fp->rows, 0);
		if (!new_row)
		{
			struct cf *old;

			old = Global->fp->next;
			obstack_free(&find_stack, Global->fp);
			Global->fp = old;
			return 0;
		}
		Global->fp->cols = Global->fp->make ? 
			(FPTR) make_rng((LLPTR) new_row, Global->fp->cols->lo, Global->fp->cols->hi, sizeof (struct cell), ROW_BUF)
			: (FPTR) find_rng((LLPTR)new_row, Global->fp->cols->lo, Global->fp->cols->hi, sizeof (struct cell));
	}
}

struct cell *
next_row_col_in_range (CELLREF *rowp, CELLREF *colp)
{
	struct cell *ret;
	struct list **new_row;

	for (;;)
	{
		if ((ret = (cell*) next_rng (Global->fp->cols, rowp)))
		{
			*colp = Global->fp->rows->cur - 1;
			return ret;
		}
		new_row = (LLPTR) next_rng (Global->fp->rows, colp);
		if (!new_row)
		{
			struct cf *old;

			old = Global->fp->next;
			obstack_free(&find_stack, Global->fp);
			Global->fp = old;
			return 0;
		}
		Global->fp->cols = Global->fp->make ? 
			(FPTR) make_rng (new_row, Global->fp->cols->lo, Global->fp->cols->hi, sizeof (struct cell), ROW_BUF)
			: (FPTR) find_rng (new_row, Global->fp->cols->lo, Global->fp->cols->hi, sizeof (struct cell));
	}
}

void
no_more_cells (void)
{
	struct cf *old;

	/* This relies on knowing that the obstack contains
	 * the current find (struct cf) underneath two associated
	 * `struct find's.
	 * Here, we pop all those frames, and then free them at once.
	 */

	old = Global->fp->next;
	Global->finds = Global->finds->next->next;
	obstack_free (&find_stack, Global->fp);
	Global->fp = old;
}

CELLREF
max_row (CELLREF col)
{
	struct list **ptr;

	ptr = (LLPTR) find (col, Global->the_cols, sizeof (void *));
	if (!ptr || !*ptr)
		return MIN;
	while ((*ptr)->next)
		ptr = &((*ptr)->next);
	return (*ptr)->hi;
}

CELLREF
max_col (CELLREF row)
{
	struct list *ptr;

	if (!Global->the_cols)
		return MIN;
	for (ptr = Global->the_cols; ptr->next; ptr = ptr->next)
		;
	return ptr->hi;
}

CELLREF 
highest_row (void)
{
	void *f;
	struct list **ptr;
	CELLREF hi = MIN;

	f = find_rng (&Global->the_cols, MIN, MAX, sizeof (void *));
	while ((ptr = (LLPTR) next_rng ((FPTR)f, 0)))
	{
		if (*ptr)
		{
			while ((*ptr)->next)
				ptr = &((*ptr)->next);
			if ((*ptr)->hi > hi)
				hi = (*ptr)->hi;
		}
	}
	return hi;
}


CELLREF 
highest_col (void)
{
	struct list *ptr;

	if (!Global->the_cols)
		return MIN;
	for (ptr = Global->the_cols; ptr->next; ptr = ptr->next)
		;
	return ptr->hi;
}


/* Routines for dealing with the widths of columns. . . */

int 
get_width (CELLREF col)
{
	int *ptr;

	ptr = (IPTR) find (col, Global->wids, sizeof (int));
	if (!ptr || !*ptr)
		return default_width;
	return (*ptr) - 1;
}


int 
get_nodef_width (CELLREF col)
{
	int *ptr;

	ptr = (IPTR) find (col, Global->wids, sizeof (int));
	return ptr ? *ptr : 0;
}

void 
set_width (CELLREF col, int wid)
{
	int *ptr;

	ptr = (IPTR) make (col, &Global->wids, sizeof (int), COL_BUF);
	*ptr = wid;
	io_update_width(col, wid);
}

struct find*
find_span(struct list** spans, CELLREF lo, CELLREF hi)
{
	//return (FPTR) find_rng (&Global->wids, lo, hi, sizeof (int));
	return (FPTR) find_rng(spans, lo, hi, sizeof (int));
}

int 
next_span(struct find* s_find, CELLREF *posp)
{
	int *ptr;

	do
		ptr = (IPTR) next_rng(s_find, posp);
	while (ptr && !*ptr);
	return ptr ? *ptr : 0;
}

static void
do_shift (int over, CELLREF lo, CELLREF hi, struct list **start, int buf)
{
	CELLREF pos;
	int w;
	int *ptr;
	int inc;
	struct list *p;

	if (!*start)
		return;
	for (p = *start; p->next; p = p->next)
		;

	if (hi > p->hi)
		hi = p->hi;

	if (over > 0)
	{
		pos = hi;
		hi = lo;
		lo = pos;
		inc = -1;
	}
	else
		inc = 1;

	if (inc > 0)
	{
		if (lo > hi)
			return;
	}
	else if (hi > lo)
		return;

	for (pos = lo;; pos += inc)
	{
		ptr = (IPTR) find (pos, *start, sizeof (int));
		w = ptr ? *ptr : 0;
		ptr =  w ? (IPTR) make (pos + over, start, sizeof (int), buf) :
			(IPTR) find (pos + over, *start, sizeof (int));
		if (w || (ptr && *ptr))
			*ptr = w;
		if (pos == hi)
			break;
	}
	for (pos = hi + over;;)
	{
		pos += inc;
		ptr = (IPTR) find (pos, *start, sizeof (int));
		if (ptr)
			*ptr = 0;
		if (pos == hi)
			break;
	}
}

void 
shift_widths (int over, CELLREF lo, CELLREF hi)
{
	if constexpr (true) {
		log_debug("shift_widths:over:" + std::to_string(over) + ",lo:" + std::to_string(lo) 
				+ ",hi:" + std::to_string(hi));
	}
	do_shift (over, lo, hi, &Global->wids, COL_BUF);
}


/* Routines for dealing with the height of rows 
 */
int 
get_height (CELLREF row)
{
	int *ptr;

	ptr = (IPTR) find (row, Global->hgts, sizeof (int));
	if (!ptr || !*ptr)
		return default_height;
	return *ptr - 1 + (Global->display_formula_mode && using_curses);
}

int 
get_nodef_height (CELLREF row)
{
	int *ptr;

	ptr = (IPTR) find (row, Global->hgts, sizeof (int));
	return ptr ? *ptr : 0;
}

void 
set_height (CELLREF row, int hgt)
{
	int *ptr;

	ptr = (IPTR) make (row, &Global->hgts, sizeof (int), ROW_BUF);
	*ptr = hgt;
}

void
set_user_scales (double hs, double ws)
{
	Global->user_height_scale = hs;
	Global->user_width_scale = ws;
	io_repaint ();
}

int
get_scaled_height (CELLREF r)
{
	return ((Global->user_height_scale <= 0.)
			? 1
			:  (int) (get_height (r) * Global->height_scale * Global->user_height_scale));
}

int
get_scaled_width (CELLREF c)
{
	return ((Global->user_width_scale <= 0.)
			? 1
			: (int)(get_width (c) * Global->width_scale * Global->user_width_scale));
}



void 
shift_heights (int dn, CELLREF lo, CELLREF hi)
{
	do_shift (dn, lo, hi, &Global->hgts, ROW_BUF);
}

