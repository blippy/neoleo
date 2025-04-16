/*
 * Copyright (c) 1990, 1992, 1993, 2000, 2001 Free Software Foundation, Inc.
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
#include <iomanip>
#include <locale>
#include <math.h>
#include <signal.h>
#include <sstream>
#include <cstring>
#include <string>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>


#include "basic.h"
#include "cell.h"
#include "cmd.h"
#include "convert.h"
#include "io-abstract.h"
#include "io-term.h"
#include "io-utils.h"
#include "sheet.h"
#include "logging.h"
#include "oleofile.h"
#include "ref.h"
#include "spans.h"
#include "utils.h"

using namespace std::literals;
using std::get;
using std::string;

#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif /* RETSIGTYPE */



/* Routines for formatting cell values */
static char *pr_int (long, struct user_fmt *, int);


std::string bool_name(bool b)
{
	if(b) return "#TRUE";
	else  return "#FALSE";
}

std::string bool_name(bool_t b)
{
	return bool_name(b.v);
}

char numb_oflo[] = "########################################";

double __plinf = MAXFLOAT;
double __neinf = -MAXFLOAT;
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




/* Slightly larger than the maximum exponent we ever expect to see */
#define BIGFLT 309
#ifdef TEST
char print_buf[1024 * 8];
#else
char print_buf[BIGFLT + 20];
#endif


/* Variables */

constexpr char* DOT() { return CCC("."); }

struct user_fmt dol =
{ CCC("dollar"), CCC("$"), CCC("$"), 0, CCC(")"), CCC("$0"), CCC(","), DOT(), FLOAT_PRECISION, 1};

struct user_fmt cma =
{ CCC("comma"), 0, CCC("("), 0, CCC(")"), CCC("0"), CCC(","), DOT(), FLOAT_PRECISION, 1};

struct user_fmt pct =
{ CCC("percent"), 0, CCC("-"), CCC("%"), CCC("%"), CCC("0%"), 0, DOT(), FLOAT_PRECISION, 100};

// issue #6/TR01: zero now uses default "D", rather than "0"
struct user_fmt fxt =
{ CCC("fixed"), 0, CCC("-"), 0, 0, CCC("D"), 0, DOT(), FLOAT_PRECISION, 1};




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
	if(!fmt) return CCC("");
	if(fmt[0] == 'S') fmt++; 
	return fmt;
}

/* Turn a floating-point number into the canonical text form.  This scribbles
   on print_buf */

	char *
flt_to_str (num_t val)
{
	if (val == (num_t) __plinf)
		return iname;
	if (val == (num_t) __neinf)
		return mname;
	if (isnan(val))
		return nname;

	double f = fabs (val);
	if (f >= 1e6 || (f > 0 && f <= 9.9999e-6)) {
		sprintf (print_buf, "%e", (double) val);
		return print_buf;
	}
	return  pr_flt (val, &fxt, FLOAT_PRECISION, false);
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
	if (j == FMT_DEF)
		j = default_fmt;

	int p = GET_PRECISION(cp);
	switch (j)
	{
		case FMT_FXT:
		case FMT_DOL:
		case FMT_PCT:
			return pr_flt (cp->to_num(), &fxt, p);
		default:
			return flt_to_str (cp->to_num());
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

std::string print_cell (CELL * cp)
{
	int j;
	int p;
	//long num;
	static char zeroes[] = "000000000000000";

	if (!cp)
		return CCC("");

	j = GET_FORMAT (cp);

	p = GET_PRECISION (cp);
	if (j == FMT_DEF) {
		j = default_fmt;
		p = default_prc;
	}

	value_t val = cp->get_value_2019();
	auto typ = cp->get_type();

	if (j == FMT_HID || is_nul(val))
		return CCC("");

	if (typ == TYP_STR)
		return get<string>(val);

	if (typ == TYP_BOL) {
		return bool_name(get<bool_t>(val));
	}
	if (typ == TYP_ERR) {
		return ename[get<err_t>(val).num];
	}
	if (typ == TYP_FLT) {
		num_t flt = get<num_t>(val);
		switch (j)
		{

			case FMT_USR:
				ASSERT_UNCALLED();
				//return pr_flt (flt, &u[p], u[p].prec);

			case FMT_GEN:
				{
					double f;
					f = fabs (flt);

					if (f >= 1e6 || (f > 0 && f <= 9.9999e-6))
						goto handle_exp;
					return pr_flt (flt, &fxt, p, false);
				}

			case FMT_DOL:
				return pr_flt (flt, &dol, p);

			case FMT_CMA:
				return pr_flt (flt, &cma, p);

			case FMT_PCT:
				return pr_flt (flt, &pct, p);

			case FMT_FXT:
				return pr_flt (flt, &fxt, p);

			case FMT_EXP:
handle_exp:
				if ((double) flt == __plinf)
					return iname;
				if ((double) flt == __neinf)
					return mname;
				if (p == FLOAT_PRECISION)
					sprintf (print_buf, "%e", (double) flt);
				else
					sprintf (print_buf, "%.*e", p, (double) flt);
				return print_buf;
#ifdef TEST
			default:
				panic ("Unknown format %d", j);
				return 0;
#endif
		}
	}

	if (typ == TYP_INT) {
		p = GET_PRECISION (cp);
		int v = get<num_t>(val);
		switch (j)
		{

#ifdef	FMT_DATE	/* Still depends on new style cell_flags */
			case FMT_DATE:
				{
					time_t t = v;
					//int	f = GET_PRECISION(cp);		/* Determines date format */
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
				ASSERT_UNCALLED();
				//return pr_int (v, &u[p], u[p].prec);

			case FMT_GEN:
				sprintf (print_buf, "%ld", (long) v);
				return print_buf;

			case FMT_DOL:
				return pr_int (v, &dol, p);

			case FMT_CMA:
				return pr_int (v, &cma, p);

			case FMT_PCT:
				return pr_int (v, &pct, p);

			case FMT_FXT:
				if (p != FLOAT_PRECISION && p != 0)
					sprintf (print_buf, "%ld.%.*s", (long) v, p, zeroes);
				else
					sprintf (print_buf, "%ld", (long) v);
				return print_buf;

			case FMT_EXP:
				if (p != FLOAT_PRECISION)
					sprintf (print_buf, "%.*e", p, (double) v);
				else
					sprintf (print_buf, "%e", (double) v);
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
	return "";
}

/* Return the value of ROW,COL in a human-readable fashion
 * In particular, strings are \\ed, and if add_quote is true,
 * they will have "" around them.
 */

std::string cell_value_string (CELLREF row, CELLREF col, int add_quote)
{
	CELL *cp;

	cp = find_cell (row, col);
	if(!cp) return "";
	ValType typ = cp->get_type();
	value_t val = cp->get_value_2019();
	switch (typ)
	{
		case TYP_NUL:
			return "";
		case TYP_FLT:
			return flt_to_str(get<num_t>(val));

		case TYP_INT:
			sprintf (print_buf, "%ld", (long) get<num_t>(val));
			return print_buf;

		case TYP_STR:
			return cp->get_formula_text();

		case TYP_BOL:
			return bool_name(get<bool_t>(val).v);

		case TYP_ERR:
			return ename[get<err_t>(val).num];
		default:
			panic ("unknown type %d in cell_value_string", typ);
	}
	return "";
}

	static char *
pr_int (long val, struct user_fmt *fmt, int prec)
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

	for(size_t i = 0; i< s.size(); ++i) buffer[i] = s[i];
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
			ASSERT_UNCALLED();
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
			if (bptr  + strlen(ufmt->decpt) >= width + oldp)
				prc = 0;
			else
			{
				prc = width - (strlen (ufmt->decpt) + bptr - oldp);
			}
			bptr = pr_flt (get<num_t>(cp->get_value_2019()), ufmt, -prc);
			len = strlen (bptr);
			if (len > width && prc > 0)
			{
				bptr = pr_flt (get<num_t>(cp->get_value_2019()), ufmt, -(prc - 1));
				len = strlen (bptr);
			}
			if (len > width)
				return numb_oflo;
			break;
			
		case FMT_EXP:
handle_exp:
			{
				num_t flt = get<num_t>(cp->get_value_2019());
				double f = fabs (flt);
				if (f > 9.99999e99 || f < 1e-99)
					len = width - 7;
				else			/* if(f>9.9999999e9 || f<1e-9) */
					len = width - 6;
				/* else
				   len=width-5; */
				if (flt < 0)
					--len;
				if (len > 0)
				{
					sprintf (oldp, "%.*e", len, (double) flt);
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




void clear_spreadsheet (void)
{
	flush_cols ();
	flush_spans();
	//default_width = saved_default_width;
	default_height = saved_default_height;
	default_jst = base_default_jst;
	default_fmt = FMT_GEN;
	default_lock = LCK_UNL;
	delete_all_cells();
}



char tname[] = "#TRUE";
char fname[] = "#FALSE";



	int
parray_len (char **array)
{
	int x;
	for (x = 0; array[x]; ++x);
	return x;
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
read_file_generic_2(FILE *fp, int ismerge, char *format, const char *name)
{
	if (stricmp ("oleo", format) == 0) {
		oleo_read_file(fp, ismerge);
#ifdef	HAVE_PANIC_SAVE
	} else if (stricmp ("panic", format) == 0) {
		panic_read_file(fp, ismerge);
#endif
	} else if (stricmp("dbf", format) == 0) {
		raise_error("Cannot read XBASE file (xbase not compiled)");
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


static std::string _FileName = "unnamed.oleo";

	void 
FileSetCurrentFileName(const char *s)
{
	FileCloseCurrentFile();
	if(!s) return;
	_FileName = s;
}

void FileSetCurrentFileName(const std::string& s)
{
	FileSetCurrentFileName(s.c_str());
}

std::string FileGetCurrentFileName()
{
	return _FileName;
}

void FileCloseCurrentFile(void)
{
	_FileName =  "";
}


std::string stringify_value_file_style(const value_t& val)	
{
	if(std::get_if<std::monostate>(&val)) return "";
	if(auto v = std::get_if<num_t>(&val)) 	return flt_to_str(*v);
	if(auto v = std::get_if<std::string>(&val)) 	return "\""s + *v + "\""s;
	if(auto v = std::get_if<err_t>(&val)) 	return ename[v->num];

	ASSERT_UNCALLED();
	return "BAD STRING";
}


std::string trim(const std::string& str)
{
    if(str.length() ==0) { return str;}
    size_t first = str.find_first_not_of(" \t\r\n");
    if(first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last-first+1));
}

std::string
getline_from_fildes(int fildes, bool& eof)
{
	char ch;
	std::string line;
	while(true) {
		eof = read(fildes, &ch, 1) == 0;
		if(eof) return line;

		if(ch == '\n') {
			//ofs << "line: " << line << endl;
			//exec_cmd(line, fildes);
			//line = "";
			return line;
		} else {
			line += ch;
		}
	}

	return line;
}
