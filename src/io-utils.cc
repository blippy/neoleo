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

#include <format>
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
#include "io-utils.h"
#include "sheet.h"
#include "logging.h"
#include "oleofile.h"
#include "spans.h"
#include "utils.h"

import errors;
import value;

using namespace std::literals;
using std::get;
using std::string;

#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif /* RETSIGTYPE */




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


/* Turn a floating-point number into the canonical text form.  This scribbles
   on print_buf */

std::string flt_to_str (num_t val)
{
	//log("flt_to_str called");
	if (val == (num_t) __plinf)
		return iname;
	if (val == (num_t) __neinf)
		return mname;
	if (isnan(val))
		return nname;

	double f = fabs (val);
	if (f >= 1e6 || (f > 0 && f <= 9.9999e-6)) {
		return std::format("{}",(double) val);
	}
	return  pr_flt (val, &fxt, FLOAT_PRECISION, false);
}


std::string  fmt_std_date(int t)
{
	time_t t1 = t;
	struct tm *tmp = localtime(&t1);
	char str[100];
	// std::format seems to cause problems
	sprintf(str, "%04d-%02d-%02d", tmp->tm_year + 1900,tmp->tm_mon + 1,tmp->tm_mday);
	//return std::format("{:04}-{:02}-{:02}",tmp->tm_year + 1900,tmp->tm_mon + 1,tmp->tm_mday);
	return str;
}


std::string print_cell () { auto *cp = find_cell(curow, cucol);  return print_cell(cp); }
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
		return ename_desc[get<err_t>(val).num];
	}
	if (typ == TYP_FLT) {
		num_t flt = get<num_t>(val);
		switch (j)
		{

			case FMT_USR:
				panic("Uncalled FMT_USR");
				//ASSERT_UNCALLED();
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
			default:
				panic ("Unknown format %d", j);
				return "YUK";
		}
	}


	panic ("Unknown cell type");
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
		case TYP_STR:
			return cp->get_formula_text();

		case TYP_BOL:
			return bool_name(get<bool_t>(val).v);

		case TYP_ERR:
			return ename_desc[get<err_t>(val).num];
		default:
			panic ("unknown type %d in cell_value_string", typ);
	}
	return "";
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

char *adjust_prc (char *oldp, CELL * cp, int width, int smallwid, int just)
{
	int fmt;
	int prc;
	struct user_fmt suspicious; // 25/4 Very dodgy
	struct user_fmt *ufmt = &suspicious;
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
			panic("Uncalled FMT_USR");
			//ASSERT_UNCALLED();
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

std::string cell_name (CELLREF rr, CELLREF cc)
{
	return std::format("r{}c{}", rr, cc);
}


std::string range_name (struct rng *rng)
{
	CELLREF lr, lc, hr, hc;
	//static char buf[2][40];
	//static int num;
	//const char *ptr;

	//ptr = &buf[num][0];
	//num = num ? 0 : 1;

	lr = rng->lr;
	lc = rng->lc;
	hr = rng->hr;
	hc = rng->hc;

	/* Return empty string when invalid */
	if (lr == 0 || lc == 0 || hc == 0 || hr == 0 ||
			lr >= MAX_ROW || lc >= MAX_COL || hc >= MAX_COL || hr >= MAX_ROW) {
		return "";
		//ptr[0] = '\0';
		//return ptr;
	}

	if ((lr == hr) && (lc == hc)) {
		//sprintf (ptr, "%s", cell_name (lr, lc));
		return cell_name(lr, lc);
	}


	if (lr == hr && lc != hc)
		return std::format("r{}c{}:{}", lr, lc, hc);
	else if (lr != hr && lc == hc)
		return std::format("r{}:{}c{}", lr, hr, lc);

	return std::format("r{}:{}c{}:{}", lr, hr, lc, hc);

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
	if(auto v = std::get_if<err_t>(&val)) 	return ename_desc[v->num];

	panic("Uncalled stringify_value_file_style");
	//ASSERT_UNCALLED();
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
