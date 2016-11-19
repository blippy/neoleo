/*
 * $Id: line.c,v 1.6 2000/08/10 21:02:50 danny Exp $
 *
 * Copyright © 1990, 1992, 1993 Free Software Foundation, Inc.
 * 
 * This file is part of Oleo, the GNU Spreadsheet.
 * 
 * Oleo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * Oleo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Oleo; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include <stdio.h>
#include "global.h"
#include "line.h"
#include <stdarg.h>

void
set_line (struct line *line, char *string)
{
  int len;

  len = strlen (string);
  if (line->alloc <= len)
    {
      if (len < LINE_MIN)
	len = LINE_MIN;
      else
	len++;
      line->alloc = len + 1;
      if (line->buf)
	line->buf = ck_realloc (line->buf, line->alloc);
      else
	line->buf = ck_malloc (line->alloc);
    }
  strcpy (line->buf, string);
}

void
setn_line (struct line *line, char *string, int n)
{
  int len = n;
  if (line->alloc <= len)
    {
      if (len < LINE_MIN)
	len = LINE_MIN;
      else
	len++;
      line->alloc = len;
      line->buf = ck_remalloc (line->buf, line->alloc);
    }
  bcopy (string, line->buf, n);
  line->buf[n] = 0;
}

#define Max(A,B)  ((A) > (B) ? (A) : (B))

void
catn_line (struct line *line, char *string, int n)
{
  int len = (line->buf ? strlen (line->buf) : 0);
  if (line->alloc <= len + n + 1)
    {
      line->alloc = Max (len + n + 1, LINE_MIN);
      line->buf = ck_remalloc (line->buf, line->alloc);
    }
  if (n)
    bcopy (string, line->buf + len, n);
  line->buf[len + n] = '\0';
}


void
sprint_line (struct line *line, char * fmt, ...)
{
  va_list iggy;
  int len;

  len = strlen (fmt) + 200;
  if (!line->alloc)
    {
      line->buf = ck_malloc (len);
      line->alloc = len;
    }
  else if (line->alloc < len)
    {
      line->buf = ck_realloc (line->buf, len);
      line->alloc = len;
    }
  va_start (iggy, fmt);
  vsprintf (line->buf, fmt, iggy);
  va_end (iggy);
}

void
splicen_line (struct line * line, char * str, int n, int pos)
{
  int old_len = strlen (line->buf);
  int len = old_len + n;
  if (line->alloc <= len)
    {
      line->alloc = len;
      line->buf = ck_remalloc (line->buf, len + 1);
    }
  line->buf[len--] = '\0';
  --old_len;
  while (old_len >= pos)
    {
      line->buf[len] = line->buf[old_len];
      --len;
      --old_len;
    }
  while (n--)
    line->buf[pos + n] = str[n];
}

void
edit_line (struct line * line, int begin, int len)
{
  int old_len = strlen (line->buf);
  int new_len = old_len - len;
  while (begin < new_len)
    {
      line->buf[begin] = line->buf[begin + len];
      ++begin;
    }
  line->buf[begin] = '\0';
}


void
free_line (struct line * line)
{
  if (line->buf && line->alloc)
    free (line->buf);
  line->buf = 0;
  line->alloc = 0;
}




int
read_line (struct line * line, FILE * fp, int * linec)
{
  int pos = 0;
  int c = getc (fp);

  while ((c != EOF) && (c != '\n'))
    {
      if (pos + 2 >= line->alloc)
	{
	  line->alloc = (line->alloc ? line->alloc * 2 : 1);
	  line->buf = ck_remalloc (line->buf, line->alloc);
	}
      if (c != '\\')
	line->buf[pos++] = c;
      else
	{
	  int next_c = getc (fp);
	  if (next_c != '\n')
	    {
	      line->buf[pos++] = '\\';
	      line->buf[pos++] = next_c;
	    }
	  /* Else the backslash and newline are discarded from the input. */
	  else
	    ++*linec;
	}
      c = getc (fp);
    }
  if (pos + 1 > line->alloc)
    {
      ++line->alloc;
      line->buf = ck_remalloc (line->buf, line->alloc);
    }
  line->buf[pos] = 0;
  if (line->buf[0] || (c != EOF))
    {
      ++*linec;
      return 1;
    }
  else
    return 0;
}
