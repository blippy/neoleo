/*
 * $Id: input.c,v 1.11 2001/02/13 23:38:06 danny Exp $
 * 
 * Copyright © 1993, 2001 Free Software Foundation, Inc.
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
#include "input.h"
#include "cmd.h"
#include "window.h"
#include "io-abstract.h"


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



#define Max(A,B)  ((A) < (B) ? (B) : (A))
#define Min(A,B)  ((A) > (B) ? (B) : (A))


/* Decide if a keymap prompt should be displayed.  If so, 
 * return the prompt, else 0.
 */

static char * 
desired_keymap_prompt (struct input_view * this_iv)
{
  int map = cur_keymap;
  if (the_cmd_frame->cmd && (the_cmd_arg.style == &keyseq_style))
    map = the_cmd_arg.val.key.cmd.code;
  return ((map >= 0) ? map_prompts[map] : 0);
}




/* In WID columns, find the first char in STR that can
 * be displayed while still leaving POS visible.  METRIC
 * converts str->cols.
 */

static int
find_vis_begin (int * wid_used, int wid,
		char * str, int pos, text_measure metric)
{
  int used;			/* How many cols allocated? */
  if (str[pos])
    used = metric (&str[pos], 1);
  else
    /* POS could be just after the end of STR, in which case pretend
     * that the character there is a SPC. 
     */
    used = metric (" ", 1);

  while (pos && (used < wid))
    {
      int next;
      next = metric (&str[pos - 1], 1);
      if (used + next > wid)
	break;
      else
	{
	  --pos;
	  used += next;
	}
    }
  if (wid_used)
    *wid_used = used;
  return pos;
}



/* Find the last visible character...  -1 if none are vis. */

static int
find_vis_end (int * wid_used, int wid, char * str, int start, text_measure
	      metric) 
{
  int used = metric (&str[start], 1);	/* How many cols allocated? */
  int pos = start;
  int max = strlen (str) - 1;

  while ((pos < max) && (used < wid))
    {
      int next;
      next = metric (&str[pos + 1], 1);
      if (used + next > wid)
	break;
      else
	{
	  ++pos;
	  used += next;
	}
    }
  if (wid_used)
    *wid_used = used;
  return pos;
}

static void
set_vis_wid (struct input_view * this_iv)
{
  Vis_wid = (Input_metric (Input_area->buf + Visibility_begin,
			     Visibility_end - Visibility_begin)
	       + ((Input_cursor > Visibility_end)
		  ? Input_metric (" ", 1)
		  : 0));
}


/* This recomputes the input area parameters of an input_view, attempting to 
 * center the cursor. 
 */

static void
iv_reset_input (struct input_view * this_iv)
{
  char * km = desired_keymap_prompt (this_iv);
  if (km && (km == Keymap_prompt))
    return;
#ifdef DEBUG
  fprintf(stderr, "input.c: iv_reset_input\n");
#endif

  Redraw_needed = FULL_REDRAW;
  Keymap_prompt = km;
  if (km)
    {
#ifdef DEBUG
      fprintf(stderr, "input.c: inside if (km) loop\n");
#endif
      /* A keymap prompt should be displayed. */
      Expanded_keymap_prompt = expand_prompt (Keymap_prompt);
      Prompt_wid = Prompt_metric (Expanded_keymap_prompt,
				  strlen(Expanded_keymap_prompt));
      Must_fix_input = 1;
      Visibility_begin = Visibility_end = 0;
      Input_area = 0;
      Vis_wid = Input_metric (" ", 1);
      Prompt = 0;
      Input_cursor = 0;
    }
  else
    {
#ifdef DEBUG
      fprintf(stderr, "input.c: inside if!(km) loop\n");
#endif
      if (Expanded_keymap_prompt)
	{
#ifdef DEBUG
	  fprintf(stderr, "input.c: inside Expanded_keymap_prompt\n");
#endif
	  ck_free (Expanded_keymap_prompt);
	  Expanded_keymap_prompt = 0;
	}
      if (!the_cmd_frame->cmd || the_cmd_arg.is_set || !the_cmd_arg.do_prompt)
	{
	  Prompt_wid = 0;
	  Must_fix_input = 0;
	  Input_area = 0;
	  Vis_wid = 0;
	  Prompt = 0;
	  Visibility_begin = Visibility_end = 0;
	  Input_cursor = 0;
	}
      else
	{
#ifdef DEBUG
	  fprintf(stderr, "input.c: inside else of ~the_cmd_frame, etc...\n");
#endif
	  Prompt_wid = Prompt_metric (the_cmd_arg.expanded_prompt,
				       strlen (the_cmd_arg.expanded_prompt));
#ifdef DEBUG
          fprintf(stderr, "step1\n");
#endif
	  Must_fix_input = 0;
#ifdef DEBUG
	  fprintf(stderr, "step2\n");
#endif
	  Input_area = &the_cmd_arg.text;
#ifdef DEBUG
	  fprintf(stderr, "step3\n");
#endif
	  Prompt = the_cmd_arg.expanded_prompt;

#ifdef DEBUG
	  fprintf(stderr, "input.c: Before scr_cols loop\n");
#endif
	  if ((Global->scr_cols - Prompt_wid) < Input_metric ("M", 1))
	    Prompt += find_vis_begin (&Prompt_wid,
				      Global->scr_cols - Input_metric("M", 1),
				      Prompt, strlen(Prompt) - 1,
				      Prompt_metric);
	  
	  {
	    int wid_avail = Global->scr_cols - Prompt_wid;
#ifdef DEBUG
	    fprintf(stderr, "input.c: Inside scr_cols loop\n");
#endif
	    Visibility_begin =
	      find_vis_begin (0, wid_avail / 2, Input_area->buf,
			      the_cmd_arg.cursor, Input_metric);
	    Visibility_end =
	      find_vis_end (0, wid_avail,
			    Input_area->buf, Visibility_begin,
			    Input_metric); 
	  }

	  Input_cursor = the_cmd_arg.cursor;
	  set_vis_wid (this_iv);
	}
    }
#ifdef DEBUG
    fprintf(stderr, "input.c: Leaving iv_reset_input\n");
#endif
}

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
  char * km_prompt = desired_keymap_prompt (this_iv);

  if (Keymap_prompt && (Keymap_prompt == km_prompt))

    Must_fix_input = 1;		/* Do nothing, keymap prompt has precedence */

  else if (Must_fix_input
	   || (Keymap_prompt != km_prompt)
	   || ((the_cmd_frame->cmd
		&& (the_cmd_arg.do_prompt && !the_cmd_arg.is_set))
	       ? ((Input_area != &the_cmd_arg.text)
		  || (Prompt != the_cmd_arg.expanded_prompt)
		  || (Input_cursor != the_cmd_arg.cursor))
	       : (Input_area || Prompt_wid)))
    iv_reset_input (this_iv);

  if (the_cmd_frame->cmd
      && ((the_cmd_arg.prompt_info != Current_info)
	  || (the_cmd_arg.info_line != Info_pos)))
    {
      Current_info = the_cmd_arg.prompt_info;
      Info_pos = the_cmd_arg.info_line;
      Info_redraw_needed = 1;
    }
  else if (Current_info
	   && (!the_cmd_frame->cmd || !the_cmd_arg.prompt_info))
    {
      Current_info = 0;
      io_repaint ();
    }
	   
}


/* Incremental updates:
 * For simple edits it is not necessary to redraw the entire line.
 * These schedule incremental updating.
 */


void
iv_move_cursor (struct input_view * this_iv)
{
  if (   Must_fix_input
      || (Visibility_begin > the_cmd_arg.cursor)
      || (   ((Visibility_end + 1) < the_cmd_arg.cursor)
	  || (((Visibility_end + 1) == the_cmd_arg.cursor)
	      && ((Vis_wid + Prompt_wid) < Global->scr_cols))))

    Must_fix_input = 1;
  else if (Redraw_needed != FULL_REDRAW)
    {
      if (   (Redraw_needed == NO_REDRAW)
	  || ((Redraw_needed > Input_cursor)
	      && (Redraw_needed > the_cmd_arg.cursor)))
	{
	  Input_cursor = the_cmd_arg.cursor;
	  set_vis_wid (this_iv);
	}
      else
	{
	  Redraw_needed = Min (Input_cursor, the_cmd_arg.cursor);
	  set_vis_wid (this_iv);
	}
    }
}

void
iv_erase (struct input_view * this_iv, int len)
{
  if (Must_fix_input
      || (the_cmd_arg.cursor <= Visibility_begin))
    {
      Must_fix_input = 1;
      return;
    }
  else if (Redraw_needed != FULL_REDRAW)
    {
      if ((Redraw_needed == NO_REDRAW) || (Redraw_needed > the_cmd_arg.cursor))
	{
	  Redraw_needed = the_cmd_arg.cursor;
	  Visibility_end = find_vis_end (0, Global->scr_cols - Prompt_wid,
					 Input_area->buf, Visibility_begin,
					 Input_metric);
	  Input_cursor = the_cmd_arg.cursor;
	}
      else
	Redraw_needed = the_cmd_arg.cursor;
      set_vis_wid (this_iv);
    }
}


void
iv_insert (struct input_view * this_iv, int len)
{
  if (!Must_fix_input)
    {
      int new_end;
      int cursor_past_end = (Input_cursor + len == strlen (Input_area->buf));
      
      new_end = find_vis_end (0, Global->scr_cols - Prompt_wid,
			      Input_area->buf, Visibility_begin,
			      Input_metric);
      
      if ((new_end + cursor_past_end) < (Input_cursor + len))
	Must_fix_input = 1;
      else
	{
	  if (   (Redraw_needed != FULL_REDRAW)
	      && ((Redraw_needed == NO_REDRAW) || (Redraw_needed > Input_cursor)))
	    Redraw_needed = Input_cursor;
	  Input_cursor += len;
	  Visibility_end = new_end;
	  set_vis_wid (this_iv);
	}
    }
}
	   
void
iv_over (struct input_view * this_iv, int len)
{
  iv_insert (this_iv, len);
}


