/*
 * $Id: busi.c,v 1.8 2001/02/13 23:38:05 danny Exp $
 *
 * Copyright © 1990, 1992, 1993, 2000, 2001 Free Software Foundation, Inc.
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

#include <math.h>

#include "funcdef.h"
#include "funcs.h"
#include "global.h"
#include "cell.h"
#include "eval.h"
#include "errors.h"
#include "busi.h"
#include "utils.h"
#include "sheet.h"

#define Float	x.c_n
#define String	x.c_s
#define Int	x.c_l
#define Value	x.c_i
#define Rng	x.c_r

static double
pmt (
     double principal,
     double rate,
     double term)
{
  return (principal * rate) / (1 - pow (1 + rate, -(term)));
}

num_t 
busi_pow(num_t x, num_t y)
{
	return (num_t) pow( (double) x, (double) y );
}


static int
npv (
     struct rng *rng,
     num_t rate,
     num_t *putres)
{
  num_t npv;
  int i;
  //double f;
  num_t f;
  CELL *cell_ptr;
  char *strptr;

  find_cells_in_range (rng);
  for (i = 0, npv = 0.0; (cell_ptr = next_cell_in_range ()); i++)
    {
      switch (GET_TYP (cell_ptr))
	{
	case 0:
	  f = 0.0;
	  goto know_f;

	case TYP_INT:
	  f = (double) (cell_ptr->gInt());
	  goto know_f;

	case TYP_FLT:
	  f = cell_ptr->gFlt();
	  goto know_f;

	case TYP_STR:
	  strptr = cell_ptr->gString();
	  f = astof (&strptr);
	  if (*strptr)
	    return NON_NUMBER;
	know_f:
	  npv += (double) f *  (1.0 / (pow (1.0 + rate, (double) i)));
	  break;

	case TYP_ERR:
	  return cell_ptr->gErr();

	default:
	  return NON_NUMBER;
	}
    }

  *putres = npv;
  return 0;
}

static void
do_pmt (
     struct value *p)
{
  p->Float = pmt (p->Float, (p + 1)->Float, (p + 2)->Float);
}

static void
do_pv (
     struct value *p)
{
  double payment, interest, term;

  payment = p[0].Float;
  interest = p[1].Float;
  term = p[2].Float;

  p->Float = payment * ((1 - pow (1 + interest, -term)) / interest);
}

static void
do_npv ( struct value *p)
{
  int tmp;

  tmp = npv (&(p->Rng), (p + 1)->Float, &(p->Float));
  if (tmp)
    {
      p->Value = tmp;
      p->type = TYP_ERR;
    }
  else
    p->type = TYP_FLT;
}

static void
do_irr (struct value *p)
{
  num_t try1;
  num_t res;
  num_t mint, maxt;
  num_t  minr, maxr;
  num_t i;
  num_t tmp;

  minr = maxr = 0;
  mint = maxt = .1;  /* avoid divide by 0 in npv */
  i = 0;
  while (minr >= 0)
    {
      mint += 1;
      tmp = npv (&(p->Rng), mint, &minr);
      if (tmp)
	{
	  p->Value = tmp;
	  p->type = TYP_ERR;
	  return;
	}
      if (++i == 40)
        {
	  p->Value = BAD_INPUT;
	  p->type = TYP_ERR;
	  return;
	}
    }
  i = 0;
  while (maxr <= 0)
    {
      maxt -= 1;
      tmp = npv (&(p->Rng), maxt, &maxr);
      if (tmp)
	{
	  p->Value = tmp;
	  p->type = TYP_ERR;
	  return;
	}
      if (++i == 40)
        {
	  p->Value = BAD_INPUT;
	  p->type = TYP_ERR;
	  return;
	}
    }
  try1 = (p + 1)->Float;
  for (i = 0;; i++)
    {
      if (i == 40)
	{
	  p->Value = BAD_INPUT;
	  p->type = TYP_ERR;
	  return;
	}
      tmp = npv (&(p->Rng), try1, &res);
      if (tmp)
	{
	  p->Value = tmp;
	  p->type = TYP_ERR;
	  return;
	}
      if (fabs (res * 1000000.0) < 1)
	break;
      if (res > 0)
	{
	  maxt = try1;
	  maxr = res;
	}
      else if (res < 0)
	{
	  mint = try1;
	  minr = res;
	}
      if (minr / -10 > maxr)
	{
	  /* it is quite near maxt */
	  try1 = (maxt * 10 + mint) / 11;
	}
      else if (minr / -2 > maxr)
	{
	  try1 = (maxt * 2 + mint) / 3;
	}
      else if (minr * -10 < maxr)
	{
	  /* It is quite near mint */
	  try1 = (maxt + mint * 10) / 11;
	}
      else if (minr * -2 < maxr)
	{
	  try1 = (maxt + mint * 2) / 3;
	}
      else
	try1 = (maxt + mint) / 2;
    }
  p->Float = try1;
  p->type = TYP_FLT;
}

/*
 * Financial management rate of return.  Many advanteges over simple
 * IRR:  single-valued solution, accounts for re-investment and sources
 * of extra outlays.
 *
 * Args: range, safe rate, reinvestment rate, reinvestment minimum
 */
static void
do_fmrr(struct value *p)
{
    struct rng *rng = &p[0].Rng;
    double safe_rate = p[1].Float;
    double reinv_rate = p[2].Float;
    double reinv_min = p[3].Float;
    double *v;
    int i, j, num;
    CELL *cell;

    /* count them */
    find_cells_in_range(rng);
    for (num=0; next_cell_in_range(); num++) ;
    v = (double *) ck_malloc(num * sizeof(double));
    /* now fill */
    find_cells_in_range(rng);
    for (num=0; (cell = next_cell_in_range()); num++) {
      switch (GET_TYP(cell)) {
        case 0:
	  v[num] = 0.;
	  break;
	case TYP_FLT:
	  v[num] = cell->gFlt();
	  break;
	case TYP_INT:
	  v[num] = (double) cell->gInt();
	  break;
	case TYP_STR: {
	  char *cp = cell->gString();
	  v[num] = astof(&cp);
	  if (!*cp)
	    break;
	  /* fall through */
	}
	default:
	case TYP_BOL:
	  p->type = TYP_ERR;
	  p->Value = NON_NUMBER;
	  goto out;
	case TYP_ERR:
	  p->type = TYP_ERR;
	  p->Value = cell->gErr();
	  goto out;
      }
    }
#if 0
    /* debug */
    fprintf(stderr, "init\n\r");
    for (i=0; i<num; i++)
	fprintf(stderr, "%d: %f\n\r", i, v[i]);
#endif
    /* pull back negative values to most recent positive at safe rate */
    for (i=num-1; i>0; i--)
	while (v[i] < 0.) {
	    for (j=i-1; j>0; j--)
		if (v[j] > 0.) {
		    double amt = -v[i];
		    if (amt > v[j])
			amt = v[j];
		    v[i] += amt;
		    amt /= pow(1. + safe_rate, (double)(i-j));
		    v[j] -= amt;
		    break;
		}
	    if (j == 0) {  /* leftover falls into initial investment */
		v[0] -= -v[i] / pow(1. + safe_rate, (double)i);
		v[i] = 0.;
	    }
	}
#if 0
    /* debug */
    fprintf(stderr, "bwd\n\r");
    for (i=0; i<num; i++)
	fprintf(stderr, "%d: %f\n\r", i, v[i]);
#endif
    /* push forward accumulations at reinvestment rate, as long as over min */
    for (i=1; i<num-1; i++)
	if (v[i]) {
	    if (v[i] >= reinv_min) {
		v[num-1] += v[i] * pow(1. + reinv_rate, (double)(num-1-i));
		v[i] = 0.;
	    } else
		v[i+1] += v[i] * (1. + safe_rate);
	}
#if 0
    /* debug */
    fprintf(stderr, "fwd\n\r");
    for (i=0; i<num; i++)
	fprintf(stderr, "%d: %f\n\r", i, v[i]);
#endif
    /* now only v[0] and v[num-1] are nonzero, what is rate to connect them? */
    p->type = TYP_FLT;
    p->Float = pow(v[num-1] / -v[0], 1./(double)(num-1)) - 1.;
  out:
    ck_free(v);
}

static void
do_fv (
     struct value *p)
{
  double payment = p->Float;
  double interest = (p + 1)->Float;
  double term = (p + 2)->Float;

  p->Float = payment * (pow(1 + interest, term) - 1) / interest;
}

static void
do_rate (
     struct value *p)
{
  double future = p->Float;
  double present = (p + 1)->Float;
  double term = (p + 2)->Float;

  p->Float = pow (future / present, 1 / term) - 1;
}

static void
do_term (
     struct value *p)
{
  double payment = p->Float;
  double interest = (p + 1)->Float;
  double future = (p + 2)->Float;

  p->Float = log (1 + future * (interest / payment)) / log (1 + interest);
}

static void
do_cterm (
     struct value *p)
{
  double interest = (p)->Float;
  double future = (p + 1)->Float;
  double present = (p + 2)->Float;

  p->Float = log (future / present) / log (1 + interest);
}

static void
do_sln (
     struct value *p)
{
  double cost = (p)->Float;
  double salvage = (p + 1)->Float;
  double life = (p + 2)->Float;

  if (life < 1)
    {
      p->Value = OUT_OF_RANGE;
      p->type = TYP_ERR;
      return;
    }
  p->Float = (cost - salvage) / life;
}

static void
do_syd (
     struct value *p)
{
  double cost, salvage, life, period;

  cost = p->Float;
  salvage = (p + 1)->Float;
  life = (p + 2)->Float;
  period = (p + 3)->Float;

  if (period < 1 || life < 1)
    {
      p->Value = OUT_OF_RANGE;
      p->type = TYP_ERR;
      return;
    }
  if (period > life)
    p->Float = 0;
  else if (period == life)
    p->Float = salvage;
  else
    p->Float = ((cost - salvage) * (life - period + 1)) / (life * ((life + 1) / 2));
}


static void
do_ddb (
     struct value *p)
{
  double cost = (p)->Float;
  double salvage = (p + 1)->Float;
  long life = (p + 2)->Int;
  long period = (p + 3)->Int;

  double bookval, tmp;
  long n;

  if (period < 1 || period > life || life < 1)
    {
      p->Value = OUT_OF_RANGE;
      p->type = TYP_ERR;
      return;
    }
  bookval = cost;
  tmp = 0;
  for (n = 0; n < period; n++)
    {
      tmp = (bookval * 2) / life;
      bookval -= tmp;
      if (bookval < salvage)
	{
	  tmp += bookval - salvage;
	  bookval = salvage;
	}
    }
  p->Float = tmp;
}

static void
do_anrate (
     struct value *p)
{
  double in_pmt = (p)->Float;
  double present = (p + 1)->Float;
  double term = (p + 2)->Float;

  double tr_lo, tr_hi;
  double mytry;
  double try_pmt;
  int n;

  if (in_pmt * term == present)
    {
      p->Float = 0.0;
      return;
    }
  if (in_pmt * term < present)
    {
      tr_lo = -1;
      tr_hi = 0;
      while (pmt (present, tr_lo, term) > in_pmt)
	{
	  tr_hi = tr_lo;
	  tr_lo *= 2;
	}
    }
  else
    {
      tr_lo = 0;
      tr_hi = 1;
      while (pmt (present, tr_hi, term) < in_pmt)
	{
	  tr_lo = tr_hi;
	  tr_hi *= 2;
	}
    }
  for (n = 0; n < 40; n++)
    {
      mytry = (tr_lo + tr_hi) / 2;
      try_pmt = pmt (present, mytry, term);
      if (try_pmt < in_pmt)
	tr_lo = mytry;
      else if (try_pmt > in_pmt)
	tr_hi = mytry;
      else
	break;
    }
  p->Float = mytry;
}

static void
do_anterm (
     struct value *p)
{
  double payment = (p)->Float;
  double principal = (p + 1)->Float;
  double rate = (p + 2)->Float;

  p->Float = (-log (1 - principal * (rate / payment))) / log (1 + rate);
}


static void
do_balance (
     struct value *p)
{
  double principal = (p)->Float;
  double rate = (p + 1)->Float;
  long term = (p + 2)->Int;
  long period = (p + 3)->Int;

  double tmp_pmt, int_part;
  long num;

  if (term < period)
    {
      p->Value = OUT_OF_RANGE;
      p->type = TYP_ERR;
      return;
    }
  tmp_pmt = pmt (principal, rate, (double) term);
  for (num = 0; num < period; num++)
    {
      int_part = rate * principal;
      if (int_part > tmp_pmt)
	{
	  p->Value = OUT_OF_RANGE;
	  p->type = TYP_ERR;
	  return;
	}
      principal -= tmp_pmt - int_part;
    }
  p->Float = principal;
}

static void
do_paidint (
     struct value *p)
{
  double principal = (p)->Float;
  double rate = (p + 1)->Float;
  long term = (p + 2)->Int;
  long period = (p + 3)->Int;

  double tmp_pmt, int_part, retval;
  long num;

  if (term < period)
    {
      p->Value = OUT_OF_RANGE;
      p->type = TYP_ERR;
      return;
    }
  tmp_pmt = pmt (principal, rate, (double) term);
  retval = 0;
  for (num = 0; num < period; num++)
    {
      int_part = rate * principal;
      if (int_part > tmp_pmt)
	{
	  p->Value = OUT_OF_RANGE;
	  p->type = TYP_ERR;
	  return;
	}
      principal -= tmp_pmt - int_part;
      retval += int_part;
    }
  p->Float = retval;
}

static void
do_kint (
     struct value *p)
{
  double principal = (p)->Float;
  double rate = (p + 1)->Float;
  long term = (p + 2)->Int;
  long period = (p + 3)->Int;

  double tmp_pmt, int_part = 0;
  long num;

  if (term < period)
    {
      p->Value = OUT_OF_RANGE;
      p->type = TYP_ERR;
      return;
    }

  tmp_pmt = pmt (principal, rate, (double) term);
  for (num = 0; num < period; num++)
    {
      int_part = rate * principal;
      if (int_part > tmp_pmt)
	{
	  p->Value = OUT_OF_RANGE;
	  p->type = TYP_ERR;
	  return;
	}
      principal -= tmp_pmt - int_part;
    }
  p->Float = int_part;
}

static void
do_kprin (
     struct value *p)
{
  double principal = (p)->Float;
  double rate = (p + 1)->Float;
  long term = (p + 2)->Int;
  long period = (p + 3)->Int;
  double tmp_pmt, int_part = 0;
  long num;

  if (term < period)
    {
      p->Value = OUT_OF_RANGE;
      p->type = TYP_ERR;
      return;
    }

  tmp_pmt = pmt (principal, rate, (double) term);
  for (num = 0; num < period; num++)
    {
      int_part = rate * principal;
      if (int_part > tmp_pmt)
	{
	  p->Value = OUT_OF_RANGE;
	  p->type = TYP_ERR;
	  return;
	}
      principal -= tmp_pmt - int_part;
    }
  p->Float = tmp_pmt - int_part;
}

static void
do_compbal (struct value *p)
{
  num_t principal = (p)->Float;
  num_t rate = (p + 1)->Float;
  num_t term = (p + 2)->Float;

  p->Float = principal * busi_pow (1 + rate, term);
}

static void
do_sum(struct value* p)
{
	struct rng* rng = &(p->Rng);
	double res = 0;

	CELL* cell_ptr;
	find_cells_in_range (rng);
	for(int i = 0; cell_ptr = next_cell_in_range (); i++) {
		switch(GET_TYP(cell_ptr)) {
			case TYP_INT:
				res += cell_ptr->gInt();
				break;
			case TYP_FLT:
				res += cell_ptr->gFlt();
				break;
		}
	}

	p->sFlt(res);
}

function_t busi_funs[] =
{
  {C_FN2, X_A2, "RF", to_vptr(do_npv), "npv"},		/* 1 */
  {C_FN2, X_A2, "RF", to_vptr(do_irr), "irr"},		/* 2 */

  {C_FN3, X_A3, "FFF", to_vptr(do_pmt), "pmt"},		/* 3 */
  {C_FN3, X_A3, "FFF", to_vptr(do_pv), "pv"},		/* 4 */
  {C_FN3, X_A3, "FFF", to_vptr(do_fv), "fv"},		/* 5 */
  {C_FN3, X_A3, "FFF", to_vptr(do_rate), "rate"},	/* 6 */
  {C_FN3, X_A3, "FFF", to_vptr(do_term), "term"},	/* 7 */
  {C_FN3, X_A3, "FFF", to_vptr(do_cterm), "cterm"},	/* 8 */
  {C_FN3, X_A3, "FFF", to_vptr(do_sln), "sln"},		/* 9 */
  {C_FN3, X_A3, "FFF", to_vptr(do_anrate), "anrate"},	/* 10 */
  {C_FN3, X_A3, "FFF", to_vptr(do_anterm), "anterm"},	/* 11 */
  {C_FN3, X_A3, "FFF", to_vptr(do_compbal), "compbal"},	/* 12 */

  {C_FN4, X_A4, "FFFF", to_vptr(do_syd), "syd"},		/* 13 */
  {C_FN4, X_A4, "FFII", to_vptr(do_ddb), "ddb"},		/* 14 */
  {C_FN4, X_A4, "FFII", to_vptr(do_balance), "balance"},	/* 15 */
  {C_FN4, X_A4, "FFII", to_vptr(do_paidint), "paidint"},	/* 16 */
  {C_FN4, X_A4, "FFII", to_vptr(do_kint), "kint"},	/* 17 */
  {C_FN4, X_A4, "FFII", to_vptr(do_kprin), "kprin"},	/* 18 */

  {C_FN4, X_A4, "RFFF", to_vptr(do_fmrr), "fmrr"},       /* 19 */
  {C_FN1, X_A1, "R",    to_vptr(do_sum), "suma"},

  {0, 0, "", 0, 0},
};

int init_busi_function_count(void) 
{
        return sizeof(busi_funs) / sizeof(struct function) - 1;
}

