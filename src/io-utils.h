#pragma once
/*
 * Copyright (c) 1992, 1999 Free Software Foundation, Inc.
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

#include <memory>
#include <string>

#include "cell.h"

extern char numb_oflo[];


char * adjust_prc (char *oldp, CELL *cp, int width, int smallwid, int just);
std::string cell_value_string (CELLREF row, CELLREF col, int add_quote);
std::string print_cell (CELL *cp);
std::string print_cell ();
std::string FileGetCurrentFileName();
int 		words_imatch (char ** ptr, char * key);
void 		FileCloseCurrentFile(void);
void 		clear_spreadsheet (void);
void 		FileSetCurrentFileName(const char *s);
void 		FileSetCurrentFileName(const std::string& s);
void 		write_file_generic(FILE *, struct rng *, char *);
void 		read_file_generic(FILE *, char *, const char *);

std::string  fmt_std_date(int t);

std::string stringify_value_file_style(const value_t& val);
std::string range_name (struct rng *);


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


extern struct user_fmt dol;
extern struct user_fmt cma;
extern struct user_fmt pct;
extern struct user_fmt fxt;

#define NUM_USER_FMT (16)
constexpr char* CCC(const char* str) { return const_cast<char*>(str); }

char* pr_flt (num_t val, struct user_fmt *fmt, int prec, bool use_prec = true);
std::string flt_to_str (num_t val);
std::string trim(const std::string& str);

#define	OLEO_DEFAULT_ENCODING	"ISOLatin1"

std::string getline_from_fildes(int fildes, bool& eof);

// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf/3742999#3742999
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}
