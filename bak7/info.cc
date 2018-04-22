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

#include <map>
#include <string>
#include <stdarg.h>

#include "global.h"
#include "info.h"
//#include "hash.h"
#include "cmd.h"
#include "logging.h"
#include "utils.h"

using namespace std::string_literals;

std::map<std::string, info_buffer_t*> info_buffers_1;

//////////////////////////////////////////////////////////////////////////
// atexit cleanup
// There must surely be a better way of doing this: probably by
// declaring info_buffer as a class, and making info_buffers_1
// be a map of info_buffer's rather than their pointers.
//
class cleanup_buffers { public: ~cleanup_buffers(); };

cleanup_buffers::~cleanup_buffers() 
{
	for(auto it = info_buffers_1.begin() ; it != info_buffers_1.end(); ++it) {
		auto buf = it->second;
		clear_info(buf);
		delete buf;
	}
}

static cleanup_buffers buffer_cleaner;
//////////////////////////////////////////////////////////////////////////


struct info_buffer *
find_info(const char * name)
{
	auto it = info_buffers_1.find(name);
	if(it != info_buffers_1.end())
		return it->second;
	else
		return nullptr;
}


info_buffer_t * 
find_or_make_info(const char * name)
{
	if constexpr(false) log_debug("find_or_make_info:"s + name);
	struct info_buffer * buf = find_info(name);
	if (buf)
		return buf;

	//buf = ((struct info_buffer *) ck_malloc (sizeof (struct info_buffer) + strlen(name) + 1));
	buf = new info_buffer_t;
	buf->name = name;
	//buf->name = (char *)buf + sizeof (*buf);
	//strcpy (buf->name, name);
	//buf->len = 0;
	//buf->text = 0;
	info_buffers_1[name] = buf;
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

void fill_info_buffer(struct info_buffer* buf, char* text, int len)
{

	++buf->len;		// Number of lines in the the_text_buf
	buf->text = (char **)ck_remalloc (buf->text, buf->len * sizeof (char *));
	buf->text[buf->len - 1] = (char *)ck_malloc (len + 1);
	bcopy(text, buf->text[buf->len - 1], len + 1);
}
	
/* This appears to be a source of trouble in the help
 * system.  Every time help is requested for non-standard
 * instructions (macros and the like), Oleo dumps core.
 * It would have been useful to see some sort of note here
 * re what this is meant to do exactly ... ?
 * --FB
 */

void
print_info (struct info_buffer * buf, const char * format, ...)
{
	log_debug("print_info");
	va_list ap;
	char txt[1000];
	int len;			// Length of the new line

	va_start (ap, format);
	vsprintf (txt, format, ap);
	va_end (ap);
	len = strlen (txt);

	fill_info_buffer(buf, txt, len);
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
io_text_line (const char * format, ...)
{
	log_debug("io_text_line");
	
	va_list ap;
	char txt[1000];
	int len;			// Length of the new line

	va_start (ap, format);
	vsprintf (txt, format, ap);
	va_end (ap);
	len = strlen (txt);

	if (the_text_buf == NULL) return;

	fill_info_buffer(the_text_buf, txt, len);

}

void
io_text_finish (void)
{
  run_string_as_macro ("{view-info _text}");
}

