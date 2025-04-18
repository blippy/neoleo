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
