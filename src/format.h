#ifndef FORMATH
#define FORMATH
/*
 * $Id: format.h,v 1.5 2000/08/10 21:02:50 danny Exp $
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
#include "global.h"
#include "cell.h"

/* extern char * fmt_to_str (int fmt);	*/
extern char *cell_format_string(CELL *cp);
extern int str_to_fmt (char *ptr);
extern char * jst_to_str (int jst);
extern int chr_to_jst (int chr);

#endif /* FORMATH */
