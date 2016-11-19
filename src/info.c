/*
 * $Id: info.c,v 1.8 2000/08/10 21:02:50 danny Exp $
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include "global.h"
#include "info.h"
#include "hash.h"
#include "cmd.h"
#include <stdarg.h>

struct hash_control * info_buffers;


void
init_info (void)
{
  info_buffers = hash_new();
}

struct info_buffer *
find_info (char * name)
{
  return (struct info_buffer *)hash_find (info_buffers, name);
}


struct info_buffer * 
find_or_make_info (char * name)
{
  struct info_buffer * buf =
    (struct info_buffer *)hash_find (info_buffers, name);
  if (buf)
    return buf;
  
  buf = ((struct info_buffer *)
	 ck_malloc (sizeof (struct info_buffer) + strlen(name) + 1));
  buf->name = (char *)buf + sizeof (*buf);
  strcpy (buf->name, name);
  buf->len = 0;
  buf->text = 0;
  hash_insert (info_buffers, buf->name, buf);
  return buf;
}

void
clear_info (struct info_buffer * buf)
{
  if (buf->text)
    {
      int x;
      int stop = buf->len;
      for (x = 0; x < stop; ++x)
	ck_free (buf->text[x]);
      ck_free (buf->text);
    }
  buf->text = 0;
  buf->len = 0;
}

/* This appears to be a source of trouble in the help
 * system.  Every time help is requested for non-standard
 * instructions (macros and the like), Oleo dumps core.
 * It would have been useful to see some sort of note here
 * re what this is meant to do exactly ... ?
 * --FB
 */

void
print_info (struct info_buffer * buf, char * format, ...)
{
  va_list ap;
  char txt[1000];
  int len;			/* Length of the new line */
  
  va_start (ap, format);
  vsprintf (txt, format, ap);
  va_end (ap);
  len = strlen (txt);

  ++buf->len;			/* Number of lines in the buffer */
  buf->text = (char **)ck_remalloc (buf->text, buf->len * sizeof (char *));
  buf->text[buf->len - 1] = (char *)ck_malloc (len + 1);
  bcopy (txt, buf->text[buf->len - 1], len + 1);
}




/* A generic buffer for the use informational commands like show-options */
static struct info_buffer * the_text_buf;

void
io_text_start (void)
{
  the_text_buf = find_or_make_info ("_text");
  clear_info (the_text_buf);
}



void
io_text_line (char * format, ...)
{
  va_list ap;
  char txt[1000];
  int len;			/* Length of the new line */
  
  va_start (ap, format);
  vsprintf (txt, format, ap);
  va_end (ap);
  len = strlen (txt);

  if (the_text_buf == NULL)
	return;

  ++the_text_buf->len;		/* Number of lines in the the_text_buf */
  the_text_buf->text =
    (char **)ck_remalloc (the_text_buf->text,
			  the_text_buf->len * sizeof (char *));
  the_text_buf->text[the_text_buf->len - 1] = (char *)ck_malloc (len + 1);
  bcopy (txt, the_text_buf->text[the_text_buf->len - 1], len + 1);
}

void
io_text_finish (void)
{
  run_string_as_macro ("{view-info _text}");
}

