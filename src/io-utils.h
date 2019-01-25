#pragma once
/*
 * $Id: io-utils.h,v 1.12 2000/08/10 21:02:50 danny Exp $
 *
 * Copyright © 1992, 1999 Free Software Foundation, Inc.
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

#include <string>

#include "global.h"
#include "cell.h"
#include "numeric.h"

extern int get_abs_rng (char **pptr, struct rng *retp);


extern char numb_oflo[];
extern char *bname[];
extern char print_buf[];

extern void init_infinity (void);
extern char * flt_to_str (num_t val);
extern char * flt_to_str_fmt (CELL *cp);
extern char * long_to_str (long val);
extern std::string print_cell (CELL *cp);
std::string cell_value_string (CELLREF row, CELLREF col, int add_quote);
extern char * adjust_prc (char *oldp, CELL *cp, int width, int smallwid, int just);
extern void set_usr_stats (int usr_n, char **usr_buf);
extern int usr_set_fmts (void);
extern void get_usr_stats (int usr_num, char **usr_buf);
extern char * cell_name (CELLREF rr, CELLREF cc);
extern char * range_name (struct rng *rng);
extern char * col_to_str (CELLREF col);
extern void clear_spreadsheet (void);
extern int words_imatch (char ** ptr, char * key);
extern int parray_len (char ** array);
extern int words_member (char ** keys, int len, char * check);
extern int prompt_len (char * prompt);
extern int says_default (char * str);

extern void FileSetCurrentFileName(const char *s);
std::string FileGetCurrentFileName();
//extern void FileCloseCurrent(void);
extern void FileCloseCurrentFile(void);
extern char *file_get_default_format(void);
extern char *file_get_format(int);
extern char *file_get_pattern(char *fmt);
extern void file_set_default_format(char *);
extern void write_file_generic(FILE *, struct rng *, char *);
extern void read_file_generic(FILE *, int, char *, const char *);

extern void OleoSetEncoding(const char *);
extern void OleoUserPrefEncoding(char *);
extern char *OleoGetEncoding(void);

std::string stringify_value_file_style(value* val);

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

//std::string 
char* pr_flt (num_t val, struct user_fmt *fmt, int prec, bool use_prec = true);
char *flt_to_str (num_t val);


#define	OLEO_DEFAULT_ENCODING	"ISOLatin1"

