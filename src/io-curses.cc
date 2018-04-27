/*
 * $Id: io-curses.c,v 1.20 2001/02/13 23:38:06 danny Exp $
 *
 * Copyright © 1992, 1993, 1999, 2001 Free Software Foundation, Inc.
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


#include <unistd.h>

#include <iostream>
#include <assert.h>
#include "funcdef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "utils.h"
#include "decompile.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using namespace std::string_literals;

#include <ncurses.h>
#include <menu.h>
#include <panel.h>

#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#undef NULL
#include "global.h"
#include "cell.h"
#include "cmd.h"
#include "line.h"
#include "io-curses.h"
#include "io-generic.h"
#include "io-edit.h"
#include "io-term.h"
#include "io-abstract.h"
#include "io-utils.h"
#include "lists.h"
#include "regions.h"
#include "window.h"
#include "key.h"
#include "input.h"
#include <term.h>
#include "logging.h"
#include "ref.h"

#define MIN_WIN_HEIGHT	(cwin->flags&WIN_EDGES ? 2 : 1)
#define MIN_WIN_WIDTH	(cwin->flags&WIN_EDGES ? 6 : 1)

static int redrew = 0;
static int textout = 0;
static int term_cursor_claimed = 0;

static void move_cursor_to (struct window *, CELLREF, CELLREF, int);

static void log_debug_1(std::string msg)
{
	if constexpr(true)
		log_debug("io-curses.cc:"s + msg);
}

void
show_main_menu()
{
	struct choice {
		char shortcut;
		std::string text;
	};

	std::vector<struct choice> choices1 { 
		{'0', "Close window"},
			{'c', "Copy cell"},
			{'v', "Paste cell"}
	};

	keypad(stdscr, TRUE);
	WINDOW *w = newwin(5, 30, 0, 0);
	box(w, 0, 0);
	PANEL  *p = new_panel(w);
	update_panels();
	doupdate();

	int r =0;
	for(const auto & c:choices1){
		r++;
		mvwprintw(w, r, 1, "%c %s", c.shortcut, c.text.c_str());
	}

	wrefresh(w);

	char sel = '\0';
	while(!sel) {
		const char c = wgetch(w);
		if( c == '0' || c == 'c' || c == 'v') sel =c;
	}

	// TODO we prolly have to delete the win, too.
	del_panel(p);
	keypad(stdscr, FALSE);

	// now actually do something
	switch(sel) {
		case 'c': // copy
			copy_this_cell_formula();
			break;
		case 'v': // paste
			paste_this_cell_formula();
			break;
		case '0': // just close the window. No action required
		default:
			break; 
	}
}

void
test_curses_suspension()
{
	const char filename[] = "ncurses.dump";
	refresh();
	def_prog_mode();
	scr_dump(filename);
	//clear();
	endwin();
	system("man man");
	reset_prog_mode();
	scr_restore(filename);
	refresh();
	doupdate();
	unlink(filename);
	io_repaint();
}


static int
curses_metric (char * str, int len)
{
  return len;
}

static struct input_view input_view
  = {0, curses_metric, curses_metric, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static void
_io_redraw_input (void)
{
  int pos;
  int row = (input_view.current_info ? 0 : Global->input);

  if (input_view.info_redraw_needed)
    {
      input_view.info_redraw_needed = 0;
      io_repaint ();
      return;
    }

  if (input_view.redraw_needed == NO_REDRAW)
    return;

  if (input_view.redraw_needed == FULL_REDRAW)
    {
      /* Redraw	the prompt. */ 
      move (row, 0);
      if (input_view.expanded_keymap_prompt)
	{
	  addstr (input_view.expanded_keymap_prompt);
	  clrtoeol ();
	  input_view.redraw_needed = NO_REDRAW;
	  return;
	}
      if (input_view.prompt_wid)
	addstr (input_view.prompt);
      pos = input_view.visibility_begin;
    }
  else
    {
      pos = input_view.redraw_needed;
      move (row,
	    input_view.prompt_wid + pos - input_view.visibility_begin);
    }
    
  if (   input_view.input_area
      && (input_view.visibility_end >= input_view.visibility_begin)
      && (input_view.visibility_begin < strlen (input_view.input_area->buf)))
    {
      int x;
      for (x = pos; x <= input_view.visibility_end; ++x)
	addch (input_view.input_area->buf[x]);
    }
  clrtoeol ();
  input_view.redraw_needed = NO_REDRAW;
}





static void
_io_fix_input (void)
{
  iv_fix_input (&input_view);
}

static void
_io_move_cursor (void)
{
  iv_move_cursor (&input_view);
}

static void
_io_erase (int len)
{
  iv_erase (&input_view, len);
}

static void
_io_insert (int len)
{
  iv_insert (&input_view, len);
}
	   
static void
_io_over (const char * str, int len)
{
  iv_over (&input_view, len);
}

static void 
_io_display_cell_cursor (void)
{
  int cell_cursor_row;
  int cell_cursor_col;
  int cc;
  int rr;
  int cwid;
  int n;
  int x, y;

  if (input_view.current_info)
    return;

  if (   (curow < cwin->screen.lr)
      || (cucol < cwin->screen.lc)
      || (curow > cwin->screen.hr)
      || (cucol > cwin->screen.hc))
    return;

  getyx (stdscr, y, x);
  cell_cursor_col = cwin->win_over;
  for (cc = cwin->screen.lc; cc < cucol; cc++)
    cell_cursor_col += get_width (cc);
  cell_cursor_row = cwin->win_down;
  for (rr = cwin->screen.lr; rr < curow; rr++)
    cell_cursor_row += get_height (rr);
  cwid = get_width (cucol);
  if (cwid > cwin->numc)
    cwid = cwin->numc;
  move (cell_cursor_row, cell_cursor_col);
  standout ();
  for (n = cwid; n; n--)
#ifdef A_STANDOUT
    addch (inch () | A_STANDOUT);
#else
    addch (inch ());
#endif
  standend ();
  move (y, x);
}

static void 
_io_hide_cell_cursor (void)
{
  int cc;
  int rr;
  int cell_cursor_row;
  int cell_cursor_col;
  int cwid;
  int n;
  int x, y;

  if (input_view.current_info)
    return;
  if (   (curow < cwin->screen.lr)
      || (cucol < cwin->screen.lc)
      || (curow > cwin->screen.hr)
      || (cucol > cwin->screen.hc))
    return;
  getyx (stdscr, y, x);
  cell_cursor_col = cwin->win_over;
  for (cc = cwin->screen.lc; cc < cucol; cc++)
    cell_cursor_col += get_width (cc);
  cell_cursor_row = cwin->win_down;
  for (rr = cwin->screen.lr; rr < curow; rr++)
    cell_cursor_row += get_height (rr);
  cwid = get_width (cucol);
  if (cwid > cwin->numc)
    cwid = cwin->numc;
  move (cell_cursor_row, cell_cursor_col);
  for (n = cwid; n; n--)
#ifdef A_STANDOUT
    addch (inch () & ~A_STANDOUT);
#else
    addch (inch ());
#endif
  move (y, x);
}

/* Functions, etc for dealing with cell contents being displayed
	on top of other cells. */

struct s { CELLREF row, clo, chi; } ;
struct slops
{
  int s_alloc, s_used;
  struct s s_b[1];
};

static void 
flush_slops (VOIDSTAR where)
{
  struct slops *s;

  s = (slops *) where;
  if (s)
    s->s_used = 0;
}

static int 
find_slop (VOIDSTAR where, CELLREF r, CELLREF c, CELLREF *cclp, CELLREF *cchp)
{
  int n;
  struct slops *s;

  s = (slops*) where;
  if (!s)
    return 0;
  for (n = 0; n < s->s_used; n++)
    {
      if (s->s_b[n].row == r && s->s_b[n].clo <= c && s->s_b[n].chi >= c)
	{
	  *cclp = s->s_b[n].clo;
	  *cchp = s->s_b[n].chi;
	  return 1;
	}
    }
  return 0;
}

static void 
kill_slop (VOIDSTAR where, CELLREF r, CELLREF clo, CELLREF chi)
{
  int n;
  struct slops *s;

  s = (slops *) where;
  for (n = 0; n < s->s_used; n++)
    {
      if (s->s_b[n].row == r && s->s_b[n].clo == clo && s->s_b[n].chi == chi)
	{
	  --(s->s_used);
	  s->s_b[n] = s->s_b[s->s_used];
	  return;
	}
    }
}

static void 
set_slop (VOIDSTAR *wherep, CELLREF r, CELLREF clo, CELLREF chi)
{
  int n;
  struct slops **sp;

  sp = (struct slops **) wherep;
  if (!*sp)
    {
      (*sp) = (slops *) ck_malloc (sizeof (struct slops) + 2 * sizeof (struct s));
      (*sp)->s_alloc = 2;
      (*sp)->s_used = 1;
      n = 0;
    }
  else
    {
      n = (*sp)->s_used++;
      if ((*sp)->s_alloc == n)
	{
	  (*sp)->s_alloc = n * 2;
	  (*sp) = (slops*) ck_realloc ((*sp), sizeof (struct slops) + n * 2 * sizeof (struct s));
	}
    }
  (*sp)->s_b[n].row = r;
  (*sp)->s_b[n].clo = clo;
  (*sp)->s_b[n].chi = chi;
}

static void 
change_slop (VOIDSTAR where,
	     CELLREF r, CELLREF olo, CELLREF ohi, CELLREF lo, CELLREF hi)
{
  int n;
  struct slops *s;

  s = (slops*)where;
  for (n = 0; n < s->s_used; n++)
    {
      if (s->s_b[n].row == r && s->s_b[n].clo == olo && s->s_b[n].chi == ohi)
	{
	  s->s_b[n].clo = lo;
	  s->s_b[n].chi = hi;
	  return;
	}
    }
}

static void 
_io_open_display (void)
{
  initscr ();
  scrollok (stdscr, 0);
#ifdef HAVE_CBREAK
  cbreak ();
#else
  crmode ();
#endif
  raw ();
  noecho ();
  nonl ();
  /* Must be after initscr() */
  io_init_windows (LINES, COLS, 1, 2, 1, 1, 1, 1);
  // io_init_windows (Global->scr_lines, Global->scr_cols, 1, 2, 1, 1, 1, 1);
  info_rows = 1;
  print_width = columns;		/* Make ascii print width == terminal width. */

  //show_main_menu();
}

void
cont_curses(void)
{
#ifdef HAVE_CBREAK
  cbreak ();
#else
  crmode ();
#endif
  raw ();
  noecho ();
  nonl ();
}


void
stop_curses(void)
{
#ifdef HAVE_CBREAK
  nocbreak ();
#else
  nocrmode ();
#endif
  noraw ();
  echo ();
  nl ();
  io_redisp ();
}

static void 
_io_cellize_cursor (void)
{
}

static void 
_io_inputize_cursor (void)
{
}

static void 
_io_redisp (void)
{
  if (!term_cursor_claimed)
    {
      _io_redraw_input ();
      if (!(input_view.current_info || input_active ||
	    input_view.expanded_keymap_prompt))
	move_cursor_to (cwin, curow, cucol, 0);
      else
	move ((input_view.current_info ? 0 : Global->input), 
	      input_view.prompt_wid + input_view.input_cursor -
	      input_view.visibility_begin); 
    }
  {
    struct rng * rng = &cwin->screen;
    if (   (curow > rng->hr)
	|| (curow < rng->lr)
	|| (cucol > rng->hc)
	|| (cucol < rng->lc))
      io_recenter_cur_win ();
  }
  refresh ();
}

static void 
_io_repaint_win (struct window *win)
{
  io_repaint ();
}

static void 
_io_repaint (void)
{
  CELLREF cc, rr;
  int n, n1;
  CELL *cp;
  struct window *win;

  clear ();
  io_fix_input ();
  redrew++;
 
  // basically, this functionality just shouldn't exist
  if(input_view.current_info)
	 return;

  for (win = wins; win < &wins[nwin]; win++)
    {
      if (win->lh_wid)
	{
	  move (win->win_down - 1, win->win_over - win->lh_wid);
	  printw ("#%*d ", win->lh_wid - 2, 1 + win - wins);
	  if (win->flags & WIN_EDGE_REV)
	    standout ();
	  cc = win->screen.lc;
	  do
	    {
	      n = get_width (cc);
	      if (n > win->numc)
		n = win->numc;
	      if (n > 1)
		{
		  char *ptr;
		  char buf[30];

		  if (Global->a0)
		    ptr = col_to_str (cc);
		  else
		    {
		      sprintf (buf, "C%u", cc);
		      ptr = buf;
		    }
		  --n;
		  n1 = strlen (ptr);
		  if (n < n1)
		    printw ("%.*s ", n, "###############");
		  else
		    {
		      n1 = (n - n1) / 2;
		      printw ("%*s%-*s ", n1, "", n - n1, ptr);
		    }
		}
	      else if (n == 1)
		addstr ("#");
	    }
	  while (cc++ < win->screen.hc);

	  rr = win->screen.lr;
	  n = win->win_down;
	  do
	    {
	      n1 = get_height (rr);
	      if (n1)
		{
		  move (n, win->win_over - win->lh_wid);
		  if (Global->a0)
		    printw ("%-*d ", win->lh_wid - 1, rr);
		  else
		    printw ("R%-*d", win->lh_wid - 1, rr);
		  n += n1;
		}
	    }
	  while (rr++ < win->screen.hr);

	  if (win->flags & WIN_EDGE_REV)
	    standend ();
	}
      flush_slops (win->win_slops);
      find_cells_in_range (&(win->screen));
      while ((cp = next_row_col_in_range (&rr, &cc)))
	if (GET_TYP (cp))
	  io_pr_cell_win (win, rr, cc, cp);
    }
  if (!(cp = find_cell (curow, cucol)) || !GET_TYP (cp))
    io_display_cell_cursor ();
  input_view.redraw_needed = FULL_REDRAW;
  _io_redraw_input ();
  io_update_status ();
}

static void 
_io_close_display (int e)
{
	if (e == 0) {
		clear ();
		refresh ();
	}

	(void) endwin ();
}

static int 
_io_input_avail (void)
{
  return (FD_ISSET (0, &read_pending_fd_set)
	  || FD_ISSET (0, &exception_pending_fd_set));
}

static void 
_curses_io_scan_for_input (int block)
{
  /* This function only exists because X kbd events don't generate
   * SIGIO. Under curses, the SIGIO hander does the work of this
   * function.
   * Attempt to have the curses mode be somewhat responsive even in
   * the presence of an endless loop by explicitly looking for events
   * here.
   */
  struct timeval tv;

  tv.tv_sec = 0;
  tv.tv_usec = 1000;
  block_until_excitement(&tv);
}

static void 
_io_wait_for_input (void)
{
	// mcarter 
  //pause ();
  int ch = getch();
  ungetch(ch);
}

static int 
_io_read_kbd (char *buf, int size)
{

	if(false) log_debug("io-curses.cc:_io_read_kbd() called");
	assert(size>0);

	int r = read (0, buf, size);
	FD_CLR (0, &read_pending_fd_set);
	FD_CLR (0, &exception_pending_fd_set);

	// mcarter 19-Aug-2017
	// Fix issue#22 wrt home, end and delete key. 
	// Detailed discussion in the manual, chapter Keymapms
	// section Special keys (home, delete, etc.)
	// remap home, and and delete keys
	if(r == 4 && buf[0] == '\033' && buf[3] == '~') {
		switch(buf[2]) {
			case '1' : buf[2] = 'H'; goto hit; // remap home key
			case '3' : goto hit; // delete key needs no change except length
			case '4' : buf[2] = 'F'; // remap end key
hit:
				   r = 3;
		}
	}

	auto log_buffer = [&]() -> void {
		std::string msg = "io-curses.cc:_io_read_kbd(): chars: ";
		for(int i=0; i<r; ++i)
			msg += "<" + std::to_string(buf[i]) + ">";
		msg += "read: " + std::to_string(r) + ", ";
		//msg += std::to_string(KEY_NPAGE);
		log_debug(msg);
	};

	// mcarter 19-Aug-2017
	// Log it when the buffer has more than 4 characters in it.
	// The function keys (F1, F2, ...) do this, which is not
	// a problem; I just wanted to guard against big sequences
	// being read in, which I am not expecting.
	// Actually, the function keys are good ones to map. I would
	// especially like to use F1 for menus or help
	if(r>4) {
		log_debug("io-curses.cc:_io_read_kbd() seems to have read suspiciously too many chars ...");
		log_buffer();
	}

	if(false) log_buffer(); // for debugging

	return r;
}


#if defined(SIGIO)


static void 
_io_nodelay (int delayp)
{
  panic ("Trying to curses nodelay on a system with SIGIO.");
}

#else

static void 
_io_nodelay (int delayp)
{
  nodelay (stdscr, delayp);
}

#endif

static int 
_io_getch (void) // TODO seems to never be called
{
	log_debug("io-curses.cc:_io_getch() called. I thought it was never called.");
	char ch;
	auto v = io_read_kbd (&ch, 1);
	int res = v == 1 ? ch : EOF;
	return res;
}

static int 
_io_get_chr (char *prompt) // TODO seems to never be called
{
	log_debug("io-curses.cc:_io_get_chr() called. I thought it was never called.");
	int x;
	mvaddstr (Global->input, 0, prompt);
	clrtoeol ();
	Global->topclear = 2;
	refresh ();
	++term_cursor_claimed;
	x = get_chr ();
	--term_cursor_claimed;
	return x;
}

#define BUFFER 10

static void 
_io_bell (void)
{
#ifndef HAVE_GETCAP
  putchar ('\007');
#else
  static char *vb;
  static int called = 0;

  if (!called)
    {
      called++;
      vb = getcap ("vb");
    }
  if (vb)
    {
      local_puts (vb);
    }
  else
    {
      local_putchar ('\007');
    }
#endif
}

static void
move_cursor_to (struct window *win, CELLREF r, CELLREF c, int dn)
{
  int cc;
  int cell_cursor_col;
  int rr;
  int cell_cursor_row;
  
  cell_cursor_col = win->win_over;
  for (cc = win->screen.lc; cc < c; cc++)
    cell_cursor_col += get_width (cc);
  cell_cursor_row = win->win_down + dn;
  for (rr = win->screen.lr; rr < r; rr++)
    cell_cursor_row += get_height (rr);
  move (cell_cursor_row, cell_cursor_col);
}

static void
_io_update_status (void)
{
	const char *ptr;
	static char hmbuf[40];
	int wid;
	int plen;
	//int dlen;
	int yy, xx;

	if (!user_status || input_view.current_info)
		return;
	getyx (stdscr, yy, xx);
	move (Global->status, 0);
	wid = columns - 2;

	if (mkrow != NON_ROW)
	{
		struct rng r;

		addch ('*');
		--wid;
		set_rng (&r, curow, cucol, mkrow, mkcol);
		ptr = range_name (&r);
	}
	else
		ptr = cell_name (curow, cucol);

	addstr (ptr);
	wid -= strlen (ptr);

	if (how_many != 1)
	{
		sprintf (hmbuf, " {%d}", how_many);
		addstr (hmbuf);
		wid -= strlen (hmbuf);
	}

	std::string dec = decomp_str(curow, cucol);
	log_debug("io-curses.cc:_io_update_status:dec:"s + dec);

	ptr = cell_value_string (curow, cucol, 1);
	plen = strlen (ptr);

	int dlen = dec.size();
	if(dlen>0)
	{
		wid -= 4;
		if (dlen + plen > wid)
		{
			if (plen + 3 > wid)
				printw (" %.*s... [...]", wid - 6, ptr);
			else
				printw (" %s [%.*s...]", ptr, wid - plen - 3, dec.c_str());
		}
		else
			printw (" %s [%s]", ptr, dec.c_str());
	}
	else if (plen)
	{
		--wid;
		if (plen > wid)
			printw (" %.*s...", wid - 3, ptr);
		else
			printw (" %s", ptr);
	}

	clrtoeol ();
	move (yy, xx);
}

extern int auto_recalc;

static void
_io_clear_input_before (void)
{
  textout = 0;
  if (Global->topclear == 2)
    {
      move (Global->input, 0);
      clrtoeol ();
      Global->topclear = 0;
    }
  move (0, 0);
}

static void
_io_clear_input_after (void)
{
  if (Global->topclear)
    {
      move (Global->input, 0);
      clrtoeol ();
      Global->topclear = 0;
    }
}


static void
_io_pr_cell_win (struct window *win, CELLREF r, CELLREF c, CELL *cp)
{
	int glowing;
	int lenstr;
	int j;
	int wid, wwid;
	int hgt;
	char *ptr;
	int yy, xx;

	if (input_view.current_info)
		return;

	wid = get_width (c);
	if (!wid)
		return;
	if (wid > win->numc)
		wid = win->numc;
	hgt = get_height (r);
	if (!hgt)
		return;
	if (hgt > win->numr)
		hgt = win->numr;

	getyx (stdscr, yy, xx);
	glowing = (r == curow && c == cucol && win == cwin);
	ptr = print_cell (cp);
	move_cursor_to (win, r, c, 0);
	if (glowing)
		standout ();
	j = GET_JST (cp);
	if (j == JST_DEF)
		j = default_jst;
	lenstr = strlen (ptr);

	if (lenstr <= wid - 1)
	{
		CELLREF ccl, cch;

		if (j == JST_LFT)
			printw ("%-*.*s", wid, wid - 1, ptr);
		else if (j == JST_RGT)
			printw ("%*.*s ", wid - 1, wid - 1, ptr);
		else if (j == JST_CNT)
		{
			wwid = (wid - 1) - lenstr;
			printw ("%*s%*s ", (wwid + 1) / 2 + lenstr, ptr, wwid / 2, "");
		}
#ifdef TEST
		else
			panic ("Unknown justification");
#endif
		if (glowing)
			standend ();

		if (lenstr == 0 && c > win->screen.lc
				&& find_slop (win->win_slops, r, c - 1, &ccl, &cch))
		{
			CELLREF ccdl, ccdh;

			if (find_slop (win->win_slops, r, c, &ccdl, &ccdh) && ccdl == c)
			{
				kill_slop (win->win_slops, r, ccdl, ccdh);
				for (; ccdh != ccdl; --ccdh)
					if (ccdh != c && (wwid = get_width (ccdh)))
					{
						move_cursor_to (win, r, ccdh, 0);
						printw ("%*s", wwid, "");
					}
			}
			kill_slop (win->win_slops, r, ccl, cch);
			io_pr_cell (r, ccl, find_cell (r, ccl));
		}
		else if (find_slop (win->win_slops, r, c, &ccl, &cch))
		{
			kill_slop (win->win_slops, r, ccl, cch);
			for (; cch != ccl; --cch)
				if (cch != c && (wwid = get_width (cch)))
				{
					move_cursor_to (win, r, cch, 0);
					printw ("%*s", wwid, "");
				}
			io_pr_cell (r, ccl, find_cell (r, ccl));
		}
	}
	else
	{
		CELLREF cc = c;
		CELL *ccp;
		CELLREF ccl, cch;

		for (wwid = wid; lenstr > wwid - 1; wwid += get_width (cc))
		{
			if (++cc > win->screen.hc
					|| ((ccp = find_cell (r, cc))
						&& GET_TYP (ccp)
						&& (GET_FORMAT (ccp) != FMT_HID
							|| (GET_FORMAT (ccp) == FMT_DEF
								&& default_fmt != FMT_HID))))
			{
				--cc;
				break;
			}
		}

		if (lenstr > wwid - 1) {  /* FIXME: This construct needs to be checked */
			if (GET_TYP (cp) == TYP_FLT)
				ptr = adjust_prc (ptr, cp, wwid - 1, wid - 1, j);
			else if (GET_TYP (cp) == TYP_INT)
				ptr = (char *) numb_oflo;
		}

		if (wwid == 1)
		{
			addch (' ');
			if (glowing)
				standend ();
		}
		else if (wwid == wid)
		{
			printw ("%-*.*s ", wwid - 1, wwid - 1, ptr);
			if (glowing)
				standend ();
		}
		else if (glowing)
		{
			printw ("%.*s", wid, ptr);
			standend ();
			printw ("%-*.*s ", wwid - wid - 1, wwid - wid - 1, ptr + wid);
		}
		else if (r == curow && (cucol > c && cucol <= cc))
		{
			CELLREF ctmp;
			int w_left;
			int w_here;

			w_left = wid;
			for (ctmp = c + 1; ctmp < cucol; ctmp++)
				w_left += get_width (ctmp);
			printw ("%.*s", w_left, ptr);
			standout ();
			w_here = get_width (cucol);
			if (wwid > w_left + w_here)
			{
				printw ("%-*.*s", w_here, w_here, ptr + w_left);
				standend ();
				printw ("%-*.*s ",
						wwid - (w_left + w_here) - 1, wwid - (w_left + w_here) - 1,
						ptr + w_left + w_here);
			}
			else
			{
				printw ("%-*.*s", w_here, w_here - 1, ptr + w_left);
				standend ();
			}
		}
		else
			printw ("%-*.*s ", wwid - 1, wwid - 1, ptr);

		if (find_slop (win->win_slops, r, c, &ccl, &cch))
		{
			change_slop (win->win_slops, r, ccl, cch, c, cc);
			for (; cch > cc; --cch)
				if ((wwid = get_width (cch)))
				{
					move_cursor_to (win, r, cch, 0);
					printw ("%*s", wwid, "");
				}
			for (cch = c - 1; cch > ccl; --cch)
				if ((wwid = get_width (cch)))
				{
					move_cursor_to (win, r, cch, 0);
					printw ("%*s", wwid, "");
				}
			if (ccl != c)
				io_pr_cell (r, ccl, find_cell (r, ccl));
		}
		else
			set_slop ((VOIDSTAR *) (&(win->win_slops)), r, c, cc);
	}
	if ((hgt > 1) && Global->display_formula_mode)
	{
		move_cursor_to (win, r, c, 1);
		std::string formula = decomp_str(r, c);
		log_debug_1("curses:_io_pr_cell_win:formula:"s + formula);
		printw ("%.*s ", wid - 1, formula.c_str());
	}
	if (glowing)
		io_update_status ();
	move (yy, xx);
}


static void
_io_flush (void)
{
  refresh ();
}

void
_io_command_loop (int a)
{
	command_loop (a, 0);
}


/* try to just pain the curses screen without doing anything else
 * Doesn't seem to work
 */
void 
show_curses_view()
{
	_io_open_display();
	//redraw_info();
	      input_view.redraw_needed = FULL_REDRAW;
     _io_redraw_input ();
//_io_redisp();
	sleep(2);
	_io_close_display(0);
}


void
tty_graphics (void)
{
  FD_SET (0, &read_fd_set);
  FD_SET (0, &exception_fd_set);
  io_command_loop = _io_command_loop;
  io_open_display = _io_open_display;
  io_redisp = _io_redisp;
  io_repaint = _io_repaint;
  io_repaint_win = _io_repaint_win;
  io_close_display = _io_close_display;
  io_input_avail = _io_input_avail;
  io_scan_for_input = _curses_io_scan_for_input;
  io_wait_for_input = _io_wait_for_input;
  io_read_kbd = _io_read_kbd;
  io_nodelay = _io_nodelay;
  io_getch = _io_getch;
  io_bell = _io_bell;
  io_get_chr = _io_get_chr;
  io_update_status = _io_update_status;
  io_fix_input = _io_fix_input;
  io_move_cursor = _io_move_cursor;
  io_erase = _io_erase;
  io_insert = _io_insert;
  io_over = _io_over;
  io_flush = _io_flush;
  io_clear_input_before = _io_clear_input_before;
  io_clear_input_after = _io_clear_input_after;
  io_pr_cell_win = _io_pr_cell_win;
  io_hide_cell_cursor = _io_hide_cell_cursor;
  io_cellize_cursor = _io_cellize_cursor;
  io_inputize_cursor = _io_inputize_cursor;
  io_display_cell_cursor = _io_display_cell_cursor;

  // added by mcarter:
  //io_run_main_loop = _io_run_curses_main_loop;
  io_run_main_loop = fairly_std_main_loop;
}
