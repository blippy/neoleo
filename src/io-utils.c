/*
 * $Id: io-utils.c,v 1.43 2011/07/05 00:16:13 delqn Exp $
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

static char *rcsid = "$Id: ";

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include "sysdef.h"
#include "io-utils.h"
#include "cell.h"
#include "ref.h"
#include "decompile.h"
#include "io-generic.h"
#include "io-abstract.h"
#include "lists.h"
#include "io-term.h"
#include "cmd.h"
#include "sylk.h"
#ifdef	HAVE_MOTIF
#include "io-motif.h"
#endif
#include "basic.h"
#include "oleofile.h"
#include "sc.h"
#include "list.h"

#ifdef	HAVE_TIME_H
#include <time.h>
#endif


/* Routines for formatting cell values */
struct user_fmt;
static char *pr_flt (double, struct user_fmt *, int);
static char *pr_int (long, struct user_fmt *, int);

/* Constants */
char *bname[] =
{
  "#FALSE", "#TRUE"
};

char numb_oflo[] = "########################################";

double __plinf;
double __neinf;
double oleo__nan;

char nname[] = "#NOT_A_NUMBER";
char iname[] = "#INFINITY";
char mname[] = "#MINUS_INFINITY";

char *date_formats[] = {
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
divide (a, b)
     double a;
     double b;
{
  return a / b;
}

static RETSIGTYPE
ignore_sig (sig)
     int sig;
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
  oleo__nan = __plinf + __neinf;
}



/* Slightly larger than the maximum exponent we ever expect to see */
#define BIGFLT 309
#ifdef TEST
static char print_buf[1024 * 8];
#else
static char print_buf[BIGFLT + 20];
#endif



/* Structures/vars/functions for dealing with formatting floating-point
   numbers, etc */

struct user_fmt {
    char *name,		/* Format name */
	*p_hdr,		/* Positive header */
	*n_hdr,		/* Negative header */
	*p_trl,		/* Positive trailer */
	*n_trl,		/* Negative trailer */
	*zero,		/* How to represent a 0 */
	*comma,		/* Thousands separator */
	*decpt;		/* Decimal point */
    unsigned char prec;	/* Precision */
    double scale;	/* Scale */
};


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

struct user_fmt fxt =
{ "fixed", 0, "-", 0, 0, "0", 0, ".", FLOAT_PRECISION, 1};

/* Variables */

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


/* Turn a floating-point number into the canonical text form.  This scribbles
   on print_buf */

char *
flt_to_str (double val)
{
  double f;

  if (val == __plinf)
    return iname;
  if (val == __neinf)
    return mname;
  f = fabs (val);
  if (f >= 1e6 || (f > 0 && f <= 9.9999e-6))
    {
      sprintf (print_buf, "%e", val);
      return print_buf;
    }
  return pr_flt (val, &fxt, FLOAT_PRECISION);
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
  double f;

  if (j == FMT_DEF)
    j = default_fmt;

  p = GET_PRECISION(cp);

  if (cp->cell_flt == __plinf)
    return iname;
  if (cp->cell_flt == __neinf)
    return mname;
  f = fabs (cp->cell_flt);
  if (f >= 1e6 || (f > 0 && f <= 9.9999e-6))
    {
      sprintf (print_buf, "%e", cp->cell_flt);
      return print_buf;
    }
  switch (j)
    {
      case FMT_FXT:
      case FMT_DOL:
      case FMT_PCT:
        return pr_flt (cp->cell_flt, &fxt, p);
      default:
        return flt_to_str (cp->cell_flt);
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
    return cp->cell_str;

  if (GET_TYP (cp) == TYP_BOL) {
#ifdef TEST
      if (cp->cell_bol < 0 || cp->cell_bol > 1)
	panic ("Bool %d out of range", cp->cell_bol);
#endif
      return bname[cp->cell_bol];
    }
  if (GET_TYP (cp) == TYP_ERR) {
#ifdef TEST
      if (cp->cell_err > ERR_MAX || cp->cell_err < 0)
	panic ("Error %d out of range", cp->cell_err);
#endif
      return ename[cp->cell_err];
    }
  if (GET_TYP (cp) == TYP_FLT) {
      switch (j)
	{
	case FMT_GPH:
	  if (cp->cell_flt < 0)
	    {
	      j = '-';
	      num = -(cp->cell_flt);
	    }
	  else if (cp->cell_flt >= 1)
	    {
	      j = '+';
	      num = (cp->cell_flt);
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
	  return pr_flt (cp->cell_flt, &u[p], u[p].prec);

	case FMT_GEN:
	  {
	    double f;

	    f = fabs (cp->cell_flt);

	    if (f >= 1e6 || (f > 0 && f <= 9.9999e-6))
	      goto handle_exp;
	    return pr_flt (cp->cell_flt, &fxt, p);
	  }

	case FMT_DOL:
	  return pr_flt (cp->cell_flt, &dol, p);

	case FMT_CMA:
	  return pr_flt (cp->cell_flt, &cma, p);

	case FMT_PCT:
	  return pr_flt (cp->cell_flt, &pct, p);

	case FMT_FXT:
	  return pr_flt (cp->cell_flt, &fxt, p);

	case FMT_EXP:
	handle_exp:
	  if (cp->cell_flt == __plinf)
	    return iname;
	  if (cp->cell_flt == __neinf)
	    return mname;
	  if (p == FLOAT_PRECISION)
	    sprintf (print_buf, "%e", cp->cell_flt);
	  else
	    sprintf (print_buf, "%.*e", p, cp->cell_flt);
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
	  if (cp->cell_int < 0)
	    {
	      j = '-';
	      num = -(cp->cell_int);
	    }
	  else if (cp->cell_int >= 1)
	    {
	      j = '+';
	      num = (cp->cell_int);
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
		    time_t t = cp->cell_int;
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
	  return pr_int (cp->cell_int, &u[p], u[p].prec);

	case FMT_GEN:
	  sprintf (print_buf, "%ld", cp->cell_int);
	  return print_buf;

	case FMT_DOL:
	  return pr_int (cp->cell_int, &dol, p);

	case FMT_CMA:
	  return pr_int (cp->cell_int, &cma, p);

	case FMT_PCT:
	  return pr_int (cp->cell_int, &pct, p);

	case FMT_FXT:
	  if (p != FLOAT_PRECISION && p != 0)
	    sprintf (print_buf, "%ld.%.*s", cp->cell_int, p, zeroes);
	  else
	    sprintf (print_buf, "%ld", cp->cell_int);
	  return print_buf;

	case FMT_EXP:
	  if (p != FLOAT_PRECISION)
	    sprintf (print_buf, "%.*e", p, (double) (cp->cell_int));
	  else
	    sprintf (print_buf, "%e", (double) (cp->cell_int));
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
char *
cell_value_string (CELLREF row, CELLREF col, int add_quote)
{
  CELL *cp;

  cp = find_cell (row, col);
  if (!cp || !GET_TYP (cp))
    return "";
  switch (GET_TYP (cp))
    {
    case TYP_FLT:
      return flt_to_str (cp->cell_flt);

    case TYP_INT:
      sprintf (print_buf, "%ld", cp->cell_int);
      return print_buf;

    case TYP_STR:
      return backslash_a_string (cp->cell_str, add_quote);

    case TYP_BOL:
      return bname[cp->cell_bol];

    case TYP_ERR:
      return ename[cp->cell_err];
#ifdef TEST
    default:
      panic ("unknown type %d in cell_value_string", GET_TYP (cp));
#endif
    }
  return 0;
}

static char *
pr_int (val, fmt, prec)
     long val;
     struct user_fmt *fmt;
     int prec;
{
  char *pf, *pff, *pt;
  long int n;
  int nn = 0;

  pt = &print_buf[sizeof (print_buf) - 1];
  *pt = '\0';

  n = fmt->scale * ((val < 0) ? -val : val);
  if (n == 0)
    return fmt->zero ? fmt->zero : "";

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

static char *
pr_flt (val, fmt, prec)
     double val;
     struct user_fmt *fmt;
     int prec;
{
  char *iptr;
  char *fptr;
  char *pptr;
  char *pf, *pff;
  double fract, integer, tmpval;
  int n;
  int isneg;
  int comlen;

  val *= fmt->scale;

  if (val == __plinf)
    return iname;
  if (val == __neinf)
    return mname;
  if (val != val)
    return nname;

  iptr = &print_buf[BIGFLT];
  fptr = &print_buf[BIGFLT];


  if (val == 0)
    return fmt->zero ? fmt->zero : "";

  if (val < 0)
    {
      isneg = 1;
      val = -val;
    }
  else
    isneg = 0;

  comlen = 0;
  if (fmt->comma && *(fmt->comma))
    for (pf = fmt->comma; *pf; comlen++, pf++)
      ;

  fract = modf (val, &integer);
  n = 0;
  do
    {
      if (iptr < &print_buf[comlen])
	return numb_oflo;
      tmpval = modf (integer / 10, &integer);
      *--iptr = '0' + (int) ((tmpval + .01) * 10);
      if (comlen && n++ == 2 && integer)
	{
	  n = 0;
	  pff = fmt->comma;
	  pf = pff + comlen;
	  do
	    *--iptr = *--pf;
	  while (pf != pff);
	}
    }
  while (integer);

  if (prec)
    {
      int p1;

      p1 = (prec == FLOAT_PRECISION) ? 15 : (prec > 0) ? prec : -prec;
      pf = fmt->decpt;
      while (pf && *pf)
	*fptr++ = *pf++;
      /* *fptr++='.'; */
      if (fract)
	{
	  do
	    {
	      fract = modf (fract * 10, &tmpval);
	      *fptr++ = '0' + (int) tmpval;
	    }
	  while (--p1 && fract);
	}
      if (prec > 0 && prec != FLOAT_PRECISION)
	while (p1--)
	  *fptr++ = '0';
      else
	{
	  fract = 0;
	  while (fptr[-1] == '0')
	    --fptr;
	  while (!isdigit (fptr[-1]))
	    --fptr;
	  *fptr = '\0';
	}
    }
  if (fract)
    {
      (void) modf (fract * 10, &tmpval);
      if (tmpval > 4)
	{
	  iptr[-1] = '0';
	  for (pptr = fptr - 1;; --pptr)
	    {
	      if (!isdigit (*pptr))
		continue;
	      else if (*pptr == '9')
		{
		  if (pptr == fptr - 1 && pptr > &print_buf[BIGFLT] && (prec < 0 || prec == FLOAT_PRECISION))
		    {
		      --fptr;
		      while (!isdigit (pptr[-1]))
			{
			  --fptr;
			  --pptr;
			}
		      *pptr = '\0';
		    }
		  else
		    *pptr = '0';
		}
	      else
		{
		  (*pptr)++;
		  break;
		}
	    }
	  if (pptr < iptr)
	    {
	      --iptr;
	      if (n == 3)
		{
		  char tmpch;

		  tmpch = *iptr++;
		  for (pf = pff = fmt->comma; *pf; pf++)
		    ;
		  do
		    *--iptr = *--pf;
		  while (pf != pff);
		  *--iptr = tmpch;
		}
	    }
	}
    }
  pf = pff = (isneg) ? fmt->n_hdr : fmt->p_hdr;
  if (pf && *pf)
    {
      while (*pf)
	pf++;
      do
	*--iptr = *--pf;
      while (pf != pff);
    }

  pf = (isneg) ? fmt->n_trl : fmt->p_trl;
  while (pf && *pf)
    *fptr++ = *pf++;
  *fptr = 0;
  return iptr;
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
      bptr = pr_flt (cp->cell_flt, ufmt, -prc);
      len = strlen (bptr);
      if (len > width && prc > 0)
	{
	  bptr = pr_flt (cp->cell_flt, ufmt, -(prc - 1));
	  len = strlen (bptr);
	}
      if (len > width)
	return numb_oflo;
      break;

    case FMT_EXP:
    handle_exp:
      {
	double f;

	f = fabs (cp->cell_flt);
	if (f > 9.99999e99 || f < 1e-99)
	  len = width - 7;
	else			/* if(f>9.9999999e9 || f<1e-9) */
	  len = width - 6;
	/* else
	   len=width-5; */
	if (cp->cell_flt < 0)
	  --len;
	if (len > 0)
	  {
	    sprintf (oldp, "%.*e", len, cp->cell_flt);
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
  u[usr_n].p_hdr = ck_malloc (len + 7);
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

  flush_everything ();
  /* flush_widths(); */
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
  default_jst = JST_LFT;
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
		defaultformat = strdup(s);
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
	} else if (!stricmp ("sylk", format)) {
		Global->sylk_a0 = 1;
		sylk_write_file(fp, rng);
	} else if (!stricmp ("sylk-noa0", format)) {
		Global->sylk_a0 = 0;
		sylk_write_file(fp, rng);
	} else if (!stricmp ("sc", format)) {
		sc_write_file(fp, rng);
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
read_file_generic_2(FILE *fp, int ismerge, char *format, char *name)
{
	if (stricmp ("oleo", format) == 0) {
		oleo_read_file(fp, ismerge);
	} else if (stricmp ("sylk", format) == 0 || stricmp ("slk", format) == 0) {
		Global->sylk_a0 = 0;	/* FIX ME */
		sylk_read_file(fp, ismerge);
	} else if (stricmp ("sylk-noa0", format) == 0) {
		Global->sylk_a0 = 0;
		sylk_read_file(fp, ismerge);
	} else if (stricmp ("sc", format) == 0) {
		sc_read_file(fp, ismerge);
	} else if (stricmp ("list", format) == 0) {
		list_read_file(fp, ismerge);
	} else if (stricmp("csv", format) == 0) {
		list_set_separator(',');
		list_read_file(fp, ismerge);
	} else if (stricmp("dbf", format) == 0) {
#if defined(HAVE_LIBXBASE) || defined(HAVE_LIBXDB)
		ReadXbaseFile(name, ismerge);
#else
		io_error_msg ("Cannot read XBASE file (xbase not compiled into " PACKAGE ")");
		return -1;
#endif
	} else {
		return -1;
	}
	return 0;
}

static struct file_formats_s {
	char	*name;
	char	*format;
} file_formats[] = {
	{ "oleo",	"oleo" },
	{ "sylk",	"[sS]*[lL][kK]" },
	{ "sc",		"sc" },
	{ "list",	"list" },
	{ "csv",	"[cC][sS][vV]" },
	{ "dbf",	"[dD][bB][fF]" },
	{ "sylk-noa0",	"sylk" },
	{ NULL,	NULL }
};

char *
file_get_format(int i)
{
	int	m = sizeof(file_formats) / sizeof(struct file_formats_s) - 1;

	if (i > m || i <= 0)
		return NULL;

	return file_formats[i-1].name;
}

char *
file_get_pattern(char *fmt)
{
	int	i, m = sizeof(file_formats) / sizeof(struct file_formats_s) - 1;

	for (i=0; i<m; i++) {
		if (stricmp(fmt, file_formats[i].name) == 0)
			return file_formats[i].format;
	}
	return NULL;
}

void
read_file_generic(FILE *fp, int ismerge, char *format, char *name)
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

void FileSetCurrentFileName(const char *s)
{
	if (Global->FileName)
		free(Global->FileName);
	Global->FileName = NULL;
	if (s)
		Global->FileName = strdup(s);

#if HAVE_MOTIF
	if (using_motif)
		MotifSetWindowName(Global->FileName);
#endif
}

char *FileGetCurrentFileName(void)
{
	return Global->FileName;
}

void FileCloseCurrentFile(void)
{
	if (Global->FileName)
		free(Global->FileName);
	Global->FileName = NULL;

#if HAVE_MOTIF
	if (using_motif)
		MotifSetWindowName("");
#endif
}

void OleoSetEncoding(char *s)
{
	extern void PrintSetEncoding(char *encoding);  /* in print.c */

	if (Global && Global->encoding)
		free(Global->encoding);
	Global->encoding = strdup(s);

	PrintSetEncoding(s);
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
