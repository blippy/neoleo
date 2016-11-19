#ifndef BASICH
#define BASICH

/*
 * $Id: basic.h,v 1.6 2000/08/10 21:02:49 danny Exp $
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
extern char * motion_name[9];
extern enum motion_magic complementary_motion[9];
extern enum motion_magic opposite_motion[9];

extern int run_load_hooks;

extern void noop (void);
extern void insert_row (int repeat);
extern void insert_col (int repeat);
extern void delete_row (int repeat);
extern void delete_col (int repeat);
extern void open_window (char *text);
extern void hsplit_window (void);
extern void vsplit_window (void);
extern void close_window (char *text);
extern void delete_window (void);
extern void delete_other_windows (void);
extern void nicely_goto_window (int n);
extern void goto_minibuffer (void);
extern void goto_window (char *text);
extern void other_window (void);
extern int set_window_option (int set_opt, char *text);
extern void show_window_options (void);
extern void recenter_window (void);
extern void suspend_oleo (void);
extern void recalculate (int all);
extern void kill_oleo (void);
extern void kill_all_cmd (void);
extern void redraw_screen (void);
extern void shift_cell_cursor (int dir, int repeat);
extern void scroll_cell_cursor (int dir, int repeat);
extern void goto_region (struct rng *r);
extern void goto_cell (struct rng * rng);
extern void exchange_point_and_mark (int clrmk);
extern void upper_left (void);
extern void lower_left (void);
extern void upper_right (void);
extern void lower_right (void);
extern void mark_cell_cmd (int popmk);
extern void unmark_cmd (void);
extern void save_mark_to_cell (struct rng * rng);
extern void save_point_to_cell (struct rng * rng);
extern void do_mouse_goto (void);
extern void do_mouse_mark (void);
extern void do_mouse_mark_and_goto (void);
extern void do_mouse_cmd (void (*fn) ());
extern void mouse_mark_cmd (void);
extern void mouse_goto_cmd (void);
extern void mouse_mark_and_goto_cmd (void);
extern void kill_cell_cmd (void);
extern void sort_region_cmd (char *ptr);
extern void imove (struct rng * rng, int ch);
extern void inc_direction (int count, int page_rule, int hack_magic);
extern void beginning_of_row (int count);
extern void end_of_row (int count);
extern void beginning_of_col (int count);
extern void end_of_col (int count);
extern void scan_cell_cursor (int magic, int count);
extern void edit_cell (char * new_formula);
extern void set_region_formula (struct rng * rng, char * str);
extern void goto_edit_cell (int c);
extern void goto_set_cell (int c);
extern void read_cmds_cmd (FILE *fp);
extern void read_file_and_run_hooks (FILE * fp, int ismerge, char * name);
extern void toggle_load_hooks (int turn_on);
extern void write_cmd (FILE *fp, char * name);
extern void read_cmd (FILE *fp, char * name);
extern void read_merge_cmd (FILE *fp);
extern void write_reg_cmd (FILE *fp, struct rng *rng);
extern void set_region_height (struct rng * rng, char * height);
extern void set_region_width (struct rng * rng, char * width);
extern void set_region_protection (struct rng * rng, int prot);
extern void set_region_alignment (struct rng * rng, int align);
extern void set_region_format (struct rng * rng, int fmt);
extern void set_def_height (char * height);
extern void set_def_width (char * width);
extern void set_def_protection (int prot);
extern void set_def_alignment (int align);
extern void set_def_format (int fmt);
extern void set_def_font (void);
extern void define_usr_fmt (int fmt, char * pos_h, char * neg_h, char * pos_t,
		char * neg_t, char * zero, char * comma, char * decimal,
		char * precision, char * scale);
extern void set_auto_direction (enum motion_magic magic);
extern void auto_move (void);
extern void auto_next_set (void);
extern void recompile_spreadsheet (void);

#endif  /* BASICH */
