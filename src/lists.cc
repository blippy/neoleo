/*
 * Copyright (C) 1990, 1992, 1993 Free Software Foundation, Inc.
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

#include "global.h"
#include "lists.h"
#include "logging.h"
#include "ref.h"
#include "utils.h"

#define ROW_BUF 3
#define COL_BUF 2
#define MAX MAX_ROW
#define MIN MIN_ROW


#define obstack_chunk_alloc ck_malloc
#define obstack_chunk_free free
#include "obstack.h"
static struct obstack find_stack;


using IPTR = int *;




struct list
{
	CELLREF lo, hi;
	struct list *next;
	char mem[1];
};
struct list* the_cols = 0;

using LPTR = list*;
using LLPTR = list**;

struct find
{
	struct find *next;
	CELLREF lo, hi, cur;
	struct list **start;
	struct list *curptr;
	CELLREF left;
	void *ret;
	char fini;
	int ele;
};
static struct find *g_finds = 0;


using FPTR = find *;


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

	for (findp = g_finds; findp; findp = findp->next)
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

	if (!*prevp || ((*prevp)->hi < lo - 1 && (!(*prevp)->next || (*prevp)->next->lo - 1 > hi)))
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
	f->next = g_finds;
	g_finds = f;
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
	f->next = g_finds;
	g_finds = f;
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
	free_find_stack(f);
	g_finds = next;
	return 0;
}


void 
init_cells ()
{
	obstack_begin (&find_stack, sizeof (struct find) * 15);
	the_cols = 0;
}


void flush_cols (void)
{
	struct list *ptr, *nxt;
	int n;

	for (ptr = the_cols; ptr; ptr = nxt)
	{
		nxt = ptr->next;
		for (n = 0; n <= ptr->hi - ptr->lo; n++)
			flush (*(struct list **) (ptr->mem + (n * sizeof (struct list *))));
		free (ptr);
	}
	the_cols = 0;
}

struct cell *
find_cell (CELLREF row, CELLREF col)
{
	void **v = (void**) find (col, the_cols, sizeof (void *));
	return v ? (cell*) find (row, (LPTR) *v, sizeof (struct cell)) :  0;
}

struct cell *
find_or_make_cell (CELLREF row, CELLREF col)
{
	struct list **v  = (LLPTR) make (col, &the_cols, sizeof (struct list *), COL_BUF);
	return (cell*) make (row, v, sizeof (struct cell), ROW_BUF);
}

////////////////////////////////////////////////////////////////////////////////////////////

struct cf
{
	struct cf *next;
	struct find *rows, *cols;
	int make;
};
static struct cf* g_fp;

void find_cells_in_range (struct rng *r)
{

	struct cf *newc = (struct cf *)obstack_alloc (&find_stack, sizeof (struct cf));
	newc->make = 0;
	newc->next = g_fp;
	g_fp = newc;
	newc->rows = (FPTR) find_rng (&the_cols, r->lc, r->hc, sizeof (void *));
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
	newc->next = g_fp;
	g_fp = newc;
	newc->rows = (FPTR) make_rng (&the_cols, r->lc, r->hc, sizeof (void *), ROW_BUF);
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
		if ((ret = (cell *) next_rng (g_fp->cols, 0)))
			return ret;
		new_row = next_rng (g_fp->rows, 0);
		if (!new_row)
		{
			struct cf *old  = g_fp->next;
			obstack_free(&find_stack, g_fp);
			g_fp = old;
			return 0;
		}
		g_fp->cols = g_fp->make ? 
			(FPTR) make_rng((LLPTR) new_row, g_fp->cols->lo, g_fp->cols->hi, sizeof (struct cell), ROW_BUF)
			: (FPTR) find_rng((LLPTR)new_row, g_fp->cols->lo, g_fp->cols->hi, sizeof (struct cell));
	}
}

struct cell *
next_row_col_in_range (CELLREF *rowp, CELLREF *colp)
{
	struct cell *ret;
	struct list **new_row;

	for (;;)
	{
		if ((ret = (cell*) next_rng (g_fp->cols, rowp)))
		{
			*colp = g_fp->rows->cur - 1;
			return ret;
		}
		new_row = (LLPTR) next_rng (g_fp->rows, colp);
		if (!new_row)
		{
			struct cf *old  = g_fp->next;
			obstack_free(&find_stack, g_fp);
			g_fp = old;
			return 0;
		}
		g_fp->cols = g_fp->make ? 
			(FPTR) make_rng (new_row, g_fp->cols->lo, g_fp->cols->hi, sizeof (struct cell), ROW_BUF)
			: (FPTR) find_rng (new_row, g_fp->cols->lo, g_fp->cols->hi, sizeof (struct cell));
	}
}

void
no_more_cells (void)
{

	/* This relies on knowing that the obstack contains
	 * the current find (struct cf) underneath two associated
	 * `struct find's.
	 * Here, we pop all those frames, and then free them at once.
	 */

	struct cf *old = g_fp->next;
	g_finds = g_finds->next->next;
	obstack_free (&find_stack, g_fp);
	g_fp = old;
}

CELLREF
max_row (CELLREF col)
{
	struct list **ptr;

	ptr = (LLPTR) find (col, the_cols, sizeof (void *));
	if (!ptr || !*ptr)
		return MIN;
	while ((*ptr)->next)
		ptr = &((*ptr)->next);
	return (*ptr)->hi;
}

CELLREF max_col()
{
	if (!the_cols) return MIN;
	struct list *ptr;
	for (ptr = the_cols; ptr->next; ptr = ptr->next)
		;
	return ptr->hi;
}

CELLREF 
highest_row (void)
{
	void *f;
	struct list **ptr;
	CELLREF hi = MIN;

	f = find_rng (&the_cols, MIN, MAX, sizeof (void *));
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
	return max_col();
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

