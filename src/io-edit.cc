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
#include <sstream>

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
//#include "xcept.h"

/* Shorthand */

#define the_cursor		the_cmd_arg.cursor
//#define the_text 		the_cmd_arg.text
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
	/*
	if (!the_cmd_frame->cmd || cur_arg >= cmd_argc || !the_do_prompt || the_is_set)
	{
		io_error_msg ("Command '%s' is not appropriate now.", cur_cmd->func_name);
		// not reached 
	}
*/
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
	ASSERT_UNCALLED();
}

	void
setn_edit_line (char * str, int len)
{
	//setn_line (&the_text, str, len);
	the_cursor = len;
}

	void
toggle_overwrite (int set, int setting)
{
	ASSERT_UNCALLED();
}

	void
beginning_of_line (void)
{
	ASSERT_UNCALLED();
}


	void
end_of_line (void)
{
	ASSERT_UNCALLED();
}

	void
backward_char (int n)
{
	ASSERT_UNCALLED();
}

	void
backward_word (int n)
{
	ASSERT_UNCALLED();
}


	void
forward_char (int n)
{
	ASSERT_UNCALLED();
}


	void
goto_char (int n)
{
	ASSERT_UNCALLED();
}

	void
forward_word (int n)
{
	ASSERT_UNCALLED();
}


	static void
erase (int len)
{
	ASSERT_UNCALLED();
}


	void
backward_delete_char (int n)
{
	ASSERT_UNCALLED();
}


	void 
backward_delete_word (int n)
{
	ASSERT_UNCALLED();
}


	void
delete_to_start(void)
{
	ASSERT_UNCALLED();
}


	void
delete_char (int n)
{
	ASSERT_UNCALLED();
}

	void
delete_word (int n)
{
	ASSERT_UNCALLED();
}


	void
kill_line(void)
{
	ASSERT_UNCALLED();
}

const char*
str_and_len(const std::string& instr, int& len)
{
	len = instr.size();
	return instr.c_str();
}

	void
insert_string(const std::string& instr)
{
	ASSERT_UNCALLED();
}


	void
over_string(const std::string& instr)
{
	int len = 0;
	const char*  str = str_and_len(instr, len);

	if (check_editting_mode ())
		return;
	/*
	if (the_cursor + len > 666)
	{
		catn_line (&the_text, str + the_text.alloc - the_cursor,
				len - (the_text.alloc - the_cursor));
		len = the_text.alloc - the_cursor;
	}
	
	if (len)
		bcopy (str, the_text.buf + the_cursor, len);
	*/
	io_over(str, len);
	the_cursor += len;
}

	void
put_string (const std::string& s1)
{
	if (check_editting_mode ())
		return;
	if(the_overwrite)
		over_string(s1);
	else
		insert_string(s1);
}


/* Higher Level editting commands. */

	void
insert_cell_expression (void)
{
	ASSERT_UNCALLED();
}


	void
insert_other_cell_expression (struct rng * rng)
{
	ASSERT_UNCALLED();
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

	std::ostringstream oss;

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
		if (mr != NON_ROW) {
			//sprintf (vbuf, "$%s$%u:$%s:$%u", col_to_str (cucol), curow, col_to_str (mc), mr) ;
			oss << "$" << col_to_str(cucol)
				<< "$" << curow 
				<< ":$" << col_to_str(mc) 
				<< ":$" << mr;
		} else {
			//sprintf (vbuf, "$%s$%u", col_to_str (cucol), curow);
			oss << "$" << col_to_str(cucol) << "$" << curow;
		}
		// in_str = vbuf;
	}
	else
	{
		if (mr != NON_ROW)
		{
			struct rng r;

			set_rng (&r, curow, cucol, mr, mc);
			oss << range_name (&r);
		}
		else
			oss << cell_name (curow, cucol);
	}
	put_string(oss.str());  
}

	void
insert_cell_attr (struct rng * rng, char * attr)
{
	std::ostringstream oss;
	if (!stricmp (attr, "width"))
	{
		int wid = get_nodef_width (rng->lc);
		if (wid == 0)
			oss << "def";
		else
			oss << wid-1;
	}
	else if (!stricmp (attr, "height"))
	{
		int hgt = get_nodef_height (rng->lr);
		if (hgt == 0)
			oss << "def";
		else
			oss << hgt-1;
	}
	else if (!stricmp (attr, "format"))
	{
		CELL * cp = find_cell (rng->lr, rng->lc);
		if (!cp)
			oss << "def";
		else
		{
			oss << cell_format_string(cp);
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
	put_string(oss.str());
}

	void
insert_usr_fmt_part (int fmt, int stat)
{
	char * usr_stats[9];
	if ((fmt < 1) || (fmt > 16))
		io_error_msg ("insert-user-format-part arg 1 out of range (%d); should be in [1-16].", fmt);
	--fmt;
	if ((stat < 1) || (stat > 16))
		io_error_msg ("insert-user-format-part arg 2 out of range (%d); should be in [1-9].", stat);
	--stat;
	get_usr_stats(fmt, usr_stats);
	put_string(usr_stats[stat]);
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


	void
insert_current_filename (void)
{
	std::string name = FileGetCurrentFileName();
	if(name == "") return;
	put_string(name);
}


/* Reading a single character is done with the read-char
 * map.  Every key in that map should be bound to this function.
 */
	void
exit_self_inserting (int c)
{
	ASSERT_UNCALLED();
}


	static int
issymb (int c)
{
	return isalpha (c) || isdigit (c) || (c == '_');
}

#undef the_text
#undef the_cursor
#undef the_do_prompt
