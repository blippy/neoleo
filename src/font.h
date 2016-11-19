/*
 * $Id: font.h,v 1.6 2000/08/10 21:02:50 danny Exp $
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
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef FONTH
#define FONTH

#include "global.h"
#include "line.h"


/* Oleo' concept of a font family: */
struct font_names
{
  struct font_names * next;

  char * oleo_name;		/* like `times' */
  char * x_name;		/* like `*times-medium-r-*'.  In particular,
				 * the defintion will only scale properly
				 * if the point size is wild-carded.
				 */
  char * ps_name;		/* like `Times-Roman'. */
};

/* Oleo's concept of a particular set of glyphs (specified as a family + a 
 * point size (relative to the default).
 */
struct font_memo
{
  struct font_memo * next;
  struct font_names * names;
  double scale;			/* ratio of def pt. size to this */
  int id_memo;			/* This is used by oleofile. */
};

extern struct font_memo *font_list;

extern void define_font (char * oleo_name, char * x_name, char * ps_name);
extern struct font_memo * intern_font (char * oleo_name, double scale);
extern struct font_memo * default_font (void);
extern struct font_memo * matching_font (char * x_name, char * ps_name, double scale);
extern struct font_memo * parsed_matching_font (char * fullname);
extern void set_region_font (struct rng *rng, char * oleo_name, double scale);
extern void flush_fonts (void);
extern void set_x_default_font (char * str);
extern void set_ps_font_cmd (char * ps_name);
extern void set_default_font (char * name, double scale);
extern void init_fonts (void);

#endif
