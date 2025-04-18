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

//EXTERN void (*io_display_cell_cursor) (void);
EXTERN void (*io_pr_cell_win) (struct window *, CELLREF, CELLREF, CELL *);
