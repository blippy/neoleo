#pragma once
/*
 * $Id: io-abstract.h,v 1.8 2000/08/10 21:02:50 danny Exp $
 *
 * Copyright � 1992, 1993 Free Software Foundation, Inc.
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
#include "window.h"

inline void _do_nothing() { };

inline void (*io_open_display) () = _do_nothing;
//EXTERN void (*io_redisp) (void);	/* Refresh the existing image. */
EXTERN void (*io_repaint) (void);	/* $$$ Recompute the image. */
EXTERN void (*io_repaint_win) (struct window *);
EXTERN void (*io_close_display) (int);

/*
 * Low level input.
 */
EXTERN int (*io_input_avail) (void);/* How many chars can be read? */
EXTERN void (*io_scan_for_input) (int ok_to_block);
//EXTERN int (*io_read_kbd) (char *buf, int size);

/* Annoy the user: */
//EXTERN void (*io_bell) (void);

/* High level i/o. */

//EXTERN void (*io_flush) (void); 

/* Input and status areas: */
//EXTERN void (*io_clear_input_before) (void);
//EXTERN void (*io_clear_input_after) (void);
//EXTERN void (*io_update_status) (void);
//EXTERN int (*io_get_chr) (char *prompt);

//EXTERN void (*io_fix_input) (void);
EXTERN void (*io_move_cursor) (void);
EXTERN void (*io_erase) (int len);
EXTERN void (*io_insert) (int len);
EXTERN void (*io_over) (const char *, int len);


/* Cell values */
EXTERN void (*io_hide_cell_cursor) (void);
EXTERN void (*io_display_cell_cursor) (void);
EXTERN void (*io_pr_cell_win) (struct window *, CELLREF, CELLREF, CELL *);

/* The terminal's cursor may be in the current cell or the input area. */
EXTERN void (*io_cellize_cursor) (void);
EXTERN void (*io_inputize_cursor) (void);


//inline void _io_update_width_nothing(int col, int wid) {};

//inline void (*io_update_width)(int col, int wid) = _io_update_width_nothing;

