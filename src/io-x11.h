/*
 * $Id: io-x11.h,v 1.6 2000/08/10 21:02:50 danny Exp $
 *
 * Copyright © 1993, 1999 Free Software Foundation, Inc.
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
#ifndef IO_X11H
#define IO_X11H

#ifndef X_DISPLAY_MISSING

extern void get_x11_args (int * argc_p, char ** argv);
extern int io_col_to_input_pos (int c);
extern void x11_graphics (void);
extern void set_x_default_point_size (int l);

#endif /* X_DISPLAY_MISSING */

#endif /* IO_X11H */
