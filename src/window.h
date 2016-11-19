/*
 * $Id: window.h,v 1.6 2001/01/10 20:16:32 danny Exp $
 *
 * Copyright © 1992, 1993, 1999 Free Software Foundation, Inc.
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

#ifndef WINDOWH
#define WINDOWH


#include "cell.h"
#include "line.h"

/* The tty windows datastructures: */


struct window
{
  /* Do not change these directly. */
  int id;
  int win_over;			/* Where the data in this window starts */
  int win_down;			/*   on the screen.  */
  struct rng screen;		/* Cells visible. recenter_* updates this. */
  int flags;			/* You must use io_set_win_flags and perhaps */
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

  /* These values may be changed at any time. */
  /* -1 if this window isn't linked to any others, else
     contains the index into wins of the window this one is
     linked to */
  int link;

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
  CELLREF win_curow;
  CELLREF win_cucol;

  VOIDSTAR *win_slops;	/* Slops in this window (tty only) */
};

struct mouse_event
{
  int seq;
  int row;
  int col;
  int button;
  int downp;
  int location;			/* See #defines, below. */
  CELLREF r;
  CELLREF c;
  struct mouse_event * next;
  struct mouse_event * prev;
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

#if 0
#define	scr_lines	Global->scr_lines
#define	scr_cols	Global->scr_cols
#endif

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

#define	info_rows	Global->info_rows
/* #define	info_line	Global->info_line */
#define	info_over	Global->info_over

/* Window borders: */
#define	default_right_border	Global->default_right_border
#define	default_bottom_border	Global->default_bottom_border

/* The window list. */
#define	nwin		Global->nwin
#define	cwin		Global->cwin
#define	wins		Global->wins
#define	win_id		Global->win_id

/* This is stored as the button number when a dequeue failes. */
#define MOUSE_QERROR	-1
/* These are the possible mouse locations. */
#define MOUSE_ON_INPUT    -1
#define MOUSE_ON_STATUS   -2
#define MOUSE_ON_EDGE	  -3

#define MOUSE_CHAR '\034'

extern int win_label_cols (struct window * win, CELLREF hr);
extern int win_label_rows (struct window * win);
extern void io_set_label_size (int r, int c);
extern void io_set_scr_size (int lines, int cols);
extern void io_set_input_rows (int n);
extern void io_set_status_rows (int n);
extern void io_set_input_status (int inp, int stat, int redraw);
extern void io_set_cwin (struct window *win);
extern void io_pr_cell (CELLREF r, CELLREF c, CELL *cp);
extern void io_redo_region (struct rng * rng);
extern void io_win_open (int hv, int where);
extern void io_win_close (struct window *win);
extern void io_move_cell_cursor (CELLREF rr, CELLREF cc);
extern void io_shift_cell_cursor (int dirn, int repeat);
extern void io_scroll_cell_cursor (int magic, int repeat);
extern void io_recenter_cur_win (void);
extern void io_recenter_all_win (void);
extern void io_set_win_flags (struct window *w, int f);
extern void io_write_window_config (struct line * out);
extern void io_read_window_config (char * line);
extern int enqueue_mouse_event (int r, int c, int button, int downp);
extern void dequeue_mouse_event (struct mouse_event *out, int seq);
extern void io_init_windows (int sl, int sc, int ui, int us, int ir, int sr,
		 int lr, int lc) ;

#endif
