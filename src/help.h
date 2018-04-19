#pragma once
/*
 * $Id: help.h,v 1.4 2000/08/10 21:02:50 danny Exp $
 *
 * Copyright © 1993 Free Software Foundation, Inc.
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
#include "info.h"

extern void expand_help_msg ();
void describe_function_nonconst(char* name);
void describe_function_const(const char* name);
extern void brief_describe_key (struct key_sequence * keyseq);
extern void describe_key(struct key_sequence * keyseq);
extern void where_is (char* name);;
extern void help_with_command ();
extern void builtin_help(char* name);
extern void make_wallchart_info ();
extern void write_info(char * info, FILE * fp);
