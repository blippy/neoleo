#pragma once
/*
 * Copyright (c) 1993 Free Software Foundation, Inc.
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

#include "global.h"

enum motion_magic
{
  magic_up = 0,
  magic_down = 1,
  magic_right = 2,
  magic_left = 3,
  magic_up_right = 4,
  magic_up_left = 5,
  magic_down_right = 6,
  magic_down_left = 7,
  magic_no_motion = 8
};

extern const int colmagic[9];
extern  const int rowmagic[9];

void delete_row (int repeat);
void set_def_format (int fmt);
void set_cell_alignment_left();
void set_cell_alignment_right();
void insert_1row();
void recalculate (int all);
void write_cmd (FILE *fp, const char * name);
void read_file_and_run_hooks (FILE * fp,  const char * name);
int set_window_option (int set_opt, char *text);
void set_cell_toggle_bold();
void set_cell_toggle_italic();
void set_cell_toggle_percent();
