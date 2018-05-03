/*
 * $Id: io-utils.c,v 1.39 2001/02/13 23:38:06 danny Exp $
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
 * You should have received a copy of the GNU General Public License along with
 * Oleo; see the file COPYING.  If not, write to the Free Software Foundation, 675
 * Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
//#include <string>
#include <iomanip>
#include <locale>
#include <math.h>
#include <signal.h>
#include <sstream>

#include <string.h>

#include <stdio.h>
#include <ctype.h>

#include "config.h"

#include "basic.h"
#include "cell.h"
#include "cmd.h"
#include "decompile.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "io-term.h"
#include "io-utils.h"
#include "list.h"
#include "sheet.h"
#include "logging.h"
#include "numeric.h"
#include "oleofile.h"
#include "ref.h"
#include "spans.h"
#include "utils.h"


/* Routines for formatting cell values */
//struct user_fmt;
static char *pr_int (long, struct user_fmt *, int);

/* Constants */
char *bname[] =
{
  "#FALSE", "#TRUE"
};

char numb_oflo[] = "########################################";

double __plinf;
double __neinf;
//double __nan;

num_t fabs(num_t x)
{
	if(x<0)
		return -x;
	else
		return x;
}


num_t neo_floor(num_t x)
{
	return (num_t) floor( (double)x);
}
char nname[] = "#NAN";
char iname[] = "#INFINITY";
char mname[] = "#MINUS_INFINITY";

const char *date_formats[] = {
	"%Y/%m/%d",			/* YYYY/MM/DD */
	"%Y-%m-%d",			/* YYYY-MM-DD */
	"%d/%m/%Y",			/* European style */
	"%m/%d/%Y",			/* American style */
	"%d/%m",
	"%Y%m",
	"%m%Y",
	"%B %d, %Y",			/* Month, day, year */
	"%d %B %Y",			/* Day, month, year */
	"%b %d, %Y",			/* Mon, day, year */
	"%d %b %Y",			/* Day, mon, year */
	NULL
};

static double
divide (
     double a,
     double b)
{
  return a / b;
}

static RETSIGTYPE
ignore_sig (int sig)
{
  (void) signal (SIGFPE, ignore_sig);
}

/* It's ok of this fails and generates signals.  In that case, 
 * the same signal will occur when evaluating formulas and a
 * (less informative) error value substituted.  Note that this 
 * should be called before init_eval.
 */
void
init_infinity (void)
{
#ifdef	MAXFLOAT
  __plinf = MAXFLOAT;
  __neinf = - MAXFLOAT;
#else
  (void) signal (SIGFPE, ignore_sig);
  __plinf = divide (1., 0.);
  (void) signal (SIGFPE, ignore_sig);
  __neinf = divide (-1., 0.);
  (void) signal (SIGFPE, ignore_sig);
#endif
  //__nan = __plinf + __neinf; // mcarter
}



/* Slightly larger than the maximum exponent we ever expect to see */
#define BIGFLT 309
#ifdef TEST
char print_buf[1024 * 8];
#else
char print_buf[BIGFLT + 20];
#endif


/* Variables */

struct user_fmt dol =
{ "dollar", "$", "($", 0, ")", "$0", ",", ".", FLOAT_PRECISION, 1};

struct user_fmt cma =
{ "comma", 0, "(", 0, ")", "0", ",", ".", FLOAT_PRECISION, 1};

struct user_fmt pct =
#if 0
{ "percent", 0, "-", "%", "%", "0%", 0, ".", 3, 100};
#else
{ "percent", 0, "-", "%", "%", "0%", 0, ".", FLOAT_PRECISION, 100};
#endif

/* issue #6/TR01: zero now uses default "D", rather than "0"*/
struct user_fmt fxt =
{ "fixed", 0, "-", 0, 0, "D", 0, ".", FLOAT_PRECISION, 1};




#define NUM_USER_FMT (16)
struct user_fmt u[NUM_USER_FMT] =
{
  {"user1", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user2", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user3", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user4", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user5", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user6", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user7", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user8", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user9", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user10", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user11", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user12", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user13", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user14", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user15", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
  {"user16", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1},
};



/* should we use the value for zero specified by fmt zero
 * or just use the default setting?
 *
 * Pertsains to issue #6/TR01
 */
template <typename T>
bool use_specified_zero_p(const T val, const char* fmt)
{
	return val == 0 && fmt && fmt[0] != 'D';
}

/* deduce the zero specifier from the fmt */
char* zero_specifier(char* fmt)
{
	if(!fmt) return "";
	if(fmt[0] == 'S') fmt++; 
	return fmt;
}

/* Turn a floating-point number into the canonical text form.  This scribbles
   on print_buf */

char *
flt_to_str (num_t val)
{
  double f;

  if (val == (num_t) __plinf)
    return iname;
  if (val == (num_t) __neinf)
    return mname;
  if (isnan(val))
	  return nname;
  f = fabs (val);
  if (f >= 1e6 || (f > 0 && f <= 9.9999e-6))
    {
      sprintf (print_buf, "%e", (double) val);
      return print_buf;
    }
  char* result = pr_flt (val, &fxt, FLOAT_PRECISION, false);
  //result[0] = 'X';
  return result;
}

/* This is used to return a formatted float for editing.
 * It is called as an alternative to flt_to_str, but
 * requires the cell array instead of the value to
 * do its work.  POSSIBLY this could be merged with
 * flt_to_str, if the cell arraw cp is available wherever
 * flt_to_str is used.  For now, though, this does the job.
 */

char *
flt_to_str_fmt (CELL *cp)
{
	int j = GET_FORMAT(cp);	/* Only format, not precision */
	int p;

	if (j == FMT_DEF)
		j = default_fmt;

	p = GET_PRECISION(cp);

	if(false) { 
		/* TODO suppressed for now */
		double f;
		if ((double) cp->gFlt() == __plinf)
			return iname;
		if ((double) cp->gFlt() == __neinf)
			return mname;
		f = fabs ( (double) cp->gFlt());
		if (f >= 1e6 || (f > 0 && f <= 9.9999e-6))
		{
			sprintf (print_buf, "%e", (double) cp->gFlt());
			return print_buf;
		}
	}

	switch (j)
	{
		case FMT_FXT:
		case FMT_DOL:
		case FMT_PCT:
			return pr_flt (cp->gFlt(), &fxt, p);
		default:
			return flt_to_str (cp->gFlt());
	}
}

char *
long_to_str (long val)
{
  sprintf (print_buf, "%ld", val);
  return print_buf;
}

/* create the human-readable version of the contents of a cell
   This scribbles on print-buf bigtime */

char *
print_cell (CELL * cp)
{
  int j;
  int p;
  long num;
  static char zeroes[] = "000000000000000";

  if (!cp)
    return "";

  j = GET_FORMAT (cp);

  p = GET_PRECISION (cp);
  if (j == FMT_DEF) {
    j = default_fmt;
    p = default_prc;
  }
  if (j == FMT_HID || GET_TYP (cp) == 0)
    return "";

  if (GET_TYP (cp) == TYP_STR)
    return cp->gString();

  if (GET_TYP (cp) == TYP_BOL) {
#ifdef TEST
      if (cp->cell_bol < 0 || cp->cell_bol > 1)
	panic ("Bool %d out of range", cp->cell_bol);
#endif
      return bname[cp->gBol()];
    }
  if (GET_TYP (cp) == TYP_ERR) {
#ifdef TEST
      if (cp->gErr() > ERR_MAX || cp->gErr() < 0)
	panic ("Error %d out of range", cp->gErr());
#endif
      return ename[cp->gErr()];
    }
  if (GET_TYP (cp) == TYP_FLT) {
      switch (j)
	{
	case FMT_GPH:
	  if (cp->gFlt() < 0)
	    {
	      j = '-';
	      num = -(cp->gFlt());
	    }
	  else if (cp->gFlt() >= 1)
	    {
	      j = '+';
	      num = (cp->gFlt());
	    }
	  else
	    {
	      j = '0';
	      num = 1;
	    }
	graph:
	  if (num >= sizeof (print_buf))
	    {
	      io_error_msg ("Cannot graph %d '%c'", p, j);
	      num = sizeof (print_buf) - 1;
	    }
	  print_buf[num] = '\0';
	  while (--num >= 0)
	    print_buf[num] = j;
	  return print_buf;

	case FMT_USR:
	  return pr_flt (cp->gFlt(), &u[p], u[p].prec);

	case FMT_GEN:
	  {
	    double f;

	    f = fabs (cp->gFlt());

	    if (f >= 1e6 || (f > 0 && f <= 9.9999e-6))
	      goto handle_exp;
	    return pr_flt (cp->gFlt(), &fxt, p, false);
	  }

	case FMT_DOL:
	  return pr_flt (cp->gFlt(), &dol, p);

	case FMT_CMA:
	  return pr_flt (cp->gFlt(), &cma, p);

	case FMT_PCT:
	  return pr_flt (cp->gFlt(), &pct, p);

	case FMT_FXT:
	  return pr_flt (cp->gFlt(), &fxt, p);

	case FMT_EXP:
	handle_exp:
	  if ((double) cp->gFlt() == __plinf)
	    return iname;
	  if ((double) cp->gFlt() == __neinf)
	    return mname;
	  if (p == FLOAT_PRECISION)
	    sprintf (print_buf, "%e", (double) cp->gFlt());
	  else
	    sprintf (print_buf, "%.*e", p, (double) cp->gFlt());
	  return print_buf;
#ifdef TEST
	default:
	  panic ("Unknown format %d", j);
	  return 0;
#endif
	}
    }

  if (GET_TYP (cp) == TYP_INT) {
      p = GET_PRECISION (cp);
      switch (j)
	{
	case FMT_GPH:
	  if (cp->gInt() < 0)
	    {
	      j = '-';
	      num = -(cp->gInt());
	    }
	  else if (cp->gInt() >= 1)
	    {
	      j = '+';
	      num = (cp->gInt());
	    }
	  else
	    {
	      j = '0';
	      num = 1;
	    }
	  goto graph;

#ifdef	FMT_DATE	/* Still depends on new style cell_flags */
	case FMT_DATE:
		{
		    time_t t = cp->gInt();
		    int	f = GET_PRECISION(cp);		/* Determines date format */
		    struct tm *tmp = localtime(&t);

#ifdef	HAVE_STRFTIME
		    (void)strftime(print_buf, sizeof(print_buf),
			date_formats[f], tmp);
#else
		    sprintf(print_buf,
			"%04d/%02d/%02d",
			tmp->tm_year + 1900,
			tmp->tm_mon + 1,
			tmp->tm_mday);
#endif
		    return print_buf;
		}
#endif

	case FMT_USR:
	  return pr_int (cp->gInt(), &u[p], u[p].prec);

	case FMT_GEN:
	  sprintf (print_buf, "%ld", cp->gInt());
	  return print_buf;

	case FMT_DOL:
	  return pr_int (cp->gInt(), &dol, p);

	case FMT_CMA:
	  return pr_int (cp->gInt(), &cma, p);

	case FMT_PCT:
	  return pr_int (cp->gInt(), &pct, p);

	case FMT_FXT:
	  if (p != FLOAT_PRECISION && p != 0)
	    sprintf (print_buf, "%ld.%.*s", cp->gInt(), p, zeroes);
	  else
	    sprintf (print_buf, "%ld", cp->gInt());
	  return print_buf;

	case FMT_EXP:
	  if (p != FLOAT_PRECISION)
	    sprintf (print_buf, "%.*e", p, (double) (cp->gInt()));
	  else
	    sprintf (print_buf, "%e", (double) (cp->gInt()));
	  return print_buf;
#ifdef TEST
	default:
	  panic ("Unknown format %d", j);
	  return 0;
#endif
	}
    }
#ifdef TEST
  panic ("Unknown cell type %d", GET_TYP (cp));
#endif
  return 0;
}

/* Return the value of ROW,COL in a human-readable fashion
 * In particular, strings are \\ed, and if add_quote is true,
 * they will have "" around them.
 */
const char *
cell_value_string (CELLREF row, CELLREF col, int add_quote)
{
  CELL *cp;

  cp = find_cell (row, col);
  if (!cp || !GET_TYP (cp))
    return "";
  switch (GET_TYP (cp))
    {
    case TYP_FLT:
      return flt_to_str (cp->gFlt());

    case TYP_INT:
      sprintf (print_buf, "%ld", cp->gInt());
      return print_buf;

    case TYP_STR:
      return backslash_a_string (cp->gString(), add_quote);

    case TYP_BOL:
      return bname[cp->gBol()];

    case TYP_ERR:
      return ename[cp->gErr()];
#ifdef TEST
    default:
      panic ("unknown type %d in cell_value_string", GET_TYP (cp));
#endif
    }
  return 0;
}

static char *
pr_int (
     long val,
     struct user_fmt *fmt,
     int prec)
{
  char *pf, *pff, *pt;
  long int n;
  int nn = 0;

  pt = &print_buf[sizeof (print_buf) - 1];
  *pt = '\0';

  n = fmt->scale * ((val < 0) ? -val : val);
  if (n == 0)
    return fmt->zero ? fmt->zero : (char*) "";

  pf = pff = (val < 0) ? fmt->n_trl : fmt->p_trl;
  if (pf && *pf)
    {
      while (*pf)
	pf++;
      do
	*--pt = *--pf;
      while (pf != pff);
    }

  if (prec != FLOAT_PRECISION && prec != 0)
    {
      while (prec-- > 0)
	*--pt = '0';
      pf = pff = fmt->decpt;
      if (pf)
	{
	  while (*pf)
	    pf++;
	  do
	    *--pt = *--pf;
	  while (pf != pff);
	}
      /* *--pt='.'; */
    }
  do
    {
      *--pt = (n % 10) + '0';
      n /= 10;
      if (nn++ == 2 && n > 0)
	{
	  if (fmt->comma && *(fmt->comma))
	    {
	      for (pf = pff = fmt->comma; *pf; pf++)
		;
	      do
		*--pt = *--pf;
	      while (pf != pff);
	    }
	  nn = 0;
	}
    }
  while (n > 0);

  pf = pff = (val < 0) ? fmt->n_hdr : fmt->p_hdr;
  if (pf && *pf)
    {
      while (*pf)
	pf++;
      do
	*--pt = *--pf;
      while (pf != pff);
    }
  return pt;
}

num_t
modn(num_t x, num_t *iptr)
{
	num_t sgn = 1;
	num_t x1 = x;
	if(x1 <0) { sgn = -1;}
	x1 *= sgn;

	num_t x2 = neo_floor(x1);
	//*iptr = sgn * (x1-x2);
	//return sgn * x2;
	*iptr = sgn * x2;
	return sgn * (x1-x2);
}

char *
pr_flt (num_t val, struct user_fmt *fmt, int prec, bool use_prec)
{
	//log_debug("pr_flt:prec:" + std::to_string(prec));
	if(isnan(val)) return nname;
	static char buffer[BIGFLT]; // not ideal of course
	//char* buffer = print_buf;
	bool neg = val < 0;
	if(val<0) val = -val;
	std::stringstream ss;
	if(fmt && fmt->comma) {
#ifndef __CYGWIN__
		ss.imbue(std::locale(""));
#endif
	}
	if(use_prec) ss << std::setprecision(prec) <<  std::fixed;
	ss << (double)val;
	std::string s = ss.str();
	if(fmt) {
		if(fmt->p_hdr && !neg) s = fmt->p_hdr + s;
		if(fmt->n_hdr && neg)  s = fmt->n_hdr + s;
		if(fmt->p_trl && !neg) s = s + fmt->p_trl;
		if(fmt->n_trl && neg) s = s + fmt->n_trl;
	}

	for(int i = 0; i< s.size(); ++i) buffer[i] = s[i];
	buffer[s.size()] = '\0';
	//strcpy(print_buf, buffer);
	return buffer;
}

char *
adjust_prc (char *oldp, CELL * cp, int width, int smallwid, int just)
{
  int fmt;
  int prc;
  struct user_fmt *ufmt;
  char *bptr;
  char *eptr;
  int len;

  fmt = GET_FORMAT (cp);
  if (fmt == FMT_DEF)
    fmt = default_fmt;
  prc = GET_PRECISION (cp);
  switch (fmt)
    {
    case FMT_GPH:
    case FMT_HID:
      return numb_oflo;
    case FMT_DOL:
      ufmt = &dol;
      goto deal_fmt;

    case FMT_CMA:
      ufmt = &cma;
      goto deal_fmt;

    case FMT_PCT:
      ufmt = &cma;
      goto deal_fmt;

    case FMT_FXT:
      ufmt = &fxt;
      goto deal_fmt;

    case FMT_USR:
      ufmt = &u[prc];
      prc = ufmt->prec;
      goto deal_fmt;

    case FMT_GEN:
      if (prc != FLOAT_PRECISION)
	return numb_oflo;
      if (index (oldp, 'e') || !index (oldp, '.'))
	goto handle_exp;

      ufmt = &fxt;
      prc = FLOAT_PRECISION;
      goto deal_fmt;

    deal_fmt:
      if (prc != FLOAT_PRECISION)
	return numb_oflo;
      len = strlen (oldp);
      bptr = (char *) strstr (oldp, ufmt->decpt);
      if (!bptr)
	return numb_oflo;
      while ((eptr = (char *) strstr (bptr + 1, ufmt->decpt)))
	bptr = eptr;

      if (width < bptr - oldp)
	return numb_oflo;
      if (bptr - oldp + strlen (ufmt->decpt) >= width)
	prc = 0;
      else
	{
	  prc = width - (strlen (ufmt->decpt) + bptr - oldp);
	}
      bptr = pr_flt (cp->gFlt(), ufmt, -prc);
      len = strlen (bptr);
      if (len > width && prc > 0)
	{
	  bptr = pr_flt (cp->gFlt(), ufmt, -(prc - 1));
	  len = strlen (bptr);
	}
      if (len > width)
	return numb_oflo;
      break;

    case FMT_EXP:
    handle_exp:
      {
	double f;

	f = fabs (cp->gFlt());
	if (f > 9.99999e99 || f < 1e-99)
	  len = width - 7;
	else			/* if(f>9.9999999e9 || f<1e-9) */
	  len = width - 6;
	/* else
	   len=width-5; */
	if (cp->gFlt() < 0)
	  --len;
	if (len > 0)
	  {
	    sprintf (oldp, "%.*e", len, (double) cp->gFlt());
	    len = strlen (oldp);
	    if (len <= width)
	      {
		bptr = oldp;
		break;
	      }
	  }
      }
      return numb_oflo;
    default:
      bptr = 0;
      len = 0;
#ifdef TEST
      panic ("Unknown format %d in adjust_prc()", fmt);
#endif
      break;
    }

  /* If we get here, bptr points to a a string of len characters
     (len<=width) that we want to output */
  if (len < smallwid)
    {
      if (just == JST_RGT || just == JST_CNT)
	{
	  int n;

	  n = (just == JST_RGT) ? smallwid - len : (1 + smallwid - len) / 2;
	  for (;;)
	    {
	      bptr[len + n] = bptr[len];
	      if (len-- == 0)
		break;
	    }
	  while (n-- >= 0)
	    bptr[n] = ' ';
	}
    }
  return bptr;
}


void
set_usr_stats (int usr_n, char **usr_buf)
{
  int len;
  int i;
  char *p_in, *p_out;

  len = 0;
  for (i = 0; i < 7; i++)
    len += strlen (usr_buf[i]);
  u[usr_n].p_hdr = (char*) ck_malloc (len + 7);
  p_out = u[usr_n].p_hdr;
  if (usr_buf[0][0])
    {
      p_in = usr_buf[0];
      while ((*p_out++ = *p_in++))
	;
    }
  else
    *p_out++ = '\0';

  if (usr_buf[1][0])
    {
      p_in = usr_buf[1];
      u[usr_n].n_hdr = p_out;
      while ((*p_out++ = *p_in++))
	;
    }
  else
    u[usr_n].n_hdr = 0;

  if (usr_buf[2][0])
    {
      p_in = usr_buf[2];
      u[usr_n].p_trl = p_out;
      while ((*p_out++ = *p_in++))
	;
    }
  else
    u[usr_n].p_trl = 0;

  if (usr_buf[3][0])
    {
      p_in = usr_buf[3];
      u[usr_n].n_trl = p_out;
      while ((*p_out++ = *p_in++))
	;
    }
  else
    u[usr_n].n_trl = 0;

  if (usr_buf[4][0])
    {
      p_in = usr_buf[4];
      u[usr_n].zero = p_out;
      while ((*p_out++ = *p_in++))
	;
    }
  else
    u[usr_n].zero = 0;

  if (usr_buf[5][0])
    {
      p_in = usr_buf[5];
      u[usr_n].comma = p_out;
      while ((*p_out++ = *p_in++))
	;
    }
  else
    u[usr_n].comma = 0;

  if (usr_buf[6][0])
    {
      p_in = usr_buf[6];
      u[usr_n].decpt = p_out;
      while ((*p_out++ = *p_in++))
	;
    }
  else
    u[usr_n].decpt = 0;

  if (!stricmp (usr_buf[7], "float") || !stricmp (usr_buf[7], "f"))
    u[usr_n].prec = 15;
  else
    u[usr_n].prec = astol (&usr_buf[7]);

  u[usr_n].scale = astof (&usr_buf[8]);
}

int
usr_set_fmts (void)
{
  int n;
  int ret = 0;

  for (n = 0; n < NUM_USER_FMT; n++)
    if (u[n].p_hdr)
      ret |= 1 << n;
  return ret;
}

void
get_usr_stats (int usr_num, char **usr_buf)
{
  static char buf1[30];
  static char buf2[30];
  static char NullStr[] = "";

  usr_buf[0] = u[usr_num].p_hdr ? u[usr_num].p_hdr : NullStr;
  usr_buf[1] = u[usr_num].n_hdr ? u[usr_num].n_hdr : NullStr;
  usr_buf[2] = u[usr_num].p_trl ? u[usr_num].p_trl : NullStr;
  usr_buf[3] = u[usr_num].n_trl ? u[usr_num].n_trl : NullStr;
  usr_buf[4] = u[usr_num].zero ? u[usr_num].zero : NullStr;
  usr_buf[5] = u[usr_num].comma ? u[usr_num].comma : NullStr;
  usr_buf[6] = u[usr_num].decpt ? u[usr_num].decpt : NullStr;
  if (u[usr_num].prec == 15)
    usr_buf[7] = "float";
  else
    {
      sprintf (buf1, "%u", u[usr_num].prec);
      usr_buf[7] = buf1;
    }
  sprintf (buf2, "%.12g", u[usr_num].scale);
  usr_buf[8] = buf2;
}

/* Functions for printing out the names of cells and ranges */

char *
cell_name (CELLREF rr, CELLREF cc)
{
  static char strs[2][20];
  static int num = 0;
  char *ptr;

  num = num ? 0 : 1;

  if (Global->a0)
    {
      ptr = &strs[num][9];
      sprintf (ptr, "%u", rr);
      if (cc < MIN_COL + 26)
	*--ptr = 'A' - MIN_COL + cc;
#if MAX_COL>702
      else if (cc < MIN_COL + 702)
	{
	  cc -= MIN_COL + 26;
	  *--ptr = 'A' + cc % 26;
	  *--ptr = 'A' + cc / 26;
	}
      else if (cc < MIN_COL + 18278)
	{
	  cc -= MIN_COL + 702;
	  *--ptr = 'A' + cc % 26;
	  cc /= 26;
	  *--ptr = 'A' + cc % 26;
	  *--ptr = 'A' + cc / 26;
	}
      else
	{
	  cc -= MIN_COL + 18278;
	  *--ptr = 'A' + cc % 26;
	  cc /= 26;
	  *--ptr = 'A' + cc % 26;
	  cc /= 26;
	  *--ptr = 'A' + cc % 26;
	  *--ptr = 'A' + cc / 26;
	}
#else
      else
	{
	  cc -= MIN_COL + 26;
	  *--ptr = 'A' + cc % 26;
	  *--ptr = 'A' + cc / 26;
	}
#endif
    }
  else
    {
      ptr = &strs[num][0];
      sprintf (ptr, "r%uc%u", rr, cc);
    }
  return ptr;
}

char *
range_name (struct rng *rng)
{
  CELLREF lr, lc, hr, hc;
  static char buf[2][40];
  static int num;
  char *ptr;

  ptr = &buf[num][0];
  num = num ? 0 : 1;

  lr = rng->lr;
  lc = rng->lc;
  hr = rng->hr;
  hc = rng->hc;

  /* Return empty string when invalid */
  if (lr == 0 || lc == 0 || hc == 0 || hr == 0 ||
      lr >= MAX_ROW || lc >= MAX_COL || hc >= MAX_COL || hr >= MAX_ROW) {
	ptr[0] = '\0';
	return ptr;
  }

  if ((lr == hr) && (lc == hc)) {
      sprintf (ptr, "%s", cell_name (lr, lc));
  } else {
      if (Global->a0)
        sprintf (ptr, "%s:%s", cell_name (lr, lc), cell_name (hr, hc));
      else {
          if (lr == hr && lc != hc)
            sprintf (ptr, "r%uc%u:%u", lr, lc, hc);
          else if (lr != hr && lc == hc)
            sprintf (ptr, "r%u:%uc%u", lr, hr, lc);
          else
            sprintf (ptr, "r%u:%uc%u:%u", lr, hr, lc, hc);
      }
  }
  return ptr;
}


/* Parse a range, allowing variable names.
 * Return 1 on failure, 0 on succes.
 */
int
get_abs_rng (char **pptr, struct rng *retp)
{
  unsigned char n;
  struct rng ignored;

  if (!retp)
    retp = &ignored;

  while (**pptr == ' ')
    (*pptr)++;
  if (!**pptr)
    return 1;
  cur_row = curow;
  cur_col = cucol;
  n = parse_cell_or_range (pptr, retp);
  if (!n)
    {
      struct var *v;
      char *ptr;

      ptr = *pptr;
      while (ptr[n] && ptr[n] != ' ')
	n++;
      v = find_var (ptr, n);
      if (!v)
	return 1;
      (*pptr) += n;
      *retp = v->v_rng;
    }
  return 0;
}


char *
col_to_str (CELLREF col)
{
  static char strs[2][10];
  static int num;
  char *ptr;

  ptr = &strs[num][9];
  num = num ? 0 : 1;

  if (col < MIN_COL + 26)
    *--ptr = 'A' - MIN_COL + col;
#if MAX_COL>702
  else if (col < MIN_COL + 702)
    {
      col -= MIN_COL + 26;
      *--ptr = 'A' + col % 26;
      *--ptr = 'A' + col / 26;
    }
  else if (col < MIN_COL + 18278)
    {
      col -= MIN_COL + 702;
      *--ptr = 'A' + col % 26;
      col /= 26;
      *--ptr = 'A' + col % 26;
      *--ptr = 'A' + col / 26;
    }
  else
    {
      col -= MIN_COL + 18278;
      *--ptr = 'A' + col % 26;
      col /= 26;
      *--ptr = 'A' + col % 26;
      col /= 26;
      *--ptr = 'A' + col % 26;
      *--ptr = 'A' + col / 26;
    }
#else
  else
    {
      col -= MIN_COL + 26;
      *--ptr = 'A' + col % 26;
      *--ptr = 'A' + col / 26;
    }
#endif
  return ptr;
}

void
clear_spreadsheet (void)
{
	int n;

	flush_cols ();
	flush_variables ();
	flush_spans();
	flush_all_timers ();
	for (n = 0; n < NUM_USER_FMT; n++)
	{
		if (u[n].p_hdr)
		{
			free (u[n].p_hdr);
			u[n].p_hdr = 0;
			u[n].prec = FLOAT_PRECISION;
			u[n].scale = 1;
		}
	}
	default_width = saved_default_width;
	default_height = saved_default_height;
	default_jst = base_default_jst;
	default_fmt = FMT_GEN;
	default_lock = LCK_UNL;
}

char *ename[] =
{
  "#WHAT?",
  "#ERROR", "#BAD_INPUT", "#NON_NUMBER", "#NON_STRING",
  "#NON_BOOL", "#NON_RANGE", "#OUT_OF_RANGE", "#NO_VALUES",
  "#DIV_BY_ZERO", "#BAD_NAME", "#NOT_AVAIL", "#PARSE_ERROR",
  "#NEED_OPEN", "#NEED_CLOSE", "#NEED_QUOTE", "#UNK_CHAR",
  "#UNK_FUNC",
  0
};

char tname[] = "#TRUE";
char fname[] = "#FALSE";


int
words_imatch (char **ptr, char *key)
{
  char *str = *ptr;

  while (isspace (*str))
    ++str;
  while (isspace (*key))
    ++key;

  while (1)
    {
      while (*str && *key && (toupper (*str) == toupper (*key)))
	{
	  ++str;
	  ++key;
	}
      /* If we're not at word breaks in both strings... */
      if (!((!*str || isspace (*str)) && (!*key || isspace (*key))))
	return 0;
      else
	{
	  while (isspace (*key))
	    ++key;
	  while (isspace (*str))
	    ++str;
	  if (!*key)
	    {
	      *ptr = str;
	      return 1;
	    }
	}
    }
}

int
parray_len (char **array)
{
  int x;
  for (x = 0; array[x]; ++x);
  return x;
}


/* Return the index of CHECK in KEYS or -1.  Case and whitespeace insenstive.
 */

int
words_member (char **keys, int len, char *check)
{
  int x;
  for (x = 0; x < len; ++x)
    {
      char *ch = check;
      if (words_imatch (&ch, keys[x]))
	if (!*ch)
	  return x;
    }
  return -1;
}

int
prompt_len (char *prompt)
{
  char *pos;
  if (!prompt)
    return 0;
  for (pos = prompt; *pos && (*pos != '\n'); ++pos)
    ;
  return pos - prompt;
}


int
says_default (char *str)
{
  char *key = "ault";
  if (strincmp (str, "def", 3))
    return 0;
  str += 3;
  while (*str && *key)
    {
      int c = isupper (*str) ? tolower (*str) : *str;
      if (c != *key)
	return 0;
      ++key;
      ++str;
    }
  while (isspace (*str))
    ++str;
  return !*str;
}

static char *defaultformat = NULL;

void
file_set_default_format(char *s)
{
	if (defaultformat)
		free(defaultformat);

	if (s == NULL)
		defaultformat = strdup("oleo");
	else
		defaultformat = s;
}

char *
file_get_default_format(void)
{
	return defaultformat;
}

int
write_file_generic_2(FILE *fp, struct rng *rng, char *format)
{
	if (!stricmp ("oleo", format)) {
		oleo_write_file(fp, rng);
#ifdef	HAVE_PANIC_SAVE
	} else if (!stricmp ("panic", format)) {
		panic_write_file(fp, rng);
#endif
	} else if (!stricmp ("list", format)) {
		list_write_file(fp, rng);
	} else {
		return -1;
	}

	return 0;
}

void
write_file_generic(FILE *fp, struct rng *rng, char *format)
{
	if (format == NULL) {
		if (defaultformat)
			(void) write_file_generic_2(fp, rng, defaultformat);
		else
			oleo_write_file(fp, rng);

		return;
	}
#if 0
	fprintf(stderr, PACKAGE " write_file_generic : format %s\n", format);
#endif
	if (write_file_generic_2(fp, rng, format) != 0) {
		if (write_file_generic_2(fp, rng, defaultformat) != 0)
			oleo_write_file(fp, rng);
	}
}

int
read_file_generic_2(FILE *fp, int ismerge, char *format, const char *name)
{
	if (stricmp ("oleo", format) == 0) {
		oleo_read_file(fp, ismerge);
#ifdef	HAVE_PANIC_SAVE
	} else if (stricmp ("panic", format) == 0) {
		panic_read_file(fp, ismerge);
#endif
	} else if (stricmp ("list", format) == 0) {
		list_read_file(fp, ismerge);
	} else if (stricmp("csv", format) == 0) {
		list_set_separator(',');
		list_read_file(fp, ismerge);
	} else if (stricmp("dbf", format) == 0) {
		io_error_msg ("Cannot read XBASE file (xbase not compiled into " PACKAGE ")");
		return -1;
	} else {
		return -1;
	}
	return 0;
}


void
read_file_generic(FILE *fp, int ismerge, char *format, const char *name)
{
	if (format == NULL) {
		if (defaultformat)
			(void) read_file_generic_2(fp, ismerge, defaultformat, name);
		else
			oleo_read_file(fp, ismerge);

		return;
	}
#if 0
	fprintf(stderr, PACKAGE " read_file_generic : format %s\n", format);
#endif
	if (read_file_generic_2(fp, ismerge, format, name) != 0) {
		if (defaultformat && read_file_generic_2(fp, ismerge, defaultformat, name) != 0)
			oleo_read_file(fp, ismerge);
	}

	recalculate(1);
}

void
fmt_set_format()
{
}

void
fmt_get_format()
{
}



static std::string _FileName = "unnamed.oleo";

void 
FileSetCurrentFileName(const char *s)
{
	FileCloseCurrentFile();
	if(!s) return;
	//Global->FileName = strdup(s);
	_FileName = s;
}

std::string FileGetCurrentFileName()
{
	return _FileName;
}

void FileCloseCurrentFile(void)
{
	/*
	if (Global->FileName)
		free(Global->FileName);
	Global->FileName = NULL;
	*/
	_FileName =  "";
}

void OleoSetEncoding(const char *s)
{
	extern void PrintSetEncoding(const char *encoding);  /* in print.c */

	if (Global && Global->encoding)
		free(Global->encoding);
	Global->encoding = strdup(s);

	//PrintSetEncoding(s);
}

char *OleoGetEncoding(void)
{
	return Global->encoding;
}

void OleoUserPrefEncoding(char *s)
{
	char	*p = s + 9;	/* Get past "encoding" */

	for (; *p && isspace(*p); p++) ;
	OleoSetEncoding(p);
}
