#pragma once
/*
 * Copyright (c) 1992, 1993 Free Software Foundation, Inc.
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


inline bool using_curses = false;
inline bool user_wants_headless = false;

/* Cell size paramaters. */
inline unsigned int default_width = 8;
inline unsigned int default_height = 1;

/* clear_spreadsheet () uses these to restore default */
inline unsigned int saved_default_width = 8;
inline unsigned int saved_default_height = 1;

/* Other cell defaults: */
inline int default_jst = base_default_jst;
inline int default_fmt = FMT_GEN;
inline int default_prc = 0x0F;		/* FIX ME */
inline int default_lock = LCK_UNL;

/* When printing ascii output, this controls page width. */
//inline int modified;

//inline int option_filter;
inline std::string option_tests_argument = "regular";

extern void set_options (char * ptr);
extern void show_options (void);
extern void read_mp_usr_fmt (char *ptr);
//extern void write_mp_options (FILE *fp);
extern void read_mp_options (char *str);
extern void show_all_var (void);
extern void write_variables (FILE * fp);
extern void read_variables (FILE * fp);
extern void InitializeGlobals(void);
void choose_display(bool force_cmd_graphics);
bool get_option_tests();




