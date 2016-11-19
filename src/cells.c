/*
 * $Id: cells.c,v 1.12 2000/08/10 21:02:50 danny Exp $
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#define obstack_chunk_alloc ck_malloc
#define obstack_chunk_free free
#include "obstack.h"
#include "funcdef.h"
#include "sysdef.h"

#include "global.h"
#include "cell.h"
#include "eval.h"
#include "errors.h"
#include "lists.h"
#include "format.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "io-term.h"
#include "cmd.h"
#include "ref.h"

#ifdef	HAVE_MOTIF
#include "io-motif.h"
#endif

struct value
  {
    int type;
    union vals x;
  };

#define Float	x.c_d
#define String	x.c_s
#define Int	x.c_l
#define Value	x.c_i
#define Rng	x.c_r

#define ERROR(x)	\
 {			\
	p->Value=x;	\
	p->type=TYP_ERR;\
	return;		\
 }


static int
cell (row, col, dowhat, p)
     long row;
     long col;
     char *dowhat;
     struct value *p;
{
  struct func
    {
      char *name;
      int typ;
    };

  static struct func cell_funs[] =
  {
    {"row", TYP_INT},		/* 1 */
    {"column", TYP_INT},	/* 2 */
    {"width", TYP_INT},		/* 3 */
    {"lock", TYP_STR},		/* 4 */
    {"protection", TYP_STR},	/* 5 */
    {"justify", TYP_STR},	/* 6 */
    {"alignment", TYP_STR},	/* 7 */
    {"fmt", TYP_STR},		/* 8 */
    {"format", TYP_STR},	/* 9 */
    {"type", TYP_STR},		/* 10 */
    {"formula", TYP_STR},	/* 11 */
    {"value", 0},		/* 12 */
    {0, 0}
  };

  CELL *cell_ptr;
  char *strptr;
  struct func *func;
  struct func *f1;
  int n;

  n = strlen (dowhat) - 1;
  f1 = 0;
  for (func = cell_funs; func->name; func++)
    if (func->name[0] == dowhat[0]
	&& (n == 0 || !strincmp (&(func->name[1]), &dowhat[1], n)))
      {
	if (f1)
	  return BAD_INPUT;
	f1 = func;
      }
  if (!f1)
    return BAD_INPUT;
  p->type = f1->typ;
  switch (f1 - cell_funs)
    {
    case 0:
      p->Int = row;
      break;
    case 1:
      p->Int = col;
      break;
    case 2:
      p->Int = get_width (col);
      break;
    case 3:
    case 4:
      cell_ptr = find_cell (row, col);
      p->String = ( (cell_ptr ? GET_LCK (cell_ptr) : default_lock)
		   ? "locked"
		   : "unlocked");
      break;
    case 5:
    case 6:
      cell_ptr = find_cell (row, col);
      p->String = jst_to_str (cell_ptr ?  GET_JST (cell_ptr) : default_jst); 
      break;
    case 7:
    case 8:
      p->String = cell_format_string(find_cell(row, col));
      break;
    case 9:
      cell_ptr = find_cell (row, col);
      if (cell_ptr)
	switch (GET_TYP (cell_ptr))
	  {
	  case TYP_FLT:
	    p->String = "float";
	    break;
	  case TYP_INT:
	    p->String = "integer";
	    break;
	  case TYP_STR:
	    p->String = "string";
	    break;
	  case TYP_BOL:
	    p->String = "boolean";
	    break;
	  case TYP_ERR:
	    p->String = "error";
	    break;
	  default:
	    p->String = "unknown";
	  }
      else
	p->String = "null";
      break;
    case 10:
      cell_ptr = find_cell (row, col);
      if (cell_ptr && (GET_TYP (cell_ptr) || cell_ptr->cell_formula))
	{
	  strptr = decomp (row, col, cell_ptr);
	  p->String = obstack_alloc (&tmp_mem, strlen (strptr) + 1);
	  strcpy (p->String, strptr);
	  decomp_free ();
	}
      else
	p->String = "";
      break;
    case 11:
      cell_ptr = find_cell (row, col);
      if (cell_ptr)
	{
	  p->type = GET_TYP (cell_ptr);
	  p->x = cell_ptr->c_z;
	}
      else
	p->type = 0;
      break;
    default:
      return BAD_INPUT;
    }
  return 0;
}


static void
do_curcell (p)
     struct value *p;
{
  int tmp;

  tmp = cell (curow, cucol, p->String, p);
  if (tmp)
    ERROR (tmp);
}

static void
do_my (p)
     struct value *p;
{
  int tmp;

  tmp = cell (cur_row, cur_col, p->String, p);
  if (tmp)
    ERROR (tmp);
}

/* Note that the second argument may be *anything* including ERROR.  If it is
   error, we find the first occurence of that ERROR in the range */

static void
do_member (p)
     struct value *p;
{
  CELLREF crow;
  CELLREF ccol;
  int foundit;
  CELL *cell_ptr;

  find_cells_in_range (&(p->Rng));
  while ((cell_ptr = next_row_col_in_range (&crow, &ccol)))
    {
      if (GET_TYP (cell_ptr) != (p + 1)->type)
	continue;
      switch ((p + 1)->type)
	{
	case 0:
	  foundit = 1;
	  break;
	case TYP_FLT:
	  foundit = cell_ptr->cell_flt == (p + 1)->Float;
	  break;
	case TYP_INT:
	  foundit = cell_ptr->cell_int == (p + 1)->Int;
	  break;
	case TYP_STR:
	  foundit = !strcmp (cell_ptr->cell_str, (p + 1)->String);
	  break;
	case TYP_BOL:
	  foundit = cell_ptr->cell_bol == (p + 1)->Value;
	  break;
	case TYP_ERR:
	  foundit = cell_ptr->cell_err == (p + 1)->Value;
	  break;
	default:
	  foundit = 0;
#ifdef TEST
	  panic ("Unknown type (%d) in member", (p + 1)->type);
#endif
	}
      if (foundit)
	{
	  no_more_cells ();
	  p->Int = 1 + crow - p->Rng.lr + (ccol - p->Rng.lc) * (1 + p->Rng.hr - p->Rng.lr);
	  p->type = TYP_INT;
	  return;
	}
    }
  p->Int = 0L;
  p->type = TYP_INT;
}

static void
do_smember (p)
     struct value *p;
{
  CELLREF crow;
  CELLREF ccol;
  CELL *cell_ptr;
  char *string;

  string = (p + 1)->String;
  find_cells_in_range (&(p->Rng));
  while ((cell_ptr = next_row_col_in_range (&crow, &ccol)))
    {
      if (((GET_TYP (cell_ptr) == 0) && (string[0] == '\0'))
	  || (cell_ptr && (GET_TYP (cell_ptr) == TYP_STR)
	      && strstr (string, cell_ptr->cell_str)))
	{
	  no_more_cells ();
	  p->Int = 1 + (crow - p->Rng.lr)
	    + (ccol - p->Rng.lc) * (1 + (p->Rng.hr - p->Rng.lr));
	  p->type = TYP_INT;
	  return;
	}
    }
  p->Int = 0L;
  p->type = TYP_INT;
}

static void
do_members (p)
     struct value *p;
{
  CELLREF crow;
  CELLREF ccol;
  CELL *cell_ptr;
  char *string;

  string = (p + 1)->String;
  find_cells_in_range (&(p->Rng));
  while ((cell_ptr = next_row_col_in_range (&crow, &ccol)))
    {
      if (GET_TYP (cell_ptr) != TYP_STR)
	continue;
      if (strstr (cell_ptr->cell_str, string))
	{
	  no_more_cells ();
	  p->Int = 1 + (crow - p->Rng.lr)
	    + (ccol - p->Rng.lc) * (1 + (p->Rng.hr - p->Rng.lr));
	  p->type = TYP_INT;
	  return;
	}
    }
  p->Int = 0L;
  p->type = TYP_INT;
}

static void
do_pmember (p)
     struct value *p;
{
  CELLREF crow;
  CELLREF ccol;
  CELL *cell_ptr;
  char *string;

  string = (p + 1)->String;
  find_cells_in_range (&(p->Rng));
  while ((cell_ptr = next_row_col_in_range (&crow, &ccol)))
    {
      if ((GET_TYP (cell_ptr) == 0 && string[0] == '\0')
	  || (cell_ptr && GET_TYP (cell_ptr) == TYP_STR && !strncmp (string, cell_ptr->cell_str, strlen (cell_ptr->cell_str))))
	{
	  no_more_cells ();
	  p->Int = 1 + (crow - p->Rng.lr)
	    + (ccol - p->Rng.lc) * (1 + (p->Rng.hr - p->Rng.lr));
	  p->type = TYP_INT;
	  return;
	}
    }
  p->Int = 0L;
  p->type = TYP_INT;
}

static void
do_memberp (p)
     struct value *p;
{
  CELLREF crow;
  CELLREF ccol;
  CELL *cell_ptr;
  int tmp;
  char *string;

  string = (p + 1)->String;
  find_cells_in_range (&(p->Rng));
  tmp = strlen (string);
  while ((cell_ptr = next_row_col_in_range (&crow, &ccol)))
    {
      if (GET_TYP (cell_ptr) != TYP_STR)
	continue;
      if (!strncmp (cell_ptr->cell_str, string, tmp))
	{
	  no_more_cells ();
	  p->Int = 1 + (crow - p->Rng.lr)
	    + (ccol - p->Rng.lc) * (1 + (p->Rng.hr - p->Rng.lr));
	  p->type = TYP_INT;
	  return;
	}
    }
  p->Int = 0L;
  p->type = TYP_INT;
}

static void
do_hlookup (p)
     struct value *p;
{

  struct rng *rng = &((p)->Rng);
  double fltval = (p + 1)->Float;
  long offset = (p + 2)->Int;

  CELL *cell_ptr;
  double f;
  CELLREF col;
  CELLREF row;
  char *strptr;

  row = rng->lr;
  for (col = rng->lc; col <= rng->hc; col++)
    {
      if (!(cell_ptr = find_cell (row, col)))
	ERROR (NON_NUMBER);
      switch (GET_TYP (cell_ptr))
	{
	case TYP_FLT:
	  if (fltval < cell_ptr->cell_flt)
	    goto out;
	  break;
	case TYP_INT:
	  if (fltval < cell_ptr->cell_int)
	    goto out;
	  break;
	case TYP_STR:
	  strptr = cell_ptr->cell_str;
	  f = astof (&strptr);
	  if (!*strptr && fltval > f)
	    goto out;
	  else
	    ERROR (NON_NUMBER);
	case 0:
	case TYP_BOL:
	case TYP_ERR:
	default:
	  ERROR (NON_NUMBER);
	}
    }
out:
  if (col == rng->lc)
    ERROR (OUT_OF_RANGE);
  --col;
  row = rng->lr + offset;
  if (row > rng->hr)
    ERROR (OUT_OF_RANGE);
  cell_ptr = find_cell (row, col);
  if (!cell_ptr)
    {
      p->type = 0;
      p->Int = 0;
    }
  else
    {
      p->type = GET_TYP (cell_ptr);
      p->x = cell_ptr->c_z;
    }
}

static void
do_vlookup (p)
     struct value *p;
{

  struct rng *rng = &((p)->Rng);
  double fltval = (p + 1)->Float;
  long offset = (p + 2)->Int;

  CELL *cell_ptr;
  double f;
  CELLREF col;
  CELLREF row;
  char *strptr;

  col = rng->lc;
  for (row = rng->lr; row <= rng->hr; row++)
    {
      if (!(cell_ptr = find_cell (row, col)))
	ERROR (NON_NUMBER);
      switch (GET_TYP (cell_ptr))
	{
	case TYP_FLT:
	  if (fltval < cell_ptr->cell_flt)
	    goto out;
	  break;
	case TYP_INT:
	  if (fltval < cell_ptr->cell_int)
	    goto out;
	  break;
	case TYP_STR:
	  strptr = cell_ptr->cell_str;
	  f = astof (&strptr);
	  if (!*strptr && fltval > f)
	    goto out;
	  else
	    ERROR (NON_NUMBER);
	case 0:
	case TYP_BOL:
	case TYP_ERR:
	default:
	  ERROR (NON_NUMBER);
	}
    }
out:
  if (row == rng->lr)
    ERROR (OUT_OF_RANGE);
  --row;
  col = rng->lc + offset;
  if (col > rng->hc)
    ERROR (OUT_OF_RANGE);

  cell_ptr = find_cell (row, col);
  if (!cell_ptr)
    {
      p->type = 0;
      p->Int = 0;
    }
  else
    {
      p->type = GET_TYP (cell_ptr);
      p->x = cell_ptr->c_z;
    }
}

static void
do_vlookup_str (p)
     struct value *p;
{

  struct rng *rng = &((p)->Rng);
  char * key = (p + 1)->String;
  long offset = (p + 2)->Int;

  CELL *cell_ptr;
  CELLREF col;
  CELLREF row;

  col = rng->lc;
  for (row = rng->lr; row <= rng->hr; row++)
    {
      if (!(cell_ptr = find_cell (row, col)))
	ERROR (NON_NUMBER);
      switch (GET_TYP (cell_ptr))
	{
	case TYP_STR:
	  if (!strcmp (key, cell_ptr->cell_str))
	    goto out;
	  break;
	case 0:
	case TYP_FLT:
	case TYP_INT:
	case TYP_BOL:
	case TYP_ERR:
	default:
	  ERROR (NON_NUMBER);
	}
    }
out:
  if (row > rng->hr)
    ERROR (OUT_OF_RANGE);
  col = rng->lc + offset;
  if (col > rng->hc)
    ERROR (OUT_OF_RANGE);

  cell_ptr = find_cell (row, col);
  if (!cell_ptr)
    {
      p->type = 0;
      p->Int = 0;
    }
  else
    {
      p->type = GET_TYP (cell_ptr);
      p->x = cell_ptr->c_z;
    }
}


static void
do_cell (p)
     struct value *p;
{
  int tmp;

  tmp = cell (p->Int, (p + 1)->Int, (p + 2)->String, p);
  if (tmp)
    ERROR (tmp);
}

/* While writing the macro loader, I found a need for a function
 * that could report back on whether or not a variable was defined.
 * The function below does that and more.  It accepts an integer
 * and two strings as arguments.  The integer specifies a corner;
 * 0 for NW, 1 for NE, 2 for SE and 3 for SW.  The first string
 * argument should specify a variable name.  The second should
 * specify one of the elements accepted by cell().  If the
 * variable has not been defined, this reports back its bare
 * name as a string.
 */

static void
do_varval (struct value *p)
{
  int tmp;
  int vr;
  int vc;
  struct var * v;

  v = find_var ((p+1)->String, strlen((p+1)->String));

  if (!v || v->var_flags == VAR_UNDEF) {
    p->type = TYP_STR;
    p->String = (p+1)->String;
  } else {
    switch(p->Int) {
      case 0:
        vr = v->v_rng.lr;
        vc = v->v_rng.lc;
        break;
      case 1:
        vr = v->v_rng.lr;
        vc = v->v_rng.hc;
        break;
      case 2:
        vr = v->v_rng.hr;
        vc = v->v_rng.hc;
        break;
      case 3:
        vr = v->v_rng.hr;
        vc = v->v_rng.lc;
        break;
      default:
        ERROR(OUT_OF_RANGE);
        return;
    }
    p->String = (p+2)->String;
    tmp = cell (vr, vc, p->String, p);
    if (tmp)
      ERROR (tmp);
  }
}

static void
do_button(struct value *p)
{
#ifdef	HAVE_MOTIF
  MotifButton(cur_row, cur_col, p->String, (p+1)->String);
#endif

  p->type = TYP_STR;
  p->String = (p+1)->String;
}

struct function cells_funs[] =
{
  {C_FN1 | C_T, X_A1, "S", do_curcell, "curcell"},
  {C_FN1 | C_T, X_A1, "S", do_my, "my"},
  {C_FN3 | C_T, X_A3, "IIS", do_cell, "cell"},
  {C_FN3 | C_T, X_A3, "ISS", do_varval, "varval"},

  {C_FN2, X_A2, "RA", do_member, "member"},
  {C_FN2, X_A2, "RS", do_smember, "smember"},
  {C_FN2, X_A2, "RS", do_members, "members"},
  {C_FN2, X_A2, "RS", do_pmember, "pmember"},
  {C_FN2, X_A2, "RS", do_memberp, "memberp"},

  {C_FN3, X_A3, "RFI", do_hlookup, "hlookup"},
  {C_FN3, X_A3, "RFI", do_vlookup, "vlookup"},
  {C_FN3, X_A3, "RSI", do_vlookup_str, "vlookup_str"},

  {C_FN2,	X_A2,	"SS",	do_button,	"button" },

  {0, 0, "", 0, 0},
};

int init_cells_function_count(void) 
{
        return sizeof(cells_funs) / sizeof(struct function) - 1;
}
