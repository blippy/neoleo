/*
 * $Id: format.c,v 1.14 2001/02/13 23:38:05 danny Exp $
 *
 * Copyright © 1993, 2001 Free Software Foundation, Inc.
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include "global.h"
#include "format.h"
#include "line.h"
#include "cell.h"
#include "cmd.h"
#include "io-term.h"
#include "io-abstract.h"
#include "io-generic.h"

static char *
fmt_to_str (int format, int precision)
{
  char *ptr;
  static char buf[30];
  char nbuf[10];

  nbuf[0] = '\0';

  switch (format) {
    case FMT_USR:
      ptr = "user-";
      sprintf (nbuf, "%d", precision + 1);
      break;
    case FMT_GEN:
      ptr = "general.";
      sprintf (nbuf, "%d", precision);
      break;
    case FMT_DOL:
      ptr = "dollar.";
      sprintf (nbuf, "%d", precision);
      break;
    case FMT_CMA:
      ptr = "comma.";
      sprintf (nbuf, "%d", precision);
      break;
    case FMT_PCT:
      ptr = "percent.";
      sprintf (nbuf, "%d", precision);
      break;
    case FMT_FXT:
      if (precision == 0)
	return "integer";
#if 0
      if (format == FMT_FXT)
	return "decimal";				/* What's a decimal ? */
#endif
      ptr = "fixed.";
      sprintf (nbuf, "%d", precision);
      break;
    case FMT_EXP:
      ptr = "exponent.";
      sprintf (nbuf, "%d", precision);
      break;
    default:
      io_error_msg ("Unknown format %d (%x)", format, format);
      ptr = "UNKNOWN";
      break;
    }
  sprintf (buf, "%s%s", ptr, nbuf);
  return buf;
}

/*
 * This is a replacement for fmt_to_str() above which used to have a dirty
 * interface (mixing format and precision).
 * Actually it was called in only two places, preceeded in both cases by
 * GET_FORMAT(cp), so I contracted GET_FORMAT and fmt_to_str.
 *
 * This was one of the calls. Nice huh ?
 * p->String = fmt_to_str ((cell_ptr = find_cell (row, col)) ? GET_FORMAT (cell_ptr) : 0);
 */
char *
cell_format_string(CELL *cp)
{
	static char no_default[] = "";

	/* FIXME: Doesn't anything! */
	if (cp == NULL)
		return fmt_to_str(0, 0);
	else
		return no_default;

	
}

struct fmt
{
  int fmt;
  char **strs;
};

static char *def_names[] =	{"default", "def", "D", 0};
static char *hid_names[] =	{"hidden", "hid", "H", 0};
static char *gph_names[] =	{"graph", "gph", "*", 0};
static char *int_names[] =	{"integer", "int", "I", 0};

static struct fmt simple[] =
{
	{FMT_DEF,	def_names},
	{FMT_HID,	hid_names},
	{FMT_GPH,	gph_names},
	{FMT_FXT,	int_names},
#if 0
	{FMT_FXT,	dec_names},
#endif
	{0, 0}
};

char *gen_names[] =	{"general.",	"gen.", "G", 0};
char *dol_names[] =	{"dollar.",	"dol.", "$", 0};
char *cma_names[] =	{"comma.",	"com.", ",", 0};
char *pct_names[] =	{"percent.",	"pct.", "%", 0};
char *fxt_names[] =	{"fixed.",	"fxt.", "F", 0};
char *exp_names[] =	{"exponent.",	"exp.", "E", 0};

static struct fmt withprec[] =
{
	{FMT_GEN, gen_names},
	{FMT_DOL, dol_names},
	{FMT_CMA, cma_names},
	{FMT_PCT, pct_names},
	{FMT_FXT, fxt_names},
	{FMT_EXP, exp_names},
	{0, 0}
};

/*
 * This function still has the messy mixup between format and precision
 * that I'm trying to get rid of.
 *
 * I'm leaving it here because I don't know how to pass two variables over
 * the command loop. Look at set_region_format for the other piece of
 * this mess.
 *
 * FIX ME.
 */
int
str_to_fmt (char *ptr)
{
  struct fmt *f;
  char **strs;
  int n;
  int ret;
  char *p1, *p2;

  for (f = simple; f->strs; f++)
    {
      for (strs = f->strs; *strs; strs++)
	{
	  if (*ptr != **strs)
	    continue;
	  for (p1 = ptr, p2 = *strs; *p1 == *p2 && *p1; p1++, p2++)
	    ;
	  if (*p1 == '\0' && *p2 == '\0')
#if 0
	    return f->fmt;		/* AHAAS had a different version here */
#else
	    return ((f->fmt) << FMT_SHIFT);
#endif
	}
    }
  if (!strncmp (ptr, "user-", 5))
    {
      ptr += 5;
      n = astol (&ptr);
      if (*ptr || n < 1 || n > 16)
	return -1;
#if 0
      return n - 1 - FLOAT_PRECISION + FMT_USR;		/* AHAAS had a different version here */
#else
      return (FMT_USR << FMT_SHIFT) + n;
#endif
    }
  for (f = withprec, ret = 0; !ret && f->strs; f++)
    {
      for (strs = f->strs; *strs; strs++)
	{
	  if (*ptr != **strs)
	    continue;
	  for (p1 = ptr, p2 = *strs; *p2 && *p1 == *p2; p1++, p2++)
	    ;
	  if (!*p2)
	    {
	      ret = f->fmt;
	      ptr = p1;
	      break;
	    }
	}
    }

  if (!ret || !*ptr)
    return -1;
  if (!strcmp (ptr, "float") || !strcmp (ptr, "f"))
    {
      n = FLOAT_PRECISION;		/* AHAAS had a different version here */
    }
  else
    {
      n = astol (&ptr);
      if (*ptr || n < 0 || n > 14)
	return -1;
    }
#if 0
  return ret + n;		/* AHAAS had a different version here */
#else
  return (ret << FMT_SHIFT) + n;
#endif
}

char *
jst_to_str (int jst)
{
  if (jst == JST_DEF)
    return "default";
  if (jst == JST_LFT)
    return "left";
  if (jst == JST_RGT)
    return "right";
  if (jst == JST_CNT)
    return "center";
  return "unknown";
}

int
chr_to_jst (int chr)
{
  if (chr == 'd' || chr == 'D')
    return JST_DEF;
  if (chr == 'l' || chr == 'L')
    return JST_LFT;
  if (chr == 'r' || chr == 'R')
    return JST_RGT;
  if (chr == 'c' || chr == 'C')
    return JST_CNT;
  return -1;
}
