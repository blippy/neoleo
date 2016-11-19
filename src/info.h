#ifndef INFOH
#define INFOH

/*
 * $Id: info.h,v 1.4 2000/08/10 21:02:50 danny Exp $
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
struct info_buffer 
{
  char * name;
  int len;
  char ** text;
};

extern void init_info (void);
extern struct info_buffer * find_info (char * name);
extern struct info_buffer * find_or_make_info (char * name);
extern void clear_info (struct info_buffer * buf);
extern void print_info (struct info_buffer * buf, char * format, ...);
extern void io_text_start (void);
extern void io_text_line (char * format, ...);
extern void io_text_finish (void);

#endif  /* INFOH */
