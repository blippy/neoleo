#ifndef IO_ABSTRACTH
#define IO_ABSTRACTH
/*
 * $Id: io-abstract.h,v 1.8 2000/08/10 21:02:50 danny Exp $
 *
 * Copyright © 1992, 1993 Free Software Foundation, Inc.
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
 * along with Oleo; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifdef DEFINE_IO_VARS
#define EXTERN
#else
#define EXTERN extern
#endif

#include "global.h"
#include "line.h"
#include "window.h"

#ifndef VOLATILE
#define VOLATILE volatile
#endif

EXTERN void (*io_open_display) (void);
EXTERN void (*io_redisp) (void);	/* Refresh the existing image. */
EXTERN void (*io_repaint) (void);	/* $$$ Recompute the image. */
EXTERN void (*io_repaint_win) (struct window *);
EXTERN void (*io_close_display) (int);

/*
 * Low level input.
 */
EXTERN int (*io_input_avail) (void);/* How many chars can be read? */
EXTERN void (*io_scan_for_input) (int ok_to_block);
EXTERN void (*io_wait_for_input) (void);
EXTERN int (*io_read_kbd) (char *buf, int size);

/* A curses compatible interface.  These have no effect on low level
   calls except to consume characters which io_read_kbd might
   otherwise return. */
EXTERN void (*io_nodelay) (int);
EXTERN int (*io_getch) (void);

/* Annoy the user: */
EXTERN void (*io_bell) (void);

/* High level i/o. */

EXTERN void (*io_flush) (void); 

/* Input and status areas: */
EXTERN void (*io_clear_input_before) (void);
EXTERN void (*io_clear_input_after) (void);
EXTERN void (*io_update_status) (void);
EXTERN int (*io_get_chr) (char *prompt);

EXTERN void (*io_fix_input) (void);
EXTERN void (*io_move_cursor) (void);
EXTERN void (*io_erase) (int len);
EXTERN void (*io_insert) (int len);
EXTERN void (*io_over) (char *, int len);

#ifndef X_DISPLAY_MISSING
extern int io_col_to_input_pos (int col);
#else
#define io_col_to_input_pos(i)  0
#endif


/* Cell values */
EXTERN void (*io_hide_cell_cursor) (void);
EXTERN void (*io_display_cell_cursor) (void);
EXTERN void (*io_pr_cell_win) (struct window *,
			       CELLREF, CELLREF,
			       CELL *);

/* The terminal's cursor may be in the current cell or the input area. */
EXTERN void (*io_cellize_cursor) (void);
EXTERN void (*io_inputize_cursor) (void);

/* The main loop */
EXTERN void (*io_command_loop) (int);

#endif /* IO_ABSTRACTH */
