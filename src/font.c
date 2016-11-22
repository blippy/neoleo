/*
 * $Id: font.c,v 1.9 2000/08/10 21:02:50 danny Exp $
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "font.h"
#include "window.h"
#include "io-abstract.h"
#include "cmd.h"
#include "io-x11.h"
#include "io-term.h"
#include "io-utils.h"

static char * default_oleo_name = "default";
static char * default_x_name = "*times-medium-r-*";
static char * default_ps_name = "Times-Roman";

static struct font_names * font_names = 0;
struct font_memo * font_list = 0;
struct font_memo * the_default_font = 0;

void
define_font (char * oleo_name, char * x_name, char * ps_name)
{
  struct font_names * fn;

  for (fn = font_names; fn; fn = fn->next)
    if (!stricmp(fn->oleo_name, oleo_name))
      {
	if (fn->x_name)
	  free (fn->x_name);
	if (fn->ps_name)
	  free (fn->ps_name);
	break;
      }
  if (!fn)
    {
      fn = (struct font_names *)ck_malloc (sizeof (*fn));
      fn->oleo_name =  strdup (oleo_name);
      fn->next = font_names;
      font_names = fn;
    }
  fn->x_name = strdup (x_name);
  fn->ps_name = strdup (ps_name);
}


static struct font_names * 
find_font_name (char * name)
{
  struct font_names * fn = font_names;
  if (!name || says_default (name))
    name = "default";

  while (fn)
    {
      if (!stricmp (name, fn->oleo_name))
	return fn;
      fn = fn->next;
    }
  io_error_msg ("Unknown font family %s\n", name);
  return 0;			/* never reached, actually */
}

/* For backwards compatability, there are created-on-demand fonts
 * who's oleo name is a concatenation of their x and postscript names.
 */

static struct font_names *
matching_font_names (char * x_name, char * ps_name)
{
  struct font_names * fn;
  for (fn = font_names; fn; fn = fn->next)
    {
      if (   stricmp (fn->oleo_name, "default")
	  && !stricmp (ps_name, fn->ps_name)
	  && !stricmp (x_name, fn->x_name))
	return fn;
    }
  {
    char * oleo_name = mk_sprintf ("%s,%s", x_name, ps_name);
    define_font (oleo_name, x_name, ps_name);
    fn = find_font_name (oleo_name);
    free (oleo_name);
    return fn;
  }
}

struct font_memo *
intern_font (char * oleo_name, double scale)
{
  struct font_names *names = find_font_name (oleo_name);
  struct font_memo *it = font_list;
  while (it)
    {
      if ((scale == font_list->scale) && (names == font_list->names))
	return it;
      it = it->next;
    }
  it = (struct font_memo *)ck_malloc (sizeof (*it));
  it->scale = scale;
  it->names = names;
  it->next = font_list;
  font_list = it;
  return it;
}

struct font_memo * 
default_font (void)
{
  return the_default_font;
}

struct font_memo *
matching_font (char * x_name, char * ps_name, double scale)
{
  struct font_names * fn = matching_font_names (x_name, ps_name);
  return intern_font (fn->oleo_name, scale);
}

/* This is for the benefit of oleofile.c only */

struct font_memo *
parsed_matching_font (char * fullname)
{
  char x_name[1000];
  char ps_name[1000];
  char *p;
  char *p2;
  double scale;

  for (p = fullname; isspace (*p); ++p);
  for (p2 = x_name; *p && (*p != ',') && !isspace (*p); *p2++ = *p++);
  *p2 = '\0';
  if (x_name[0] == '\0' || says_default(x_name))
    strcpy (x_name, default_x_name);
/* Got rid of the weird defined font-name expansion here. 
 * These auto-magical fonts are only for the sake of reading old files
 * which don't use that (cough) feature.
 */
  while (isspace (*p))   ++p;
  if (*p == ',')    ++p;
  while (isspace (*p))  ++p;
  for (p2 = ps_name; *p && (*p != ',') && !isspace (*p); *p2++ = *p++) ;
  *p2 = '\0';

  if (ps_name[0] == '\0' || says_default(x_name))
    strcpy (ps_name, default_ps_name);
  while (isspace (*p))     ++p;
  if (*p == ',')     ++p;
  while (isspace (*p))     ++p;

  if (isdigit (*p))
    {
      errno = 0;
      scale = atof (p);
      if (errno)
	scale = 1.;
    }
  else
    scale = 1.;

  return matching_font (x_name, ps_name, scale);
}

void 
set_region_font (struct rng *rng, char * oleo_name, double scale)
{
  struct font_memo *font = ((oleo_name || scale != 1.0)
			    ? intern_font (oleo_name, scale)
			    : 0);
  CELL * cp;
  
  make_cells_in_range (rng);
  cp = next_cell_in_range ();
  while (cp)
    {
      cp->cell_font = font;
      cp = next_cell_in_range ();
    }
  io_redo_region (rng);
}

void 
flush_fonts (void)
{
  CELL * cp;
  struct rng rng;
  rng.lr = MIN_ROW;
  rng.hr = MAX_ROW;
  rng.lc = MIN_COL;
  rng.hc = MAX_COL;
  find_cells_in_range (&rng);
  cp = next_cell_in_range ();
  while (cp)
    {
      cp->cell_font = 0;
      cp = next_cell_in_range ();
    }
}

void
set_x_default_font (char * str)
{
  define_font (default_oleo_name, str, the_default_font->names->ps_name);
  io_repaint ();
}

void
set_ps_font_cmd (char * ps_name)
{
  define_font (default_oleo_name, the_default_font->names->x_name, ps_name);
}

void
set_default_font (char * name, double scale)
{
  struct font_names * fn = find_font_name (name);
  if (fn != find_font_name (default_oleo_name))
    define_font (default_oleo_name, fn->x_name, fn->ps_name);
  the_default_font = intern_font (default_oleo_name, scale);
#ifndef X_DISPLAY_MISSING
  if (using_x)
    set_x_default_point_size (Global->cell_font_point_size);
  else
#endif
    io_repaint ();
}

void 
init_fonts (void)
{
  define_font (default_oleo_name, default_x_name, default_ps_name);
  define_font ("times", default_x_name, default_ps_name);
  define_font ("f9x15", "9x15", "sdlkfj");
  the_default_font = intern_font (default_oleo_name, 1.0);
}
