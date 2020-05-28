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
#include "io-abstract.h"
#include "utils.h"
//#include "xcept.h"


/* In the functions below, we only ever deal with one input_view at a 
 * time.  By convention, call a pointer to it THIS_IV and use these
 * macros: 
 */
#define Redraw_needed 		(this_iv->redraw_needed)
#define Prompt_metric 		(this_iv->prompt_metric)
#define Input_metric 		(this_iv->input_metric)
#define Keymap_prompt 		(this_iv->keymap_prompt)
#define Expanded_keymap_prompt 	(this_iv->expanded_keymap_prompt)
#define Prompt_wid 		(this_iv->prompt_wid)
#define Must_fix_input 		(this_iv->must_fix_input)
#define Input_area 		(this_iv->input_area)
#define Prompt 			(this_iv->prompt)
#define Visibility_begin 	(this_iv->visibility_begin)
#define Visibility_end 		(this_iv->visibility_end)
#define Current_info		(this_iv->current_info)
#define Info_pos		(this_iv->info_pos)
#define Info_redraw_needed	(this_iv->info_redraw_needed)
#define Vis_wid			(this_iv->vis_wid)
#define Input_cursor 		(this_iv->input_cursor)




/* This is called strategicly from the command loop and whenever
 * the input area is changed by beginning a complex command.
 * It may do nothing, though if the input area appeas to have changed
 * it will recompute its appearence.
 *
 * now it also updates the info_fields of the input view.
 */

	void
iv_fix_input (struct input_view * this_iv)
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


