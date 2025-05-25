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

#if 0
#include <memory>
#include <string>

#include "cell.h"

extern char numb_oflo[];


char * adjust_prc (char *oldp, CELL *cp, int width, int smallwid, enum jst just);
std::string cell_value_string (CELLREF row, CELLREF col, int add_quote);
std::string print_cell (CELL *cp);
std::string print_cell ();

int 		words_imatch (char ** ptr, char * key);
void 		FileCloseCurrentFile(void);

void 		write_file_generic(FILE *, struct rng *, char *);


std::string  fmt_std_date(int t);

std::string stringify_value_file_style(const value_t& val);
std::string range_name (struct rng *);





extern struct user_fmt dol;
extern struct user_fmt cma;
extern struct user_fmt pct;
extern struct user_fmt fxt;

#define NUM_USER_FMT (16)



std::string flt_to_str (num_t val);
std::string trim(const std::string& str);

#define	OLEO_DEFAULT_ENCODING	"ISOLatin1"

std::string getline_from_fildes(int fildes, bool& eof);


#endif
