/*
 * $Id: io-utils.h,v 1.13 2001/04/19 00:05:27 pw Exp $
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
#ifndef IO_UTILSH
#define IO_UTILSH

#include "global.h"
#include "cell.h"
extern char numb_oflo[];
extern char *bname[];

extern void init_infinity (void);
extern char * flt_to_str (double val);
extern char * flt_to_str_fmt (CELL *cp);
extern char * long_to_str (long val);
extern char * print_cell (CELL *cp);
extern char * cell_value_string (CELLREF row, CELLREF col, int add_quote);
extern char * adjust_prc (char *oldp, CELL *cp, int width, int smallwid, int just);
extern void set_usr_stats (int usr_n, char **usr_buf);
extern int usr_set_fmts (void);
extern void get_usr_stats (int usr_num, char **usr_buf);
extern char * cell_name (CELLREF rr, CELLREF cc);
extern char * range_name (struct rng *rng);
extern int get_abs_rng (char **pptr, struct rng *retp);
extern char * col_to_str (CELLREF col);
extern void clear_spreadsheet (void);
extern int words_imatch (char ** ptr, char * key);
extern int parray_len (char ** array);
extern int words_member (char ** keys, int len, char * check);
extern int prompt_len (char * prompt);
extern int says_default (char * str);

extern void FileSetCurrentFileName(const char *s);
extern char *FileGetCurrentFileName(void);
extern void FileCloseCurrent(void);
extern char *file_get_default_format(void);
extern char *file_get_format(int);
extern char *file_get_pattern(char *fmt);
extern void file_set_default_format(char *);
extern void write_file_generic(FILE *, struct rng *, char *);
extern void read_file_generic(FILE *, int, char *, char *);

extern void OleoSetEncoding(char *);
extern void OleoUserPrefEncoding(char *);
extern char *OleoGetEncoding(void);

#define	OLEO_DEFAULT_ENCODING	"ISOLatin1"
#endif
