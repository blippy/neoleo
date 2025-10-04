/*
 * Copyright (c) 1990, 1992, 1993, 2001 Free Software Foundation, Inc.
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

//module;

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <filesystem>
//#include <format>

#include "cell.h"
#include "neotypes.h"
#include "oleofile.h"
#include "sheet.h"


//export module utl;

//import std;


using namespace std::literals;
using std::get;
using std::string;
namespace fs = std::filesystem;








Log m_log;

Log::Log()
{
	auto dir = std::string(std::getenv("HOME")) + "/.neoleo";
	fs::create_directories(dir);
	auto logname = dir + "/log.txt";
	m_ofs.open(logname, std::ofstream::out | std::ofstream::app);
}

void log_debug(const std::string& s)
{
	m_log.debug(s);
}

void log_debug(const char* s)
{
	m_log.debug(s);
}


void Log::debug(std::string s)
{
	m_ofs << m_count++ << " ";
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	m_ofs << std::put_time(&tm, "%Y-%m-%d %H:%M:%S ");
	m_ofs << s << std::endl;

}

Log::~Log()
{
	if(m_ofs.is_open())
		m_ofs.close();
}










//using namespace std;


const char* ValErr::what() const throw()
{
	return ename_desc[n];

}
const int ValErr::num() const throw()
{
	return n;
}

bool is_num(const value_t& val) { return std::holds_alternative<num_t>(val); }
bool is_range(const value_t& val) { return std::holds_alternative<rng_t>(val); }


// FN raise_error .
void raise_error (const char *str, ...) 
{
	va_list args;
	char buf[1000];
	va_start (args, str);
	vsprintf (buf, str, args);
	va_end(args);

	throw OleoJmp(buf);
}

void raise_error (const std::string& msg) 
{
	raise_error("%s", msg.c_str());
}
// FN-END

// FN panic .
void panic (const char *s,...)
{
	va_list iggy;

	va_start (iggy, s);
	fprintf (stderr, "%s %s:", PACKAGE_NAME, VERSION);
	vfprintf (stderr, s, iggy);
	putc ('\n', stderr);
	va_end (iggy);
	exit (2);
}
// FN-END






constexpr char* CCC(const char* str) { return const_cast<char*>(str); }

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









std::string spaces(int n)
{
	n = std::max(0, n);
	//char sa[n+1];
	char *sa = (char*) alloca(n+1);
	std::fill(sa, sa+n, ' ');
	sa[n] = '\0';
	return sa; 
}

// FN pad_left .
std::string pad_left(const std::string& s, int width)
{
	return spaces(width-s.size()) + s;
}
// FN-END

// FN pad_right .
std::string pad_right(const std::string& s, int width)
{
	return s + spaces(width-s.size());
}
// FN-END

// FN pad_centre .
std::string pad_centre(const std::string& s, int width)
{
	return pad_left(pad_right(s, width/2), width);
}
// FN-END

// FN pad_jst .
std::string pad_jst(const std::string& s, int width, enum jst j)
{
	std::string txt{s};
	switch(j) {
		case jst::def:
		case jst::rgt:
			txt = pad_left(txt, width);
			break;
		case jst::lft:
			txt = pad_right(txt, width);
			break;
		case jst::cnt:
			txt = pad_centre(txt, width);
			break;
	}
	return txt;
}
// FN-END



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


constexpr char* DOT() { return CCC("."); }

struct user_fmt dol = { CCC("dollar"), CCC("$"), CCC("$"), 0, CCC(")"), CCC("$0"), CCC(","), DOT(), FLOAT_PRECISION, 1};

struct user_fmt cma = { CCC("comma"), 0, CCC("("), 0, CCC(")"), CCC("0"), CCC(","), DOT(), FLOAT_PRECISION, 1};

struct user_fmt pct = { CCC("percent"), 0, CCC("-"), CCC("%"), CCC("%"), CCC("0%"), 0, DOT(), FLOAT_PRECISION, 100};

// issue #6/TR01: zero now uses default "D", rather than "0"
struct user_fmt fxt = { CCC("fixed"), 0, CCC("-"), 0, 0, CCC("D"), 0, DOT(), FLOAT_PRECISION, 1};



/* Slightly larger than the maximum exponent we ever expect to see */
#define BIGFLT 309
#ifdef TEST
char print_buf[1024 * 8];
#else
char print_buf[BIGFLT + 20];
#endif


/* should we use the value for zero specified by fmt zero
 * or just use the default setting?
 *
 * Pertsains to issue #6/TR01
 */

template <typename T> bool use_specified_zero_p(const T val, const char* fmt)
{
	return val == 0 && fmt && fmt[0] != 'D';
}



char* pr_flt(num_t val, struct user_fmt *fmt, int prec, bool use_prec) {
	//log_debug("pr_flt:prec:" + std::to_string(prec));
	if (isnan(val))
		return nname;
	static char buffer[BIGFLT]; // not ideal of course
	//char* buffer = print_buf;
	bool neg = val < 0;
	if (val < 0)
		val = -val;
	std::stringstream ss;
	if (fmt && fmt->comma) {
#ifndef __CYGWIN__
		ss.imbue(std::locale(""));
#endif
	}
	if (use_prec)
		ss << std::setprecision(prec) << std::fixed;
	ss << (double) val;
	std::string s = ss.str();
	if (fmt) {
		if (fmt->p_hdr && !neg)
			s = fmt->p_hdr + s;
		if (fmt->n_hdr && neg)
			s = fmt->n_hdr + s;
		if (fmt->p_trl && !neg)
			s = s + fmt->p_trl;
		if (fmt->n_trl && neg)
			s = s + fmt->n_trl;
	}

	for (size_t i = 0; i < s.size(); ++i)
		buffer[i] = s[i];
	buffer[s.size()] = '\0';
	//strcpy(print_buf, buffer);
	return buffer;
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
		//return std::format("{}",(double) val);
		return std::to_string(val);
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


/* create the human-readable version of the contents of a cell
   This scribbles on print-buf bigtime */
std::string print_cell_flt (num_t flt, unsigned int precision, unsigned int j)
{
	switch (j)
	{

		case FMT_USR:
			panic("Uncalled FMT_USR");

		case FMT_DEF:
		case FMT_GEN:
			{
				double f;
				f = fabs (flt);

				if (f >= 1e6 || (f > 0 && f <= 9.9999e-6)) goto handle_exp;
				return pr_flt (flt, &fxt, precision, false);
			}

		case FMT_DOL:
			return pr_flt (flt, &dol, precision);

		case FMT_CMA:
			return pr_flt (flt, &cma, precision);

		case FMT_PCT:
			return pr_flt (flt, &pct, precision);

		case FMT_FXT:
			return pr_flt (flt, &fxt, precision);

		case FMT_EXP:
	handle_exp:
			if ((double) flt == __plinf) return iname;
			if ((double) flt == __neinf) return mname;
			if (precision == FLOAT_PRECISION)
				sprintf (print_buf, "%e", (double) flt);
			else
				sprintf (print_buf, "%.*e", precision, (double) flt);
			return print_buf;
		default:
			panic ("Unknown format: %d", j);
			return "YUK";
	}
}


// FN fmt_value .
std::string fmt_value (value_t& val, int p, int j)
{
	if(std::holds_alternative<std::monostate>(val)) return "";
	if(std::holds_alternative<num_t>(val)) return print_cell_flt(get<num_t>(val), p, j);
	if(std::holds_alternative<std::string>(val)) return get<string>(val);
	if(std::holds_alternative<bool_t>(val)) return bool_name(get<bool_t>(val));
	if(std::holds_alternative<err_t>(val)) return ename_desc[get<err_t>(val).num];
	throw std::logic_error("Unhandled variant type in print_cell");

}
// FN-END

// FN print_cell .
std::string print_cell (CELL * cp)
{

	if (!cp) return CCC("");

	int j = GET_FORMAT (cp);

	int p = GET_PRECISION (cp);
	if (j == FMT_DEF) {
		j = default_fmt;
		p = default_prc;
	}

	if (j == FMT_HID) return "";

	value_t val = cp->get_value_2019();
	return fmt_value(val, p, j);

}

std::string print_cell () { auto *cp = find_cell(curow, cucol);  return print_cell(cp); }
// FN-END





/* Return the value of ROW,COL in a human-readable fashion
 * In particular, strings are \\ed, and if add_quote is true,
 * they will have "" around them.
 */

std::string cell_value_string (CELLREF row, CELLREF col, int add_quote)
{
	CELL* cp = find_cell (row, col);
	if(!cp) return "";

	// TODO use get_if()
	value_t val = cp->get_value_2019();
	if(std::holds_alternative<std::monostate>(val)) return "";
	if(std::holds_alternative<num_t>(val)) return flt_to_str(get<num_t>(val));
	if(std::holds_alternative<std::string>(val)) return cp->get_formula_text();
	if(std::holds_alternative<bool_t>(val)) return bool_name(get<bool_t>(val));
	if(std::holds_alternative<err_t>(val)) return ename_desc[get<err_t>(val).num];
	throw std::logic_error("Unhandled variant type in cell_value_string");

}





char *adjust_prc (char *oldp, CELL * cp, int width, int smallwid, enum jst just)
{
	int fmt;
	int prc;
	struct user_fmt suspicious; // 25/4 Very dodgy
	struct user_fmt *ufmt = &suspicious;
	char *bptr;
	char *eptr;
	int len;

	num_t flt = get<num_t>(cp->get_value_2019());

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
			bptr = pr_flt (flt, ufmt, -prc);
			len = strlen (bptr);
			if (len > width && prc > 0)
			{
				bptr = pr_flt (flt, ufmt, -(prc - 1));
				len = strlen (bptr);
			}
			if (len > width)
				return numb_oflo;
			break;

		case FMT_EXP:
handle_exp:
			{
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
		if (just == jst::rgt || just == jst::cnt)
		{
			int n;

			n = (just == jst::rgt) ? smallwid - len : (1 + smallwid - len) / 2;
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
	}

	char res[1000];
	if ((lr == hr) && (lc == hc)) {
		sprintf(res, "r%dc%d", lr, lc);
		//return std::format("r{}c{}", lr, lc);
	} else 	if (lr == hr && lc != hc) {
		sprintf(res, "r%dc%d:%d", lr, lc, hc);
		//return std::format("r{}c{}:{}", lr, lc, hc);
	} else if (lr != hr && lc == hc) {
		sprintf(res, "r%d:%dc%d", lr, hr, lc);
		//return std::format("r{}:{}c{}", lr, hr, lc);
	} else { 
		sprintf(res, "r%d:%dc%d:%d", lr, hr, lc, hc);
		//return std::format("r{}:{}c{}:{}", lr, hr, lc, hc);
	}

	return res;

}








std::string stringify_value_file_style(const value_t& val)
{
	if(std::get_if<std::monostate>(&val)) return "";
	if(auto v = std::get_if<num_t>(&val)) 	return flt_to_str(*v);
	if(auto v = std::get_if<std::string>(&val)) 	return "\""s + *v + "\""s;
	if(auto v = std::get_if<err_t>(&val)) 	return ename_desc[v->num];
	throw std::logic_error("Unhandled stringify_value_file_style value type");
}


// FN trim .
// Remove the leading and trailing spaces of a string
std::string trim(const std::string& str)
{
    if(str.length() ==0) { return str;}
    size_t first = str.find_first_not_of(" \t\r\n");
    if(first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last-first+1));
}
// FN-END


// FN to_int .
std::optional<int> to_int(const std::string& str)
{
	try {
		return stoi(str);
	} catch(...) {
		return std::nullopt;
	}
}
// FN-END


