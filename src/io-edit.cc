/*
 * Copyright (c) 1992, 1993, 1999 Free Software Foundation, Inc.
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

#include <string.h>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <tuple>

#undef NULL

#include "global.h"
#include "cell.h"
#include "io-utils.h"
#include "io-edit.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "format.h"
#include "mem.h"
#include "sheet.h"
#include "regions.h"
#include "spans.h"
#include "utils.h"
#include "decompile.h"

/* Shorthand */

#define the_cursor		the_cmd_arg.cursor
#define the_text 		the_cmd_arg.text
#define the_do_prompt		the_cmd_arg.do_prompt
#define the_is_set		the_cmd_arg.is_set
#define the_overwrite		the_cmd_arg.overwrite

/* Editting primitives
 * 
 * Commands that edit arguments to other commands should work by changing
 * the string stored in the_cmd_arg.text.   These functions edit that string
 * and correctly update the display.
 */

	int
check_editting_mode (void)
{
	if (!the_cmd_frame->cmd || cur_arg >= cmd_argc || !the_do_prompt || the_is_set)
	{
		io_error_msg ("Command '%s' is not appropriate now.", cur_cmd->func_name);
		/* not reached */
	}
	return 0;
}

/* Set the currently-being-editted line. 
 *
 * When this function is called, it indicates that some argument
 * is being read interactively from the user.  That fact is recorded
 * in the command frame because it relevant to error handling.
 * (See cmd_error in cmd.c)
 *
 */
	void
begin_edit (void)
{
	Global->topclear = 0;
	the_cmd_frame->complex_to_user = 1;
	io_fix_input ();
}

	void
setn_edit_line (char * str, int len)
{
	setn_line (&the_text, str, len);
	the_cursor = len;
}

	void
toggle_overwrite (int set, int setting)
{
	if (!set)
		the_overwrite = !the_overwrite;
	else
		the_overwrite = (setting > 0);
}

	void
beginning_of_line (void)
{
	if (check_editting_mode ())
		return;
	the_cursor = 0;
	io_move_cursor ();
}


	void
end_of_line (void)
{
	if (check_editting_mode ())
		return;
	the_cursor = strlen (the_text.buf);
	io_move_cursor ();
}

	void
backward_char (int n)
{
	if (check_editting_mode ())
		return;
	if (n < 0)
		forward_char (-n);
	else
	{
		char * error = 0;
		if (the_cursor < n)
		{
			error = "Beginning of buffer.";
			the_cursor = 0;
		}
		else
			the_cursor -= n;
		io_move_cursor ();
		if (error)
			io_error_msg (error);	/* Doesn't return. */
	}
}

	void
backward_word (int n)
{
	if (check_editting_mode ())
		return;
	if (n < 0)
		forward_word (-n);
	else
	{
		if (the_cursor == strlen (the_text.buf))
			--the_cursor;
		while (n)
		{
			while (the_cursor
					&& !isalnum (the_text.buf[the_cursor]))
				--the_cursor;
			while (the_cursor
					&& isalnum (the_text.buf[the_cursor]))
				--the_cursor;
			--n;
		}
		io_move_cursor ();
	}
}


	void
forward_char (int n)
{
	if (check_editting_mode ())
		return;
	if (n < 0)
		backward_char (-n);
	else
	{
		char * error = 0;
		int len = strlen(the_text.buf);
		if ((the_cursor + n) > len)
		{
			error = "End of buffer.";
			the_cursor = len;
		}
		else
			the_cursor += n;
		io_move_cursor ();
		if (error)
			io_error_msg (error);	/* Doesn't return. */
	}
}


	void
goto_char (int n)
{
	int len;
	if (n < 0)
		n = 0;
	len = strlen(the_text.buf);
	if (n > len)
		n = len;
	the_cursor = n;
	io_move_cursor ();
}

	void
forward_word (int n)
{
	if (check_editting_mode ())
		return;
	if (n < 0)
		backward_word (-n);
	else
	{
		int len = strlen (the_text.buf);
		while (n)
		{
			while ((the_cursor < len)
					&& !isalnum (the_text.buf[the_cursor]))
				++the_cursor;
			while ((the_cursor < len)
					&& isalnum (the_text.buf[the_cursor]))
				++the_cursor;
			--n;
		}
		io_move_cursor ();
	}
}


	static void
erase (int len)
{
	if (check_editting_mode ())
		return;
	// strcpy() with overlap
	// as per https://stackoverflow.com/questions/14476627/strcpy-implementation-in-c
	char *dst = &the_text.buf[the_cursor];
	char* src = &the_text.buf[the_cursor + len];
	while(*dst++ = *src++);
	io_erase (len);
}


	void
backward_delete_char (int n)
{
	if (check_editting_mode ())
		return;
	if (n < 0)
		delete_char (-n);
	else
	{
		char * error = 0;
		if (the_cursor < n)
		{
			error = "Beginning of buffer.";
			n = the_cursor;
		}
		the_cursor -= n;
		erase (n);
		if (error)
			io_error_msg (error);	/* Doesn't return. */
	}
}


	void 
backward_delete_word (int n)
{
	if (check_editting_mode ())
		return;
	else
	{
		int at = the_cursor;
		while (n)
		{
			while (the_cursor
					&& !isalnum (the_text.buf[the_cursor]))
				--the_cursor;

			while (the_cursor
					&& isalnum (the_text.buf[the_cursor - 1]))
				--the_cursor;
			--n;
		}
		erase (at - the_cursor);
	}
}


	void
delete_to_start(void)
{
	if (check_editting_mode ())
		return;
	else
	{
		int at = the_cursor;
		the_cursor = 0;
		erase (at);
	}
}


	void
delete_char (int n)
{
	if (check_editting_mode ())
		return;
	if (n < 0)
		backward_delete_char (-n);
	else
	{
		char * error = 0;
		int len = strlen (the_text.buf);
		if (the_cursor + n > len)
		{
			error = "End of buffer.";
			n = len - the_cursor;
		}
		erase (n);
		if (error)
			io_error_msg (error);	/* Doesn't return. */
	}
}

	void
delete_word (int n)
{
	if (check_editting_mode ())
		return;
	if (n < 0)
		backward_delete_word (-n);
	else
	{
		int len = strlen (the_text.buf);
		int erase_len = 0;
		while (n)
		{
			while (((the_cursor + erase_len) < len)
					&& !isalnum (the_text.buf[(the_cursor + erase_len)]))
				++erase_len;
			while (((the_cursor + erase_len) < len)
					&& isalnum (the_text.buf[(the_cursor + erase_len)]))
				++erase_len;
			--n;
		}      
		erase (erase_len);
	}
}


	void
kill_line(void)
{
	if (check_editting_mode ())
		return;
	else
	{
		int len = strlen (the_text.buf);
		erase (len - the_cursor);
	}
}

	void
insert_string(const char * str, int len)
{
	if (check_editting_mode ())
		return;
	splicen_line (&the_text, str, len, the_cursor);
	io_insert (len);
	the_cursor += len;
}

const char*
str_and_len(const std::string& instr, int& len)
{
	len = instr.size();
	return instr.c_str();
}

	void
over_string(const std::string& instr)
{
	int len;
	const char*  str = str_and_len(instr, len);

	if (check_editting_mode ())
		return;
	if (the_cursor + len > strlen (the_text.buf))
	{
		catn_line (&the_text, str + the_text.alloc - the_cursor,
				len - (the_text.alloc - the_cursor));
		len = the_text.alloc - the_cursor;
	}
	if (len)
		bcopy (str, the_text.buf + the_cursor, len);
	io_over(str, len);
	the_cursor += len;
}

	void
put_string (const char * str, int len)
{
	std::string s1{str};
	assert(s1.size() == len);

	if (check_editting_mode ())
		return;
	if(the_overwrite)
		over_string(s1);
	else
		insert_string(str, len);
}

void
put_string (const std::string& s)
{
	put_string(s.c_str(), s.size());
}

/* Higher Level editting commands. */

	void
insert_cell_expression (void)
{
	if (check_editting_mode ()) return;
	std::string in_str = decomp_str(curow, cucol);
	put_string(in_str);
}


	void
insert_other_cell_expression (struct rng * rng)
{
	if (check_editting_mode ()) return;
	std::string in_str = decomp_str(rng->lr, rng->lc);
	//put_string(in_str.c_str(), in_str.size());
	put_string(in_str);
}

/* No quotes are provided here, because it's easier to add
 * quotes at the end of a macro than to strip them in the
 * middle of one.
 * --FB 1997.12.28
 */
	void
insert_cell_value(void)
{
	if (check_editting_mode ())
		return;
	put_string(cell_value_string(curow, cucol, 0));
}

/* Ditto.
 * --FB 1997.12.28
 */

	void
insert_other_cell_value(struct rng * rng)
{
	if (check_editting_mode ())
		return;
	put_string(cell_value_string(rng->lr, rng->lc, 0));
}



	void
insert_abs_ref(int x)
{
	if (check_editting_mode ())
		return;

	char vbuf[50];
	char * in_str;
	CELLREF mr = mkrow;
	CELLREF mc = mkcol;
	/* Insert current cell/range name as an absolute reference
	 * but if argument x is 1, insert current cell address,
	 * leaving out mark information.
	 */
	if (x)
	{
		mr = curow;
		mc = cucol;
	}
	if (Global->a0)
	{
		if (mr != NON_ROW)
			sprintf (vbuf, "$%s$%u:$%s:$%u",
					col_to_str (cucol), curow, col_to_str (mc), mr) ;
		else
			sprintf (vbuf, "$%s$%u", col_to_str (cucol), curow);
		in_str = vbuf;
	}
	else
	{
		if (mr != NON_ROW)
		{
			struct rng r;

			set_rng (&r, curow, cucol, mr, mc);
			in_str = range_name (&r);
		}
		else
			in_str = cell_name (curow, cucol);
	}
	put_string (in_str, strlen (in_str));  
}

	void
insert_cell_attr (struct rng * rng, char * attr)
{
	struct line line;
	init_line (&line);
	if (!stricmp (attr, "width"))
	{
		int wid = get_nodef_width (rng->lc);
		if (wid == 0)
			set_line (&line, "def");
		else
			sprint_line (&line, "%d", wid - 1);
	}
	else if (!stricmp (attr, "height"))
	{
		int hgt = get_nodef_height (rng->lr);
		if (hgt == 0)
			set_line (&line, "def");
		else
			sprint_line (&line, "%d", hgt - 1);
	}
	else if (!stricmp (attr, "format"))
	{
		CELL * cp = find_cell (rng->lr, rng->lc);
		if (!cp)
			set_line (&line, "def");
		else
		{
			set_line(&line, cell_format_string(cp));
		}
	}
	else if (!stricmp (attr, "font"))
	{
		// 07-Jul-2017 mcarter remove font handling
	}
	else if (!stricmp (attr, "font-scale"))
	{
		// 07-Jul-2017 mcarter more font handling removal
	}
	put_string (line.buf, strlen (line.buf));
	free_line(&line);
}

	void
insert_usr_fmt_part (int fmt, int stat)
{
	char * usr_stats[9];
	if ((fmt < 1) || (fmt > 16))
		io_error_msg
			("insert-user-format-part arg 1 out of range (%d); should be in [1-16].",
			 fmt);
	--fmt;
	if ((stat < 1) || (stat > 16))
		io_error_msg
			("insert-user-format-part arg 2 out of range (%d); should be in [1-9].",
			 stat);
	--stat;
	get_usr_stats (fmt, usr_stats);
	put_string (usr_stats[stat], strlen (usr_stats[stat]));
}

	void
self_insert_command (int ch, int count)
{
	if (check_editting_mode ()) return;

	std::string s;
	for(int i = 0; i< count; ++i)
		s += (char) ch;
	put_string(s);
}


/* Keysequences are read using the `keyseq' keymap.
 * Every key in that map should be bound to this function.
 */
	void
self_map_command (int c)
{
	struct keymap * map = the_maps[the_cmd_arg.val.key.cmd.code];
	char space = ' ';
	const char * str = char_to_string (c);

	insert_string (str, strlen (str));
	insert_string (&space, 1);

	while (map)
	{
		the_cmd_arg.val.key.cmd = map->keys[c];
		if (the_cmd_arg.val.key.cmd.vector < 0)
		{
			if (the_cmd_arg.val.key.cmd.code < 0)
				map = map->map_next;
			else
				return;
		}
		else
			break;
	}
	exit_minibuffer ();
	return;
}

	void
insert_current_filename (void)
{
	int len = FileGetCurrentFileName().size();
	if (len >0)
		put_string(FileGetCurrentFileName().c_str(), len);
}


/* Reading a single character is done with the read-char
 * map.  Every key in that map should be bound to this function.
 */
	void
exit_self_inserting (int c)
{
	const char * str = char_to_string (c);

	insert_string (str, strlen (str));
	exit_minibuffer ();
}


	static int
issymb (int c)
{
	return isalpha (c) || isdigit (c) || (c == '_');
}

#undef the_text
#undef the_cursor
#undef the_do_prompt
	void
insert_context_word (void)
{
	struct command_frame * cf = the_cmd_frame->prev;
	if (   (cf == the_cmd_frame)
			|| !cf->cmd
			|| !cf->argv [cf->_cur_arg].do_prompt)
		return;
	{
		struct command_arg * ca = &cf->argv [cf->_cur_arg];
		char * beg_text = ca->text.buf;
		char * last = beg_text + ca->cursor;
		char * start;

		while ((last > beg_text) && !issymb(*last))
			--last;
		while (*last && issymb (*last))
			++last;
		--last;
		start = last;
		while ((start > beg_text) && issymb(*start))
			--start;
		if (!issymb (*start) && (start < last))
			++start;

		if ((start <= last) && issymb (*start))
		{
			insert_string (start, last - start + 1);
			the_cmd_arg.cursor = 0;
		}
	}
}
