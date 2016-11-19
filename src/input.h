#ifndef INPUTH
#define INPUTH
/*
 * $Id: input.h,v 1.4 2000/08/10 21:02:50 danny Exp $
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

/* Display-generic updating logic for the input area. */
typedef int (*text_measure) (char * str, int len);

/* These are for the field REDRAW_NEEDED */
#define NO_REDRAW		-2
#define FULL_REDRAW		-1

struct input_view
{
  /* If this is less than 0, see the #defines above.
   * >= 0, this is the index of a character in the 
   * input string.  All characters at that index and 
   * greater need to be redrawn.
   */
  int redraw_needed;

  /* These are provided by io-{curses,x11} and tell how to convert
   * strings to widths.
   */
  text_measure prompt_metric;
  text_measure input_metric;
  
  /* If the currently mapped keymap has a prompt, the display of that
   * prompt takes precedence.
   */
  char * keymap_prompt;
  char * expanded_keymap_prompt;

  /* This is the width of either the keymap_prompt or the input text
   * prompt, whichever is current (0 if neither is).
   */

  int prompt_wid;
  
  /* The parameters below are a cache.  If this flag is true,
   * the cache is known to be wrong.
   */
  int must_fix_input;

  struct line * input_area;	/* The text editted in the input area or 0. */
  char * prompt;
  int visibility_begin;		/* Index of first visible char or 0. */
  int visibility_end;		/* Index of last visible char or 0. */
  int input_cursor;		/* Index of the cursor position or 0. */
  int vis_wid;			/* This is the width of the visible text 
				 * with extra space for the cursor, if it 
				 * happens to be past the end of the string.
				 */

  /* A command_arg can specify an info buffer which should be displayed 
   * while prompting for that arg.
   */
  struct info_buffer * current_info;
  int info_pos;		/* In the current info, the first vis. line */ 
  int info_redraw_needed;	/* != 0 if redraw needed */
};


extern void iv_fix_input (struct input_view * this_iv);
extern void iv_move_cursor (struct input_view * this_iv);
extern void iv_erase (struct input_view * this_iv, int len);
extern void iv_insert (struct input_view * this_iv, int len);
extern void iv_over (struct input_view * this_iv, int len);

#endif  /* INPUTH */
