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

#include <ncurses.h>
#include <string>

//extern void cont_curses(void);
//extern void stop_curses(void);
//extern void tty_graphics (void);



void _io_repaint();
void cur_io_update_status (void);
void win_io_hide_cell_cursor ();
void cur_io_display_cell_cursor();
void cur_io_open_display();
void  io_init_windows ();



#include "cell.h"

/* The tty windows datastructures: */


struct window
{
  /* Do not change these directly. */
  const int id = 1; // a window id
  int win_over = 0;			// x-posiition Where the data in this window starts. Can change due to row number
  const int win_down = 3;	// y-position where data grid starts
  struct rng screen{0};		/* Cells visible. recenter_* updates this. */
  //int flags;			/* You must use io_set_win_flags and perhaps */
				/*   io_recenter_cur_win */

  /* Number of lines of spreadsheet that can fit in this window.
     This only changes when the screen is resized,
     win->flags&WIN_EDGES changes, or a window is either
     created or destroyed */
  int numr;
  
  /* Number of text columns that can fit in this window.
     This changes when the screen is resized,
     win->flags&WIN_EDGES changes, a window is created or
     destoryed, or win->lh_wid changes.  In the last case
     win->numc+win->lh_wid remains a constant. */
  int numc;
  
  /* 
   * Number of columns and rows for right and bottom edges. 
   * As this changes, numc and numr change accordingly.
   */
  int bottom_edge_r;
  int right_edge_c;


  /* Number of columns taken up by the row numbers at the
     left hand edge of the screen.  Zero if edges is
     win->flags&WIN_EDGES is off (by definition).  Seven (or
     five) if win->flags&WIN_PAG_HZ (to make things easier).
     Ranges between three "R9 " to seven "R32767 " depending on
     the number of the highest row on the screen.  */
  int lh_wid;

  
  /* Cursor row/column in this window */
  /* Note that the external variables curow, cucol are used for
     the currently active cursor position, so if you want
     cwin->curow and cwin->cucol to be accurate, you have to
     set them yourself. */
  //CELLREF win_curow;
  //CELLREF win_cucol;

  //void* *win_slops;	/* Slops in this window (tty only) */
};


/* Window flags:
   0x01	Locked horizontally
   0x02	Locked vertically
   0x04	Page Horizontally
   0x08	Page Vertically
   0x10	Edges disabled
   0x20	Edges standout
   */
#define WIN_LCK_HZ	0x01
#define WIN_LCK_VT	0x02
#define WIN_PAG_HZ	0x04
#define WIN_PAG_VT	0x08
#define WIN_EDGES	0x10
#define WIN_EDGE_REV	0x20
inline int win_flags = WIN_EDGES | WIN_EDGE_REV; 


/* These control the layout of input and status lines. */
#define	user_input	Global->user_input
#define	user_status	Global->user_status
/* #define	input		Global->input */
/* #define	status		Global->status */
#define	input_rows	Global->input_rows
#define	status_rows	Global->status_rows

/* These control the layout of edge labels. */
#define	label_rows	Global->label_rows
#define	label_emcols	Global->label_emcols

/* Temporary hacks for displaying multi-line messages. */
/* If this is non-0, it should be displayed instead of 
 * the windows of cells.  This is a temporary hack.
 * Use set_info to change this.
 */
/* #define	current_info	Global->current_info */

//#define	info_rows	Global->info_rows
/* #define	info_line	Global->info_line */
//#define	info_over	Global->info_over

/* Window borders: */
#define	default_right_border	Global->default_right_border
#define	default_bottom_border	Global->default_bottom_border


inline window the_cwin;
inline window* cwin = &the_cwin;
//#define	wins		Global->wins
#define	win_id		Global->win_id


enum class dirn { left, right, up, down};

extern int win_label_cols (struct window * win, CELLREF hr);
extern int win_label_rows (struct window * win);
extern void io_set_input_status (int inp, int stat, int redraw);
extern void io_set_cwin (struct window *win);
extern void io_pr_cell (CELLREF r, CELLREF c, CELL *cp);
extern void io_win_open (int hv, int where);
extern void io_move_cell_cursor (CELLREF rr, CELLREF cc);
extern void io_recenter_cur_win (void);
extern void io_recenter_named_window(struct window *w);
extern void io_recenter_all_win (void);
void io_shift_cell_cursor (dirn way, int repeat);
extern void io_set_win_flags (struct window *w, int f);
extern void io_init_windows () ;
void  recenter_window (struct window *win);
void set_numcols (struct window *win, CELLREF hr);
void win_io_repaint_win(struct window *win);
void cur_io_pr_cell_win (struct window *win, CELLREF r, CELLREF c, CELL *cp);
void cur_io_pr_cell (CELLREF r, CELLREF c, CELL *cp);
