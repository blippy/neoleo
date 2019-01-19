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

#include <iostream>
#include <math.h>

#include "funcs.h"
#include "global.h"
#include "cell.h"
#include "eval.h"
#include "errors.h"
#include "busi.h"
#include "logging.h"
#include "utils.h"
#include "sheet.h"

using std::cout;
using std::endl;

static double
pmt ( double principal, double rate, double term)
{
  return (principal * rate) / (1 - pow (1 + rate, -(term)));
}

num_t 
busi_pow(num_t x, num_t y)
{
	return (num_t) pow( (double) x, (double) y );
}

double to_double(const char* strptr, bool &ok)
{
	double d;
	ok = true;
	size_t idx;
	try { 
		d = std::stod(strptr, &idx);
	} catch(...) {
		ok = false;
	}
	return d;
}

static int
npv ( struct rng *rng, num_t rate, num_t *putres)
{
	num_t npv = 0;
	int i = 0;;
	num_t f;
	CELL *cell_ptr;
	const char *strptr;
	bool ok;

	for(auto cell_ptr: get_cells_in_range(rng)) {
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
				f = to_double(strptr, ok);
				if (!ok) return NON_NUMBER;
know_f:
				npv += (double) f *  (1.0 / (pow (1.0 + rate, (double) i)));
				break;

			case TYP_ERR:
				return cell_ptr->gErr();

			default:
				return NON_NUMBER;
		}
		i++;
	}

	*putres = npv;
	return 0;
}

static void
do_pmt ( struct value *p)
{
	p->sFlt(pmt (p->gFlt(), (p + 1)->gFlt(), (p + 2)->gFlt()));
}

static void
do_pv ( struct value *p)
{
	double payment, interest, term;

	payment = p[0].gFlt();
	interest = p[1].gFlt();
	term = p[2].gFlt();

	p->sFlt(payment * ((1 - pow (1 + interest, -term)) / interest));
}

static void
do_npv ( struct value *p)
{
	num_t putres = p->gFlt();
	struct rng a_rng = p->gRng();
  int tmp =  npv (&a_rng, (p + 1)->gFlt(), &putres);
  p->sFlt(putres);
  if (tmp)
    {
	    p->sErr(tmp);
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
	struct rng a_rng = p->gRng();

	minr = maxr = 0;
	mint = maxt = .1;  /* avoid divide by 0 in npv */
	i = 0;
	while (minr >= 0)
	{
		mint += 1;
		tmp = npv (&a_rng, mint, &minr);
		if (tmp)
		{
			p->sErr(tmp);
			return;
		}
		if (++i == 40)
		{
			p->sErr(BAD_INPUT);
			return;
		}
	}
	i = 0;
	while (maxr <= 0)
	{
		maxt -= 1;
		tmp = npv (&a_rng, maxt, &maxr);
		if (tmp)
		{
			//p->Value = tmp;
			//p->type = TYP_ERR;
			p->sErr(tmp);
			return;
		}
		if (++i == 40)
		{
			//p->Value = BAD_INPUT;
			//p->type = TYP_ERR;
			p->sErr(BAD_INPUT);
			return;
		}
	}
	try1 = (p + 1)->gFlt();
	for (i = 0;; i++)
	{
		if (i == 40)
		{
			//p->Value = BAD_INPUT;
			//p->type = TYP_ERR;
			p->sErr(BAD_INPUT);
			return;
		}
		tmp = npv (&a_rng, try1, &res);
		if (tmp)
		{
			//p->Value = tmp;
			//p->type = TYP_ERR;
			p->sErr(tmp);
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
	p->sFlt(try1);
	p->type = TYP_FLT;
}


static void
do_fv ( struct value *p)
{
	double payment = p->gFlt();
	double interest = (p + 1)->gFlt();
	double term = (p + 2)->gFlt();

	p->sFlt(payment * (pow(1 + interest, term) - 1) / interest);
}

static void
do_rate ( struct value *p)
{
	double future = p->gFlt();
	double present = (p + 1)->gFlt();
	double term = (p + 2)->gFlt();

	p->sFlt(pow (future / present, 1 / term) - 1);
}

static void
do_term ( struct value *p)
{
	double payment = p->gFlt();
	double interest = (p + 1)->gFlt();
	double future = (p + 2)->gFlt();

	p->sFlt(log (1 + future * (interest / payment)) / log (1 + interest));
}

static void
do_cterm ( struct value *p)
{
	double interest = (p)->gFlt();
	double future = (p + 1)->gFlt();
	double present = (p + 2)->gFlt();

	p->sFlt(log (future / present) / log (1 + interest));
}

static void
do_sln ( struct value *p)
{
	double cost = (p)->gFlt();
	double salvage = (p + 1)->gFlt();
	double life = (p + 2)->gFlt();

	if (life < 1)
	{
		//p->Value = OUT_OF_RANGE;
		//p->type = TYP_ERR;
		p->sErr(OUT_OF_RANGE);
		return;
	}
	p->sFlt((cost - salvage) / life);
}

static void
do_syd ( struct value *p)
{
	double cost, salvage, life, period;

	cost = p->gFlt();
	salvage = (p + 1)->gFlt();
	life = (p + 2)->gFlt();
	period = (p + 3)->gFlt();

	if (period < 1 || life < 1)
	{
		//p->Value = OUT_OF_RANGE;
		//p->type = TYP_ERR;
		p->sErr(OUT_OF_RANGE);
		return;
	}
	if (period > life)
		p->sFlt(0);
	else if (period == life)
		p->sFlt(salvage);
	else
		p->sFlt(((cost - salvage) * (life - period + 1)) / (life * ((life + 1) / 2)));
}


static void
do_ddb ( struct value *p)
{
	double cost = (p)->gFlt();
	double salvage = (p + 1)->gFlt();
	long life = (p + 2)->gInt();
	long period = (p + 3)->gInt();

	double bookval, tmp;
	long n;

	if (period < 1 || period > life || life < 1)
	{
		//p->Value = OUT_OF_RANGE;
		//p->type = TYP_ERR;
		p->sErr(OUT_OF_RANGE);
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
	p->sFlt(tmp);
}

static void
do_anrate ( struct value *p)
{
	double in_pmt = (p)->gFlt();
	double present = (p + 1)->gFlt();
	double term = (p + 2)->gFlt();

	double tr_lo, tr_hi;
	double mytry;
	double try_pmt;
	int n;

	if (in_pmt * term == present)
	{
		p->sFlt(0.0);
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
	p->sFlt(mytry);
}

static void
do_anterm ( struct value *p)
{
	double payment = (p)->gFlt();
	double principal = (p + 1)->gFlt();
	double rate = (p + 2)->gFlt();

	p->sFlt((-log (1 - principal * (rate / payment))) / log (1 + rate));
}


static void
do_balance ( struct value *p)
{
	double principal = (p)->gFlt();
	double rate = (p + 1)->gFlt();
	long term = (p + 2)->gInt();
	long period = (p + 3)->gInt();

	double tmp_pmt, int_part;
	long num;

	if (term < period)
	{
		//p->Value = OUT_OF_RANGE;
		//p->type = TYP_ERR;
		p->sErr(OUT_OF_RANGE);
		return;
	}
	tmp_pmt = pmt (principal, rate, (double) term);
	for (num = 0; num < period; num++)
	{
		int_part = rate * principal;
		if (int_part > tmp_pmt)
		{
			//p->Value = OUT_OF_RANGE;
			//p->type = TYP_ERR;
			p->sErr(OUT_OF_RANGE);
			return;
		}
		principal -= tmp_pmt - int_part;
	}
	p->sFlt(principal);
}

static void
do_paidint ( struct value *p)
{
	double principal = (p)->gFlt();
	double rate = (p + 1)->gFlt();
	long term = (p + 2)->gInt();
	long period = (p + 3)->gInt();

	double tmp_pmt, int_part, retval;
	long num;

	if (term < period)
	{
		//p->Value = OUT_OF_RANGE;
		//p->type = TYP_ERR;
		p->sErr(OUT_OF_RANGE);
		return;
	}
	tmp_pmt = pmt (principal, rate, (double) term);
	retval = 0;
	for (num = 0; num < period; num++)
	{
		int_part = rate * principal;
		if (int_part > tmp_pmt)
		{
			//p->Value = OUT_OF_RANGE;
			//p->type = TYP_ERR;
			p->sErr(OUT_OF_RANGE);
			return;
		}
		principal -= tmp_pmt - int_part;
		retval += int_part;
	}
	p->sFlt(retval);
}

static void
do_kint ( struct value *p)
{
	double principal = (p)->gFlt();
	double rate = (p + 1)->gFlt();
	long term = (p + 2)->gInt();
	long period = (p + 3)->gInt();

	double tmp_pmt, int_part = 0;
	long num;

	if (term < period)
	{
		//p->Value = OUT_OF_RANGE;
		//p->type = TYP_ERR;
		p->sErr(OUT_OF_RANGE);
		return;
	}

	tmp_pmt = pmt (principal, rate, (double) term);
	for (num = 0; num < period; num++)
	{
		int_part = rate * principal;
		if (int_part > tmp_pmt)
		{
			//p->Value = OUT_OF_RANGE;
			//p->type = TYP_ERR;
			p->sErr(OUT_OF_RANGE);
			return;
		}
		principal -= tmp_pmt - int_part;
	}
	p->sFlt(int_part);
}

static void
do_kprin ( struct value *p)
{
	double principal = (p)->gFlt();
	double rate = (p + 1)->gFlt();
	long term = (p + 2)->gInt();
	long period = (p + 3)->gInt();
	double tmp_pmt, int_part = 0;
	long num;

	if (term < period)
	{
		p->sErr(OUT_OF_RANGE);
		return;
	}

	tmp_pmt = pmt (principal, rate, (double) term);
	for (num = 0; num < period; num++)
	{
		int_part = rate * principal;
		if (int_part > tmp_pmt)
		{
			//p->Value = OUT_OF_RANGE;
			//p->type = TYP_ERR;
			p->sErr(OUT_OF_RANGE);
			return;
		}
		principal -= tmp_pmt - int_part;
	}
	p->sFlt(tmp_pmt - int_part);
}

static void
do_compbal (struct value *p)
{
	num_t principal = (p)->gFlt();
	num_t rate = (p + 1)->gFlt();
	num_t term = (p + 2)->gFlt();

	p->sFlt(principal * busi_pow (1 + rate, term));
}

static void do_ceil(struct value *p)
{
	p->sInt(ceil(p->gFlt()));
}
static void do_floor(struct value *p)
{
	p->sInt(floor(p->gFlt()));
}

static void
do_sum(struct value* p)
{
	log_debug("busi.cc:do_sum called");
	struct rng rng1 = p->gRng();
	struct rng* rng = &rng1;
	double res = 0;

	CELL* cell_ptr;
	for(auto cell_ptr: get_cells_in_range(rng)) {
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

  {C_FN1, X_A1, "R",    to_vptr(do_sum), "sum"},
  {C_FN1, X_A1, "F",    to_vptr(do_floor), "floor"},
  {C_FN1, X_A1, "F",    to_vptr(do_ceil), "ceil"},

  {0, 0, "", 0, 0},
};

int init_busi_function_count(void) 
{
        return sizeof(busi_funs) / sizeof(function_t) - 1;
}

