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
#include "cell.h"


//inline bool using_curses = false;
//inline bool user_wants_headless = false;



inline std::string option_tests_argument = "";

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
int do_set_option (char *ptr);



