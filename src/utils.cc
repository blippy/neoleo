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
#include <map>
//#include <optional>
#include <ranges>
//#include <format>

#include "cell.h"
#include "neotypes.h"
#include "oleofile.h"
#include "sheet.h"
#include "spans.h"
#include "wsht.hh"


//export module utl;

//import std;

//#define get_width the_wsht.get_width
#define wsh the_wsht

using namespace std::literals;
using std::get;
using std::string;
namespace fs = std::filesystem;


// 26/3 mmoved from io-2019.cc because we want to access it from Tcl but without including curses
static std::map<char, string> custom_bindings; // map between a char and a Tcl string that must be interpreted

// bind a character to some Tcl code requiring interpreting
void bind_char(char c, const std::string& str_to_interpret)
{
	custom_bindings[c] = str_to_interpret;
}

std::optional<std::string> get_binding(char c)
{
	if(custom_bindings.contains(c))
		return custom_bindings[c];
	return {};
}

/*
// 26/3
void slurp (FILE *fp)
{
	std::string res;
	char buf[100];
	do {
		size_t n = fread(buf, sizeof(buf), 1, fp);
		res +=
	} while(n>0);
	std::string contents{slurp(fp)};
}
*/

// 26/3
bool streq(const char* a, const char* b)
{
	return strcmp(a, b) == 0;
}


// 25/11
std::size_t strlen_utf8 (const std::string& str) {
    std::size_t length = 0;
    for (char c : str) {
        if ((c & 0xC0) != 0x80) {
            ++length;
        }
    }
    return length;
}





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





// can't go in io-2019.cc due to library compilation issues
static string status; // 26/3 what to write on the status line

// FN set_status
void set_status (const std::string& str)
{
	status = str;
}
// FN-END

std::string get_status () { return status; }

// 26/3
void test_status ()
{
	set_status("This is a status test");
}


// 26/3 created
std::string nchars(int n, char c)
{
	if(n <=0) return "";
	std::string res;
	res.resize(n, c);
	//res.reserve(n);
	//std::fill(res.begin(), res.end()-1, c);
	return res;
}

std::string spaces(int n)
{
	return nchars(n, ' ');
}

// FN pad_left .
// 26/3 added trunc
std::string pad_left(const std::string& s, int width, bool trunc)
{
	size_t len = strlen_utf8(s);
	if(trunc && len>width) {
		//log("pad_left:trunc");
		std::string s1 = s.substr(len -width, width); // take the right-most
		//s1.reserve(width);
		//for(int i = 0; i< width; i++) s1[i] += s[i+len];
		//log("pad_left:trunc str:", s1);
		return s1;
	}
	return spaces(width-len) + s;
}
// FN-END

// FN pad_right .
// 26/3 added trunc
std::string pad_right(const std::string& s, int width, bool trunc)
{
	size_t len = strlen_utf8(s);
	if(trunc && len>width) {
		//log("pad_right:trunc");
		std::string s1 = s.substr(0, width); // take the left-most
		//for(int i = 0; i< len; i++) s1 += s[i+len];
		return s1;
	}
	return s + spaces(width-len);
}
// FN-END

// FN pad_centre .
// 26/3 added trunc
std::string pad_centre(const std::string& s, int width, bool trunc)
{
	return pad_left(pad_right(s, width/2, trunc), width, trunc);
}
// FN-END

// FN pad_jst .
// 26/2 added trunc
std::string pad_jst(const std::string& s, int width, enum jst j, bool trunc)
{
	std::string txt{s};
	switch(j) {
		case jst::def:
		case jst::rgt:
			txt = pad_left(txt, width, trunc);
			break;
		case jst::lft:
			txt = pad_right(txt, width, trunc);
			break;
		case jst::cnt:
			txt = pad_centre(txt, width, trunc);
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



std::string utl_fmt_cell(CELL* cp, int col_width, int max_width)
{
	assert(max_width >= col_width);
	std::string str1 = string_cell(cp);
	auto len1 = str1.size();
	std::string res{str1};

	if(len1<col_width) {
		enum jst just = cp->get_cell_jst();
		if(is_flt(cp) && just == jst::def) just = jst::rgt;
		if(just == jst::rgt)
			res = spaces(col_width-len1) + str1;
		else if (just == jst::cnt)
			res = spaces((col_width-len1)/2) + str1;
		//else
		//	res = str1;
	} else if(len1 > max_width) {
		if(is_flt(cp)) {
			res = nchars(col_width, '#');
		} else {
			res = str1.substr(0, max_width);
		}
	}

	//log("utl_fmt_cell `", res, "'");
	return res;
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
			break;
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

	return "Impossible to reach";
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


// convert a cell to a string adjusting for width and format
std::string string_cell_formatted (CELLREF r, CELLREF c)
{
	int w = get_width(wsh, c);

	CELL *cp = find_cell(r, c);
	std::string text;
	if (cp == 0) {
		text = pad_left("", w);
	} else {
		enum jst just = cp->get_cell_jst();
		text = string_cell(cp);
		text = pad_jst(text, w, just);
	}

	return text;
}

// FN print_cell .
std::string string_cell (CELL * cp)
{

	if (!cp) return "";

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

std::string string_cell () { auto *cp = find_cell(curow, cucol);  return string_cell(cp); }

std::string string_cell (CELLREF r, CELLREF c)
{
	CELL* cp =  find_cell(r, c);
	return string_cell(cp);

}
// FN-END





/* Return the value of ROW,COL in a human-readable fashion
 * In particular, strings are \\ed, and if add_quote is true,
 * they will have "" around them.
 */

std::string cell_value_string (CELLREF row, CELLREF col, int add_quote)
{
	CELL* cp = find_cell (row, col);
	if(!cp) return "";

	value_t val = cp->get_value_2019();
	if(std::holds_alternative<std::monostate>(val)) return "";
	if(std::holds_alternative<std::string>(val)) return cp->get_formula_text();
	if(auto v = std::get_if<num_t>(&val)) 	return flt_to_str(*v);
	if(auto v = std::get_if<bool_t>(&val)) 	return bool_name(*v);
	if(auto v = std::get_if<err_t>(&val)) 	return ename_desc[v->num];

	throw std::logic_error("Unhandled variant type in cell_value_string");
}




#if 0
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
#endif


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


