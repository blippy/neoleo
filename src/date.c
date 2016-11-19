/*
 * $Id: date.c,v 1.7 2001/02/13 23:38:05 danny Exp $
 *
 * Copyright © 1990, 1992, 1993, 2001 Free Software Foundation, Inc.
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

#include "funcdef.h"
#include "sysdef.h"

#include "global.h"
#include "cell.h"
#include "eval.h"
#include "errors.h"

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



struct timeb;
extern time_t get_date (char *, struct timeb *);
extern time_t posixtime (char *);



/* These functions simply provide convenient syntax for expressing intervals 
 * of time.
 */

static long
dt_hms_to_time (long h, long m, long s)
{
  return ((h * 60) + m) * 60 + s;
}

static long
dt_dhms_to_time (long d, long h, long m, long s)
{
  return ((((d * 24) + h) * 60) + m) * 60 + s;
}

static long
dt_time_to_d (long t)
{
  return t / (60 * 60 * 24);
}

static long
dt_time_to_h (long t)
{
  return (t / (60 * 60)) % 24;
}

static long
dt_time_to_m (long t)
{
  return (t / 60) % 60;
}

static long
dt_time_to_s (long t)
{
  return t % 60;
}



/* These relate time values to calendar dates using localtime, gmtime, 
 * strftime, mktime, etc.
 */

/* mktime: */


static long
dt_ymd_dst (long y, long mo, long d, long dst)
{
  struct tm tm;
  tm.tm_year = y - 1900;
  tm.tm_mon = mo;
  tm.tm_mday = d;
  tm.tm_hour = 0;
  tm.tm_min = 0;
  tm.tm_sec = 0;
  tm.tm_isdst = dst;
  return mktime (&tm);
}

static long
dt_ymd (long y, long mo, long d)
{
  return dt_ymd_dst (y, mo, d, -1);
}




#define TM_ACCESS(FN,FIELD,TMFN)			\
static long						\
FN (clk)						\
     long clk;					\
{							\
  time_t t = (time_t)clk;				\
  struct tm * tm = TMFN(&t);				\
  return (long)tm->FIELD;				\
}

#define TM_ACCESS_LOCAL(FN,FIELD)  TM_ACCESS(FN, FIELD, localtime)

TM_ACCESS_LOCAL (dt_local_year, tm_year + 1900)
TM_ACCESS_LOCAL (dt_local_month, tm_mon)
TM_ACCESS_LOCAL (dt_local_date, tm_mday)
TM_ACCESS_LOCAL (dt_local_hour, tm_hour)
TM_ACCESS_LOCAL (dt_local_min, tm_min)
TM_ACCESS_LOCAL (dt_local_sec, tm_sec)
TM_ACCESS_LOCAL (dt_local_isdst, tm_isdst)
TM_ACCESS_LOCAL (dt_local_yday, tm_yday)
TM_ACCESS_LOCAL (dt_local_wday, tm_wday)


#define TM_ACCESS_GMT(FN,FIELD)  TM_ACCESS(FN, FIELD, gmtime)

TM_ACCESS_GMT (dt_gmt_year, tm_year + 1900)
TM_ACCESS_GMT (dt_gmt_month, tm_mon)
TM_ACCESS_GMT (dt_gmt_date, tm_mday)
TM_ACCESS_GMT (dt_gmt_hour, tm_hour)
TM_ACCESS_GMT (dt_gmt_min, tm_min)
TM_ACCESS_GMT (dt_gmt_sec, tm_sec)
TM_ACCESS_GMT (dt_gmt_isdst, tm_isdst)
TM_ACCESS_GMT (dt_gmt_yday, tm_yday)
TM_ACCESS_GMT (dt_gmt_wday, tm_wday)



static char *
dt_strftime (char * format, long clk)
{
  int len_used = 0;
  int len = 32;
  char * buf = (char *)ck_malloc (len);
  time_t ck = (time_t)clk;
  struct tm * tm = localtime (&ck);
  while (!len_used)
    {
      len *= 2;
      buf = (char *)ck_remalloc (buf, len);
      len_used = strftime (buf, len - 1, format, tm);
    }
  if (len - 1 > len_used)
    buf = (char *)ck_remalloc (buf, len_used + 1);
  buf [len_used] = 0;
  return buf;
}



static long
dt_get_date (char * date)
{
  return get_date (date, NULL);
}

static long
dt_posix_date (char * date)
{
  return posixtime (date);
}



static void
l_l (fn, p)
     long (*fn)();
     struct value * p;
{
  p[0].Int = fn (p->Int);
}

static void
l_lll (fn, p)
     long (*fn)();
     struct value * p;
{
  p[0].Int = fn (p[0].Int, p[1].Int, p[2].Int);
}

static void
l_llll (fn, p)
     long (*fn)();
     struct value * p;
{
  p[0].Int = fn (p[0].Int, p[1].Int, p[2].Int, p[3].Int);
}

static void
l_s (fn, p)
     long (*fn)();
     struct value * p;
{
  p[0].Int = fn (p[0].String);
  p[0].type = TYP_INT;
}



#define CALLER(FN,CALL,VIA) static void FN (p) struct value * p; { VIA(CALL,p); }

CALLER(do_hms_to_time, dt_hms_to_time, l_lll)
CALLER(do_dhms_to_time, dt_dhms_to_time, l_llll)
CALLER(do_time_to_d, dt_time_to_d, l_l)
CALLER(do_time_to_h, dt_time_to_h, l_l)
CALLER(do_time_to_m, dt_time_to_m, l_l)
CALLER(do_time_to_s, dt_time_to_s, l_l)
CALLER(do_ymd_dst, dt_ymd_dst, l_llll)
CALLER(do_ymd, dt_ymd, l_lll)
CALLER(do_local_year, dt_local_year, l_l)
CALLER(do_local_month, dt_local_month, l_l)
CALLER(do_local_date, dt_local_date, l_l)
CALLER(do_local_hour, dt_local_hour, l_l)
CALLER(do_local_min, dt_local_min, l_l)
CALLER(do_local_sec, dt_local_sec, l_l)
CALLER(do_local_isdst, dt_local_isdst, l_l)
CALLER(do_local_yday, dt_local_yday, l_l)
CALLER(do_local_wday, dt_local_wday, l_l)
CALLER(do_gmt_year, dt_gmt_year, l_l)
CALLER(do_gmt_month, dt_gmt_month, l_l)
CALLER(do_gmt_date, dt_gmt_date, l_l)
CALLER(do_gmt_hour, dt_gmt_hour, l_l)
CALLER(do_gmt_min, dt_gmt_min, l_l)
CALLER(do_gmt_sec, dt_gmt_sec, l_l)
CALLER(do_gmt_isdst, dt_gmt_isdst, l_l)
CALLER(do_gmt_yday, dt_gmt_yday, l_l)
CALLER(do_gmt_wday, dt_gmt_wday, l_l)
CALLER(do_get_date, dt_get_date, l_s)
CALLER(do_posix_date, dt_posix_date, l_s)



void
do_strftime (p)
     struct value * p;
{
  p[0].String = dt_strftime (p[0].String, p[1].Int);
}




struct function date_funs[] =
{
  {C_FN3, X_A3, "III", do_hms_to_time, "hms_to_time"},		/* 1 */
  {C_FN4, X_A4, "IIII", do_dhms_to_time, "dhms_to_time"},	/* 2 */
  {C_FN1, X_A1, "I", do_time_to_d, "time_to_d"},		/* 3 */
  {C_FN1, X_A1, "I", do_time_to_h, "time_to_h"},		/* 4 */
  {C_FN1, X_A1, "I", do_time_to_m, "time_to_m"},		/* 5 */
  {C_FN1, X_A1, "I", do_time_to_s, "time_to_s"},		/* 6 */
  {C_FN3, X_A3, "III", do_ymd, "ymd"},				/* 7 */
  {C_FN4, X_A4, "IIII", do_ymd_dst, "ymd_dst"},			/* 8 */
  {C_FN1, X_A1, "I", do_local_year, "local_year"},		/* 9 */
  {C_FN1, X_A1, "I", do_local_month, "local_month"},		/* 10 */
  {C_FN1, X_A1, "I", do_local_date, "local_date"},		/* 11 */
  {C_FN1, X_A1, "I", do_local_hour, "local_hour"},		/* 12 */
  {C_FN1, X_A1, "I", do_local_min, "local_min"},		/* 13 */
  {C_FN1, X_A1, "I", do_local_sec, "local_sec"},		/* 14 */
  {C_FN1, X_A1, "I", do_local_isdst, "local_isdst"},		/* 15 */
  {C_FN1, X_A1, "I", do_local_yday, "local_yday"},		/* 16 */
  {C_FN1, X_A1, "I", do_local_wday, "local_wday"},		/* 17 */
  {C_FN1, X_A1, "I", do_gmt_year, "gmt_year"},			/* 18 */
  {C_FN1, X_A1, "I", do_gmt_month, "gmt_month"},		/* 19 */
  {C_FN1, X_A1, "I", do_gmt_date, "gmt_date"},			/* 20 */
  {C_FN1, X_A1, "I", do_gmt_hour, "gmt_hour"},			/* 21 */
  {C_FN1, X_A1, "I", do_gmt_min, "gmt_min"},			/* 22 */
  {C_FN1, X_A1, "I", do_gmt_sec, "gmt_sec"},			/* 23 */
  {C_FN1, X_A1, "I", do_gmt_isdst, "gmt_isdst"},		/* 24 */
  {C_FN1, X_A1, "I", do_gmt_yday, "gmt_yday"},			/* 25 */
  {C_FN1, X_A1, "I", do_gmt_wday, "gmt_wday"},			/* 26 */
  {C_FN1, X_A1, "S", do_get_date, "get_date"},			/* 27 */
  {C_FN1, X_A1, "S", do_posix_date, "posix_date"},		/* 28 */
  {C_FN2, X_A2, "SI", do_strftime, "strftime"},			/* 29 */
  {0, 0, "", 0, 0}
};

int init_date_function_count(void)
{
	return sizeof(date_funs) / sizeof(struct function) - 1;
}
