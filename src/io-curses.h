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


void cur_io_repaint();




#define WIN_LCK_HZ		0x01 // locked horizontally
#define WIN_LCK_VT		0x02 // locked vertically
#define WIN_PAG_HZ		0x04 // page horizontally
#define WIN_PAG_VT		0x08 // page vertically
#define WIN_EDGES		0x10 // edged disabled
#define WIN_EDGE_REV	0x20 // edges standout
inline int win_flags = WIN_EDGES | WIN_EDGE_REV; 








enum class dirn { left, right, up, down};

