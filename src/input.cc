/*
 * Copyright (c) 1993, 2001 Free Software Foundation, Inc.
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

#include "global.h"
#include "errors.h"
#include "input.h"
#include "cmd.h"
#include "window.h"
#include "utils.h"



/* This is called strategicly from the command loop and whenever
 * the input area is changed by beginning a complex command.
 * It may do nothing, though if the input area appeas to have changed
 * it will recompute its appearence.
 *
 * now it also updates the info_fields of the input view.
 */

void iv_fix_input (struct input_view * this_iv)
{

}


/* Incremental updates:
 * For simple edits it is not necessary to redraw the entire line.
 * These schedule incremental updating.
 */


	void
iv_move_cursor (struct input_view * this_iv)
{
	ASSERT_UNCALLED();
}

	void
iv_erase (struct input_view * this_iv, int len)
{
	ASSERT_UNCALLED();
}


	void
iv_insert (struct input_view * this_iv, int len)
{
	ASSERT_UNCALLED();
}

	void
iv_over (struct input_view * this_iv, int len)
{
	ASSERT_UNCALLED();
}


