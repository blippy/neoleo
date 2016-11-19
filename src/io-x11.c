#undef	I18N_VERBOSE
#define	X_I18N
/*
 * $Id: io-x11.c,v 1.31 2001/03/09 11:33:29 danny Exp $
 *
 *	Copyright © 1992, 1993, 1999, 2000, 2001 Free Software Foundation, Inc.
 * 	
 * 	This program is free software; you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation; either version 2, or (at your option)
 * 	any later version.
 * 	
 * 	This program is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 * 	
 * 	You should have received a copy of the GNU General Public License
 * 	along with this software; see the file COPYING.  If not, write to
 * 	the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#ifndef	HAVE_MOTIF
#ifndef X_DISPLAY_MISSING

#include <stdio.h>
#include <ctype.h>
#include <sys/param.h>
#define NeedFunctionPrototypes 0
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/keysym.h>
#include "global.h"
#include "utils.h"
#include "io-generic.h"
#include "io-edit.h"
#include "io-utils.h"
#include "io-term.h"
#include "cmd.h"
#include "window.h"
#include "line.h"
#include "font.h"
#include "lists.h"
#include "ir.h"
#include "display.h"
#include "io-abstract.h"
#include "regions.h"
#include "input.h"
#include "info.h"

#include <locale.h>

#if defined(HAVE_RINT)
extern double rint (double);
#else
#define rint(x) (((x)<0) ? ceil((x)-.5) : floor((x)+.5))
#endif
static void record_damage ();

extern char * x_get_string_resource (XrmDatabase, char *, char *);
extern XrmDatabase x_load_resources (Display *, char *, char *);
extern char * getenv (const char *);

static char *emergency_font_name = "8x13";
static char *cell_font_name = "times_roman12";
static char *default_font_name = "8x13";
static char *input_font_name = "8x13";
static char *status_font_name = "6x10";
static char *text_line_font_name = "8x13";
static char *label_font_name = "5x8";
static char *default_bg_color_name = "black";
static char *default_fg_color_name = "white";

/* The geometry of the first window. */
static int geom_x = 0;
static int geom_y = 0;
static int geom_w = 675;
static int geom_h = 350;
static char geom_string[] = "675x350+0+0";

/* This global is used only during command line and .Xdefaults handling. */
static Display * theDisplay;
static struct sXport *thePort;

/* i18n */
static XIC	xic = 0;
static XIM	xim = 0;

static char * rdb_class_name = "Oleo";
static char * rdb_name = "oleo";

#if 0
static XrmOptionDescRec x11_options[] =
{
  {0, 0, 0, 0}
};

/* This dynamicly computes the size of x11_options to make 
 * upkeep of this file less error prone.
 */
static int
  count_options ()
{
  int x;
  for (x = 0; x11_options[x].option; ++x)
    ;
  return x;
}
#endif
/* This synthesizes class and generic names for resources (fancy strcat).
 * Memory is reused after a reasonable number of calls. 
 */
#define RDB_NAME_BUF	8
#define RDB_NAME_SIZE	256

static char * 
resource_name (char * left, char * right)
{
  static char bufs[RDB_NAME_BUF][RDB_NAME_SIZE];
  static int buf_pos = 0;
  int len = strlen(left);
  
  if (len)
    bcopy (left, bufs[buf_pos], len);
  bufs[buf_pos][len] = '.';
  strcpy (bufs[buf_pos] + len + 1, right);
  return bufs [buf_pos++];
}

static char *
class_of (char * foo)
{
  return resource_name (rdb_class_name, foo);
}

static char *
name_of (char * foo)
{
  return resource_name (rdb_name, foo);
}

static XrmDatabase rdb;
static KeySym RetKeySym;

void
get_x11_args (int * argc_p, char ** argv)
{
  char	*locale;

  XrmInitialize ();
  
#if 0
  XrmDatabase argv_resources;
  /* Get the command line arguments. */
  XrmParseCommand (&argv_resources, x11_options,
		   count_options (x11_options),
		   rdb_name, argc_p, argv);
  
  /* Compute the display from either resources or getenv. */
  
  Global->io_x11_display_name =
    x_get_string_resource (argv_resources, class_of ("Display"),
			   name_of ("display"));
#endif					       
  if (!Global->io_x11_display_name)
    Global->io_x11_display_name = ck_savestr (getenv ("DISPLAY"));
  
  if (!Global->io_x11_display_name)
    return;
  
  locale = setlocale(LC_ALL, "");	/* i18n */
#ifdef	I18N_VERBOSE
  fprintf(stderr, "Setlocale() -> locale %s\n", locale);
#endif

  if (! XSupportsLocale()) {
	fprintf(stderr, "X doesn't support this locale\n");
	exit(0);
  }
  if (XSetLocaleModifiers("@im=none") == NULL) {
	fprintf(stderr, "XSetLocaleModifiers failed\n");
	exit(0);
  }

  theDisplay = XOpenDisplay (Global->io_x11_display_name);
  if (!theDisplay)
    panic ("Can not connect to X.  Check your DISPLAY evironment variable.");
  
  FD_SET (ConnectionNumber (theDisplay), &read_fd_set);
  FD_SET (ConnectionNumber (theDisplay), &exception_fd_set);
  
  RetKeySym = XStringToKeysym("Return");

  /* Load the resource databases in a manner not unlike emacs :). */
  rdb = x_load_resources (theDisplay, 0, rdb_class_name);
  
#if 0
  /* Merge in the command line database.  */
  XrmMergeDatabases (argv_resources, rdb);
#endif
  
  /* Set up the various defaults (staticly declared above). */
  {
    char * val;
    
    val = x_get_string_resource (rdb, class_of ("Foreground"), 
				 name_of("foreground"));
    if (val)
      default_fg_color_name = val;
    
    val = x_get_string_resource (rdb, class_of ("Background"), 
				 name_of("background"));
    if (val)
      default_bg_color_name = val;
    
    val = x_get_string_resource (rdb, class_of ("Geometry"),
				 name_of("geometry"));
    if (val)
      XGeometry (theDisplay, DefaultScreen(theDisplay), val, geom_string,
		 0, 1, 1, 0, 0, &geom_x, &geom_y, &geom_w, &geom_h);
  }
}

static int x11_opened = 0;
typedef struct sXport *Xport;

struct sXport
{
  Display *dpy;
  int screen;
  Colormap color_map;
  XColor fg_color;
  XColor bg_color;
  unsigned long fg_color_pixel;
  unsigned long bg_color_pixel;
  Window window;
  Cursor mouse_cursor;
  
  XFontStruct *input_font;
  XFontStruct *text_line_font;
  XFontStruct *label_font;
  XFontStruct *status_font;
  
  GC neutral_gc;
  GC normal_gc;
  GC standout_gc;
  GC input_gc;
  GC standout_input_gc;
  GC text_line_gc;
  GC text_line_standout_gc;
  GC label_gc;
  GC label_standout_gc;
  GC status_gc;
  
  struct x_window *xwins;
  int cursor_visible;		/* init to 1 */
  int redisp_needed;
  Atom wm_delete_window;
  
  /* The state of the input area. */
  struct input_view input_view;
};

#if defined(XlibSpecificationRelease) && (XlibSpecificationRelease >= 6)
static void
GetXIC(Display *theDisplay)
{
	XIMStyles	*xim_styles;
	XIMStyle	input_style = 0;
	char		*p;
	int		i, j, found = 0;
#if 0
	XVaNestedList	list;
	XFontSet	fontset;
	char		**missing_charsets;
	int		num_missing_charsets = 0;
	char		*default_string;
	long		mask;
#endif

	static char	*styles[] =
	{
		"OverTheSpot",
		"OffTheSpot",
		"Root",
		"Root",
		"Rxvt",
		NULL
	};

	static XIMStyle style_bits[] =
	{
	/* OverTheSpot */	XIMPreeditPosition | XIMStatusArea,
	/* OffTheSpot */	XIMPreeditArea | XIMStatusArea,
	/* Root */		XIMPreeditNothing | XIMStatusNothing,
	/* Not really root */	XIMPreeditNone | XIMStatusNone,
	/* See rxvt */		XIMPreeditPosition | XIMStatusNothing,
	};

	/* FIX ME this should be user configurable such as in Motif */
	static char *preeditTypes = "OffTheSpot,OverTheSpot,Root";

	if (!XSupportsLocale()) {
#ifdef	I18N_VERBOSE
		fprintf(stderr, "X doesn't support this locale\n");
#endif
	}

	if (xim == NULL && (p = XSetLocaleModifiers("@im=none")) != NULL) {
		xim = XOpenIM(theDisplay, NULL, NULL, NULL);
#ifdef	I18N_VERBOSE
		if (xim) fprintf(stderr, "XOpenIM with @im=none succeeded\n");
#endif
	}
	if (xim == NULL && (p = XSetLocaleModifiers("")) != NULL) {
		xim = XOpenIM(theDisplay, NULL, NULL, NULL);
#ifdef	I18N_VERBOSE
		if (xim) fprintf(stderr, "XOpenIM with '' succeeded\n");
#endif
	}
	if (!xim) {
#ifdef	I18N_VERBOSE
		fprintf(stderr, "Failed to open input method\n");
#endif
		return;
	}

	if (XGetIMValues(xim, XNQueryInputStyle, &xim_styles, NULL) || xim_styles == NULL) {
#ifdef	I18N_VERBOSE
		fprintf(stderr, "Input method doesn't support any style\n");
#endif
		XCloseIM(xim);
		return;
	}

	/* Match input styles between X server + input method and ourselves */
	for (i = 0, found = False; styles[i] && !found; i++) {
#ifdef	I18N_VERBOSE
		fprintf(stderr, "Trying %s (0x%lx)\n", styles[i], style_bits[i]);
#endif

		if (strstr(preeditTypes, styles[i]) == 0) {
			continue;
		}
		for (j = 0; j < xim_styles->count_styles; j++) {
			if (style_bits[i] == xim_styles->supported_styles[j]) {
				found = True;
				input_style = style_bits[i];
				break;
			}
		}
	}

	xic = XCreateIC(xim,
			XNInputStyle,		input_style,
			XNClientWindow,		thePort->window,
			XNFocusWindow,		thePort->window,
		NULL);

#ifdef	I18N_VERBOSE
	if (xic)
		fprintf(stderr, "We have an IC - input style %x window %p\n",
			input_style, thePort->window);
	else
		fprintf(stderr, "No IC\n");
#endif
}

static void
xi18nGetFocus()
{
	if (xic) {
#ifdef	I18N_VERBOSE
		fprintf(stderr, "Get Focus\n");
#endif
		XSetICFocus(xic);
	}
}

static void
xi18nLoseFocus()
{
	if (xic) {
#ifdef	I18N_VERBOSE
		fprintf(stderr, "Lose Focus\n");
#endif
		XUnsetICFocus(xic);
	}
}

static int
xi18nlookup(XEvent *evp, char *buf, int nbytes, KeySym *keysym, Status *status)
{
	int	i;
	static Status	s;

#ifdef	X_I18N
	if (xic) {
		i = XmbLookupString(xic, evp, buf, nbytes, keysym, &s);
	} else {
		i = XLookupString(evp, buf, nbytes, keysym, &s);
	}

#ifdef	I18N_VERBOSE
	fprintf(stderr, "xi18nlookup (%d bytes): %d %c\t", i,
			buf[0], isprint(buf[0]) ? buf[0] : '.');
	fprintf(stderr, "status %s",
		(s == XLookupKeySym) ? "XLookupKeySym" : 
		(s == XLookupNone) ? "XLookupNone" : 
		(s == XLookupBoth) ? "XLookupBoth" : 
		(s == XBufferOverflow) ? "XBufferOverflow" : 
		(s == XLookupChars) ? "XLookupChars" : "");
	fprintf(stderr, "\n");
#endif

#else
	i = XLookupString(evp, buf, nbytes, keysym, &s);
#endif
	if (status)	*status = s;
	return i;
}
#endif

static void
beep (Xport xport)
{
#if 0
  XBell (xport->dpy, 30);
  XFlush (xport->dpy);
#endif
}

static void
xio_bell (void)
{
  beep (thePort);
}

static void
xdraw_text_item (Xport xport, int c, int r, int wid, int hgt,
		XFontStruct *font, GC gc, XTextItem *text, int do_clip)
{
  XRectangle clip;
  int widused;
  int tab_char;
  XTextItem to_draw;

  clip.x = c;
  clip.y = r;
  clip.width = wid;
  clip.height = hgt;
  if (!x11_opened)
    return;
  if (do_clip)
    XSetClipRectangles (xport->dpy, gc, 0, 0, &clip, 1, YXBanded);


  tab_char = 0;
  to_draw = *text;
  widused = 0;
  while (to_draw.nchars)
    {
      XTextItem draw_now;
      XTextItem after_tab;

      /* Draw tab characters carefully. */
      {
	int x;
	for (x = 0; x < to_draw.nchars; ++x)
	  if (to_draw.chars[x] == '\t')
	    {
	      draw_now = to_draw;
	      draw_now.nchars = x;
	      after_tab = to_draw;
	      after_tab.chars += x + 1;
	      after_tab.nchars -= x + 1;
	      goto draw_burst;
	    }
	/* No tabs found. */
	draw_now = to_draw;
	bzero (&after_tab, sizeof (after_tab));
      }

    draw_burst:
      XDrawImageString (xport->dpy, xport->window, gc,
			c + widused, r + font->ascent,
			draw_now.chars, draw_now.nchars);
      widused += XTextWidth (font, draw_now.chars, draw_now.nchars);
      tab_char += draw_now.nchars;
      if (after_tab.chars)
	{
	  int tab_stop_col = ((tab_char + 8 - tab_char % 8)
			      * font->max_bounds.width);
	  XFillRectangle (xport->dpy, xport->window, xport->neutral_gc,
			  c + widused, r, tab_stop_col - widused, hgt);
	  widused = tab_stop_col;
	  tab_char += 8 - tab_char % 8;
	}
      to_draw = after_tab;
    }

  if (widused < wid)
    XFillRectangle (xport->dpy, xport->window, xport->neutral_gc,
		    c + widused, r, wid - widused, hgt);
  if (do_clip)
    XSetClipMask (xport->dpy, gc, None);
}

static void
xio_redraw_input_cursor (int on)
{
  struct input_view * iv = &thePort->input_view;
  int offset = iv->input_cursor - iv->visibility_begin;
  int start;
  XTextItem cursor_text;
  int cwid;
  int ypos = (iv->current_info ? 0 : Global->input);
  char * inp;

  inp = (iv->input_area
	 ? iv->input_area->buf + iv->visibility_begin
	 : "");
  start = (XTextWidth (thePort->input_font, inp, offset) + iv->prompt_wid);

  cursor_text.font = thePort->input_font->fid;
  cursor_text.nchars = 1;
  if (iv->input_cursor <= iv->visibility_end)
    {
      cwid = XTextWidth (thePort->input_font, inp + offset, 1);
      cursor_text.chars = inp + offset;
    }
  else
    {
      cwid = XTextWidth (thePort->input_font, " ", 1);
      cursor_text.chars = " ";
    }
  xdraw_text_item (thePort, start, ypos, cwid, input_rows,
		  thePort->input_font,
		  (on ? thePort->standout_input_gc : thePort->input_gc),
		  &cursor_text, 1);
}

static void
xio_cellize_cursor (void)
{
  xio_redraw_input_cursor (0);
}

static void
xio_inputize_cursor (void)
{
  xio_redraw_input_cursor (1);
}

static int
x_input_metric (str, len)
     char * str;
     int len;
{
  return XTextWidth (thePort->input_font, str, len);
}

/* This redraws the input area without recomputing anything. */
static void
xio_redraw_input (void)
{
  {
    struct input_view * iv = &thePort->input_view;
    int ypos = (iv->current_info ? 0 : Global->input);

    if (iv->redraw_needed == NO_REDRAW)
      return;
    if (iv->redraw_needed == FULL_REDRAW)
      {
	XTextItem text;
	if (iv->expanded_keymap_prompt)
	  {
	    text.font = thePort->input_font->fid;
	    text.chars = iv->expanded_keymap_prompt;
	    text.nchars = strlen (iv->expanded_keymap_prompt);
	    xdraw_text_item (thePort,
			    0, ypos, Global->scr_cols, input_rows,
			    thePort->input_font, thePort->input_gc,
			    &text, 0);
	    iv->redraw_needed = NO_REDRAW;
	    if (input_active || iv->current_info)
	      xio_redraw_input_cursor (1);
	    return;
	  }
	else if (iv->prompt_wid)
	  {
	    text.font = thePort->input_font->fid;
	    text.chars = iv->prompt;
	    text.nchars = prompt_len (text.chars);
	    xdraw_text_item (thePort, 0, ypos, iv->prompt_wid, input_rows,
			    thePort->input_font, thePort->input_gc,
			    &text, 0);
	  }
      }
    
    if (!iv->input_area
	|| (iv->visibility_begin > iv->visibility_end))
      {
	XFillRectangle (thePort->dpy, thePort->window, thePort->neutral_gc,
			iv->prompt_wid, ypos,
			Global->scr_cols - iv->prompt_wid, input_rows);
	iv->redraw_needed = NO_REDRAW;
	return;
      }
    
    
    if (iv->visibility_end >= iv->visibility_begin)
      {
	int pos = ((iv->redraw_needed == FULL_REDRAW)
		   ? iv->visibility_begin
		   : iv->redraw_needed);

	int xpos = (iv->prompt_wid
		    + XTextWidth (thePort->input_font,
				  iv->input_area->buf + iv->visibility_begin,
				  pos - iv->visibility_begin));
	int wid = Global->scr_cols - xpos;
	int hgt = input_rows;

	XTextItem text;

	text.font = thePort->input_font->fid;
	text.chars = iv->input_area->buf + pos;
	text.nchars = iv->visibility_end - pos + 1;

	xdraw_text_item (thePort, xpos, ypos, wid, hgt, 
			thePort->input_font, thePort->input_gc,
			&text, 0);

	if (input_active || iv->current_info)
	  xio_redraw_input_cursor (1);
	iv->redraw_needed = NO_REDRAW;
      }
  }
}


static void
xio_fix_input (void)
{
  iv_fix_input (&thePort->input_view);
}

static void
xio_move_cursor (void)
{
  if (input_active)
    xio_redraw_input_cursor (0);
  iv_move_cursor (&thePort->input_view);
  if (input_active)
    xio_redraw_input_cursor (1);
}

static void
xio_erase (int len)
{
  iv_erase (&thePort->input_view, len);
}

static void
xio_insert (int len)
{
  iv_insert (&thePort->input_view, len);
}
	   
static void
xio_over (char * str, int len)
{
  iv_over (&thePort->input_view, len);
}

int
io_col_to_input_pos (int c)
{
  struct input_view * iv = &thePort->input_view;
  char * prompt = (iv->expanded_keymap_prompt
		   ? iv->expanded_keymap_prompt
		   : (iv->prompt ? iv->prompt : ""));
  int prompt_wid = iv->prompt_metric (prompt, strlen(prompt));

  c -= prompt_wid;
  {
    int cpos;
    int max = iv->visibility_end - iv->visibility_begin + 1;
    for (cpos = 1; cpos <= max; ++cpos)
      if (iv->input_metric (iv->input_area->buf + iv->visibility_begin, cpos)
	  >= c)
	break;
    return iv->visibility_begin + cpos - 1;
  }
}

/****************************************************************
 * Low level Input
 ****************************************************************/

/* This is the buffer of decoded keyboard events. */
static char *input_buf = 0;
static int input_buf_allocated = 0;
static int chars_buffered = 0;
static char *se_buf = 0;
static int se_buf_allocated = 0;
static int se_chars_buffered = 0;
#define MAX_KEY_TRANSLATION	(1024)

static Status compose;

static void
xio_scan_for_input (int blockp)
{
    XEvent event_return;
    static int pendingw;
    static int pendingh;
    static int resize_pending = 0;
    int events_pending;
    int len;

    events_pending = XPending (thePort->dpy);
    do {
	if (resize_pending && !events_pending) {
	  if (((pendingh / Global->height_scale) > 9.)
			&& ((pendingw / Global->width_scale) > 9.)) {
		io_set_scr_size (pendingh, pendingw);
		resize_pending = 0;
	  }
	}

	if (!events_pending)
		io_redisp ();
      
	if (events_pending || blockp) {
	    XNextEvent (thePort->dpy, &event_return);
	    if (XFilterEvent(&event_return, None))
		continue;

	    if (event_return.xany.send_event) {
		/*
		 * Allow other XClients to send oleo commands.
		 */
		static KeySym se_keysym;

		switch (event_return.type) {
		case FocusIn:
			xi18nGetFocus();
			break;

		case FocusOut:
			xi18nLoseFocus();
			break;

		case KeyPress:
			/* Put keypress send_events aside until a <Return>. */
			if (se_chars_buffered + MAX_KEY_TRANSLATION >= se_buf_allocated) {
				se_buf_allocated = 2 * (se_buf_allocated ?
					se_buf_allocated : MAX_KEY_TRANSLATION);
				se_buf = (char *)ck_remalloc(se_buf, se_buf_allocated);
			}
			len = xi18nlookup(&event_return, se_buf + se_chars_buffered,
				MAX_KEY_TRANSLATION, &se_keysym, &compose);

			if ((len == 1) && (event_return.xkey.state & Mod1Mask))
				se_buf[se_chars_buffered] |= META_BIT;

			if (se_keysym == RetKeySym) {
				jmp_buf tmp_exception;

				bcopy(Global->error_exception, tmp_exception, sizeof(jmp_buf));
				se_buf[se_chars_buffered] = '\0';

				if (setjmp (Global->error_exception)) {
					fprintf(stderr, "Error in send_event command: %s\n", se_buf);
					se_buf[0] = '\0';
					se_chars_buffered = 0;
					set_info (0);
					pop_unfinished_command ();
					bcopy(tmp_exception, Global->error_exception,
						sizeof(jmp_buf));
					longjmp (Global->error_exception, 1); /**/
				} else {
					execute_command (se_buf);
					bcopy(tmp_exception, Global->error_exception,
						sizeof(jmp_buf));
				}
				se_buf[0] = '\0';
				se_chars_buffered = 0;
			} else {
				se_chars_buffered += len;
			}
			break;
		}
		events_pending = XPending (thePort->dpy);
		continue;
	    }
	} else {
		return;
	}

	switch (event_return.type) {
	case FocusIn:
		xi18nGetFocus();
		break;

	case FocusOut:
		xi18nLoseFocus();
		break;

	case ClientMessage:
		if (event_return.xclient.data.l[0] == thePort->wm_delete_window) {
			XCloseDisplay(thePort->dpy);
			exit(0);
		}
		break;

	case KeyPress:
	case ButtonPress:
	case ButtonRelease:
		if (chars_buffered + MAX_KEY_TRANSLATION >= input_buf_allocated) {
			input_buf_allocated = 2 * (input_buf_allocated ? input_buf_allocated
				: MAX_KEY_TRANSLATION);
			input_buf = (char *)ck_remalloc(input_buf, input_buf_allocated);
		}
	}

	switch (event_return.type) {
	case KeyPress:
		len = xi18nlookup(&event_return,input_buf + chars_buffered,
			MAX_KEY_TRANSLATION, 0, &compose);
		if ((len == 1) && (event_return.xkey.state & Mod1Mask))
			input_buf[chars_buffered] |= META_BIT;
		chars_buffered += len;
		break;
	  
	case ButtonPress:
	case ButtonRelease:
	{
		int seq = enqueue_mouse_event (event_return.xbutton.y,
				event_return.xbutton.x,
				event_return.xbutton.button,
				event_return.type == ButtonPress);
		input_buf[chars_buffered++] = MOUSE_CHAR;
		input_buf[chars_buffered++] = event_return.xbutton.button - 1 + '0';
		input_buf[chars_buffered++] = (char) seq;
		break;
	}
	  
	case Expose:
		record_damage (thePort,
			event_return.xexpose.x, event_return.xexpose.y,
			event_return.xexpose.width,
			event_return.xexpose.height);
		break;
	  
	case MapNotify:
		break;
	  
	case ReparentNotify:
		break;
	  
	case ConfigureNotify:
		pendingw = event_return.xconfigure.width;
		pendingh = event_return.xconfigure.height;
		resize_pending = 1;
		break;
	  
	case FocusIn:
		xi18nGetFocus();
		break;

	case FocusOut:
		xi18nLoseFocus();
		break;

	default:
		break;
	}
      events_pending = XPending (thePort->dpy);
    }
  while (events_pending || !chars_buffered);
}

static int
xio_input_avail (void)
{
  return chars_buffered;
}

static void
xio_wait_for_input (void)
{
  io_scan_for_input (1);
}

static int
xio_read_kbd (char *buffer, int size)
{
  int amt_read = size < chars_buffered ? size : chars_buffered;
  if (amt_read)
    bcopy (input_buf, (char *) buffer, amt_read);
  chars_buffered -= amt_read;
  if (chars_buffered)
    bcopy ((char *) input_buf + amt_read, (char *) input_buf, chars_buffered);
  return amt_read;
}

static void
xio_nodelay (int delayp)
{
  Global->block_on_getch = delayp;
}

static int
xio_getch (void)
{
  char buf;
  
  if (!chars_buffered)
    io_scan_for_input (Global->block_on_getch);
  
  if (chars_buffered)
    {
      io_read_kbd (&buf, 1);
      return buf;
    }
  else
    return 0;
}

/****************************************************************
 * Low level support for input and status areas.
 ****************************************************************/

/*
 * CRAM tells how many characters of a string will fit within
 * a given number of cols, presuming that if they don't all fit,
 * the string CONTINUE must be added to the end of those that do.
 */
static int
cram (int cols, XFontStruct *font, char *text, int len, char *continuation)
{
  int cont_cols = XTextWidth (font, continuation, strlen (continuation));
  int cols_used = 0;
  int x = 0;
  
  if (XTextWidth (font, text, len) < cols)
    return len;
  
  cols_used = 0;
  while (x < len && cols_used < (cols - cont_cols))
    cols_used += XTextWidth (font, text + x, 1);
  
  return x;
}


static void
xset_text (XTextItem *xtext, char *text, int len)
{
  if (xtext->nchars < len)
    {
      xtext->chars = ck_remalloc (xtext->chars, len);
      xtext->nchars = len;
    }
  if (len)
    bcopy (text, xtext->chars, len);
  while (len < xtext->nchars)
    xtext->chars[len++] = ' ';
}

/* The input area. */

/*
 * Low level interface to the input area specificly.
 */

static XTextItem input_text;
static int term_cursor_visible = 0;
static int input_cursor = 0;	/* Position of cursor, if visible */
static int textout;
static int input_more_mode = 0;

static void
draw_input (void)
{
  if (!input_text.chars)
    {
      XFillRectangle (thePort->dpy, thePort->window, thePort->neutral_gc,
		      Global->input, 0, Global->scr_cols, input_rows);
      return;
    }
  input_text.font = thePort->input_font->fid;
  xdraw_text_item (thePort, 0, Global->input, Global->scr_cols, input_rows, thePort->input_font,
		  thePort->input_gc, &input_text, 1);
  if (input_more_mode)
    {
      XTextItem more_text;
      int mwid = XTextWidth (thePort->input_font, "[more]", 6);
      more_text.chars = "[MORE]";
      more_text.nchars = 6;
      more_text.delta = 0;
      more_text.font = thePort->input_font->fid;
      xdraw_text_item (thePort, Global->scr_cols - mwid, Global->input, mwid, input_rows,
		      thePort->input_font, thePort->standout_input_gc,
		      &more_text, 1);
    }
  else if (term_cursor_visible)
    {
      int start = XTextWidth (thePort->input_font, input_text.chars, input_cursor);
      int cwid = XTextWidth (thePort->input_font,
			     input_text.chars + input_cursor, 1);
      XTextItem cursor_text;
      cursor_text.chars = input_text.chars + input_cursor;
      cursor_text.nchars = 1;
      cursor_text.font = thePort->input_font->fid;
      xdraw_text_item (thePort, start, Global->input, cwid, input_rows,
		      thePort->input_font, thePort->standout_input_gc,
		      &cursor_text, 1);
    }
}

static void
xio_clear_input_before (void)
{
  textout = 0;
  if (Global->topclear == 2)
    {
      int x;
      for (x = 0; x < input_text.nchars; ++x)
	input_text.chars[x] = ' ';
      input_cursor = 0;
      draw_input ();
      Global->topclear = 0;
    }
}

static void
xio_clear_input_after (void)
{
  if (Global->topclear)
    {
      int x;
      for (x = 0; x < input_text.nchars; ++x)
	input_text.chars[x] = ' ';
      input_cursor = 0;
      draw_input ();
      Global->topclear = 0;
    }
}

static void
set_input (char *text, int len, int cursor)
{
  xset_text (&input_text, text, len);
  if (cursor + 1 > input_text.nchars)
    {
      input_text.chars = (char *) ck_remalloc (input_text.chars, cursor + 1);
      while (input_text.nchars < cursor + 1)
	input_text.chars[input_text.nchars++] = ' ';
    }
  input_cursor = cursor;
  draw_input ();
}

/*
 * Low level interface to the status area specificly.
 */
static XTextItem status_text;

static void
xdraw_status (void)
{
  if (!x11_opened || thePort->input_view.current_info)
    return;
  if (user_status)
    xdraw_text_item (thePort, 0, Global->status, Global->scr_cols,
		    status_rows, thePort->status_font,
		    thePort->status_gc, &status_text, 1);
}

static void
xset_status (char *text)
{
  xset_text (&status_text, text, strlen (text));
  status_text.font = thePort->status_font->fid;
  xdraw_status ();
}

/****************************************************************
 * High level interfaces for the input and status areas.
 ****************************************************************/

static void
xio_update_status (void)
{
  CELL *cp;
  char *dec;
  char *ptr;
  int plen;
  int dlen;
  int mplen;
  char buf[1024];
  char *assembled;
  char *pos = buf;
  
  if (!user_status)
    return;
  
  if (mkrow != NON_ROW)
    {
      struct rng r;
      *pos++ = '*';
      set_rng (&r, curow, cucol, mkrow, mkcol);
      ptr = range_name (&r);
    }
  else
    ptr = cell_name (curow, cucol);
  if (*ptr)
    bcopy (ptr, pos, strlen (ptr));
  pos += strlen (ptr);
  if (how_many != 1)
    {
      sprintf (pos, " {%d}", how_many);
      pos += strlen (pos);
    }
  *pos++ = ' ';
  mplen = XTextWidth (thePort->status_font, buf, pos - buf);
  
  if ((cp = find_cell (curow, cucol)) && cp->cell_formula)
    {
      dec = decomp (curow, cucol, cp);
      dlen = XTextWidth (thePort->status_font, dec, strlen (dec));
    }
  else
    {
      dec = 0;
      dlen = 0;
    }

  ptr = cell_value_string (curow, cucol, 1);
  plen = XTextWidth (thePort->status_font, ptr, strlen (ptr));
  
  assembled = (char *) ck_malloc (plen + dlen + mplen);
  if (pos - buf)
    bcopy (buf, assembled, pos - buf);
  pos = assembled + (pos - buf);
  
  {
    int c;
    int l;
    int wid;
    
    l = strlen (ptr);
    c = cram (Global->scr_cols - mplen - thePort->status_font->max_bounds.width,
	      thePort->status_font, ptr, l, (dec ? " [...]" : "..."));
    if (c)
      bcopy (ptr, pos, c);
    pos += c;
    if (c == l)
      *pos++ = ' ';
    else if (dec)
      {
	bcopy (" [...]", pos, 6);
	pos += 6;
	dec = 0;
	dlen = 0;
	decomp_free ();
      }
    else
      {
	bcopy ("...", pos, 3);
	pos += 3;
      }
    *pos++ = ' ';
    wid = XTextWidth (thePort->status_font, assembled, pos - assembled);
    
    if (dec)
      {
	l = strlen (dec);
	c = cram (Global->scr_cols - wid, thePort->status_font, dec, l, "[...]");
	*pos++ = '[';
	if (c < l)
	  {
	    if (c)
	      bcopy (dec, pos, c);
	    bcopy ("...]", pos, 4);
	    pos += c + 4;
	  }
	else
	  {
	    if (c)
	      bcopy (dec, pos, c);
	    pos += c;
	    *pos++ = ']';
	  }
      }
    
    *pos++ = '\0';
    xset_status (assembled);
    xdraw_status ();
    free (assembled);
  }
}

static int
xio_get_chr (char *prompt)
{
  int len = strlen (prompt);
  set_input (prompt, len, len);
  Global->topclear = 2;
  draw_input ();
  return io_getch ();
}

/*
 * Multi-line informational messages to the user:
 */

/* old: */
struct text_line
{
  struct text_line *next;
  int nchars;
  int standout;
  char line[TAIL_ARRAY];
};
static int text_damaged = 0;
static struct text_line *text_lines = 0;
extern int auto_recalc;

/****************************************************************
 * Low level support for the cell windows.
 ****************************************************************/

/* Every cell has an associated font name.  This is a cache mapping
 * font names and scales to graphics contexts.
 */

struct cell_gc
{
  struct cell_gc *next;
  struct cell_gc *prev;
  Xport port;
  GC gc;
  char *font_name;
  double scale;
  XFontStruct *font;
  int clipped_to;
  int cursor:1;
};

/* The cell_gc cache is only valid for a specific size default font.  If the
 * defualt font size changes, then the cache must be flushed.
 */
static int cell_gc_basis = 12;


/* Individual cache entries are also validated by the clipping set for their
 * gc.  To the rest of the system, it appears that there is one clipping region
 * for all cell_gc's.
 */
static int clipcnt = 0;
static int cliprectc = 0;
static int cliprect_alloc = 0;
static XRectangle *cliprectv = 0;

#define GC_CACHE_SIZE	10
static struct cell_gc *cell_gc_cache = 0;

/* This takes the full name of an X11 font, and returns its point size (in
 * tenths of a point.  If the string is not a valid font name, 0 is returned.
 */

static int 
name_to_ps (char *xfont)
{
  while (*xfont)
    if (*xfont == '-')
      {
	++xfont;
	if (*xfont == '-')	/* look for -- */
	  {
	    ++xfont;
	    break;
	  }
      }
    else
      ++xfont;
  if (!*xfont)
    return 0;
  while (isdigit (*xfont++));
  --xfont;
  if (!*xfont++ == '-')
    return 0;
  return atoi (xfont);
}


#define ABS(A) ((A) < 0 ? -(A) : (A))

static struct cell_gc *
cell_gc (Xport port, struct font_memo *font_passed, int cursor)
{
  struct font_memo *font =
    (font_passed ? font_passed : default_font());
  char *font_name = font->names->x_name;
  double scale = font->scale;
  struct cell_gc *c = cell_gc_cache;
  if (cell_gc_basis != Global->cell_font_point_size)
    {
      do
	{
	  if (c->font_name)
	    {
	      XFreeFont (c->port->dpy, c->font);
	      free (c->font_name);
	    }
	  c->font_name = 0;
	  c->port = 0;
	  c->font = 0;
	  c = c->next;
	}
      while (c != cell_gc_cache);
      cell_gc_basis = Global->cell_font_point_size;
    }
  else
    {
      do
	{
	  if ((c->scale == scale)
	      && port == c->port
	      && c->font_name && !strcmp (font_name, c->font_name))
	    {
	      if (cell_gc_cache == c)
		cell_gc_cache = cell_gc_cache->next;
	      c->next->prev = c->prev;
	      c->prev->next = c->next;
	      c->next = cell_gc_cache;
	      c->prev = cell_gc_cache->prev;
	      c->prev->next = c;
	      c->next->prev = c;
	      cell_gc_cache = c;
	      goto check_clipping;
	    }
	  c = c->next;
	}
      while (c != cell_gc_cache);
    }
  
  c = c->prev;
  cell_gc_cache = c;
  if (c->font_name)
    {
      XFreeFont (c->port->dpy, c->font);
      free (c->font_name);
    }
  c->port = port;
  c->scale = scale;
  {
    char **fontv;
    int fontc;
    fontv = XListFonts (port->dpy, font_name, 1024, &fontc);
    if (fontv)
      {
	int x, best;
	int ideal_size = rint (scale * (double) Global->cell_font_point_size * 10.);
	int best_dist;
	best = 0;
	best_dist = ideal_size - name_to_ps (fontv[0]);
	for (x = 1; x < fontc; ++x)
	  {
	    int ps = name_to_ps (fontv[x]);
	    int tdist = ideal_size - ps;
	    if (ABS (tdist) < ABS (best_dist)
		|| ((ABS (tdist) == ABS (best_dist)) && (tdist > best_dist)))
	      {
		best_dist = tdist;
		best = x;
	      }
	  }
	c->font = XLoadQueryFont (port->dpy, fontv[best]);
	XFreeFontNames (fontv);
      }
    else
      c->font = 0;
  }
  if (c->font)
    c->font_name = (char *) ck_savestr (font_name);
  else
    {
      c->font = XLoadQueryFont (port->dpy, cell_font_name);
      if (c->font)
	c->font_name = ck_savestr (cell_font_name);
      else
	{
	  c->font = XLoadQueryFont (port->dpy, emergency_font_name);
	  if (c->font)
	    c->font_name = ck_savestr (emergency_font_name);
	  else
	    panic ("Unable to load even the emergency font.");
	}
    }
  
  {
    XGCValues v;
    v.font = c->font->fid;
    XChangeGC (port->dpy, c->gc, GCFont, &v);
  }
  
 check_clipping:
  
  if (clipcnt != c->clipped_to)
    {
      XSetClipRectangles (port->dpy, c->gc, 0, 0, cliprectv, cliprectc, Unsorted);
      c->clipped_to = clipcnt;
    }
  
  {
    XGCValues v;
    v.foreground = cursor ? port->bg_color_pixel : port->fg_color_pixel;
    v.background = cursor ? port->fg_color_pixel : port->bg_color_pixel;
    XChangeGC (port->dpy, c->gc, GCForeground | GCBackground, &v);
    c->cursor = cursor;
  }
  return c;
}

/* This is the data for an oleo window, displayed under X. */

enum kinds_of_layout_needed
{
  damaged_display = 1,
  new_display = 2
};

struct x_window
{
  struct x_window *next;
  struct window *win;
  struct display display;
  int layout_needed;
  int label_damage;
  Xport port;
};

static void
collect_clipping (xx_IntRectangle rect)
{
  if (cliprectc == cliprect_alloc)
    {
      cliprect_alloc = cliprect_alloc ? cliprect_alloc * 2 : 16;
      cliprectv =
	((XRectangle *)
	 ck_remalloc (cliprectv,
		      cliprect_alloc * sizeof (XRectangle)));
    }
  cliprectv[cliprectc].x = rect->x;
  cliprectv[cliprectc].y = rect->y;
  cliprectv[cliprectc].width = rect->w;
  cliprectv[cliprectc].height = rect->h;
  ++cliprectc;
}

static void 
clip_to_intrectangle (struct x_window * xwin, xx_IntRectangle rect)
{
  struct window * win = xwin->win;
  int x_ceil = win->win_over + win->numc;
  int y_ceil = win->win_down + win->numr;

  if (rect->w + rect->x >= x_ceil)
    {
      rect->w = x_ceil - rect->x;
    }
  if (rect->h + rect->y >= y_ceil)
    {
      rect->h = y_ceil - rect->y;
    }
  cliprectc = 0;
  collect_clipping (rect);
  ++clipcnt;
}

static void 
place_text (xx_IntRectangle out, struct display *disp, struct cell_display *cd, XFontStruct *font, char *string)
{
  int *widths = disp->widths;
  int *heights = disp->heights;
  int *colx = disp->colx;
  int *rowy = disp->rowy;
  struct rng *range = &disp->range;
  int hout = font->ascent + font->descent;
  int wout = XTextWidth (font, string, strlen (string));
  int ci = cd->c - range->lc;
  int ri = cd->r - range->lr;
  int yout = rowy[ri] + heights[ri] - (font->ascent + font->descent);
  int xout;
  switch (cd->justification)
    {
    default:
    case JST_LFT:
      xout = colx[ci];
      break;
    case JST_RGT:
      xout = colx[ci] + widths[ci] - 1 - wout;
      break;
    case JST_CNT:
      xout = colx[ci] + (widths[ci] - wout) / 2;
      break;
    }
  xx_IRinit (out, xout, yout, wout, hout);
}

static void
x_metric (struct cell_display *cd, struct display *disp)
{
  struct x_window *xw = (struct x_window *) disp->vdata;
  Xport port = xw->port;
  if (!cd->unclipped)
    xx_IRinit (&cd->goal, 0, 0, 0, 0);
  else
    {
      struct cell_gc *cgc = cell_gc (port, cd->font, 0);
      place_text (&cd->goal, disp, cd, cgc->font, cd->unclipped);
    }
}

static struct x_window *
x_win (Xport port, struct window *win, int rebuild)
{
  struct x_window *xw;
  
  for (xw = port->xwins; xw && xw->win != win; xw = xw->next);
  
  if (xw && !rebuild)
    return xw;
  
  if (xw)
    {
      free_display (&xw->display);
    }
  else
    {
      xw = (struct x_window *) ck_malloc (sizeof (*xw));
      xw->next = port->xwins;
      port->xwins = xw;
      xw->port = port;
    }
  xw->win = win;
  build_display (&xw->display, &win->screen, x_metric, xw);
  xw->label_damage = 1;
  xw->layout_needed = new_display;
  port->redisp_needed = 1;
  return xw;
}

static void
flush_x_windows (Xport port)
{
  while (port->xwins)
    {
      struct x_window * xw = port->xwins;
      free_display (&xw->display);
      port->xwins = xw->next;
      free (xw);
      port->redisp_needed = 1;
    }
}

static void
record_damage (Xport port, int x, int y, int w, int h)
{
  struct x_window *xwin = port->xwins;
  
  if (port->input_view.current_info)
    {
      port->input_view.info_redraw_needed = 1;
      return;
    }

  if (text_lines)
    {
      text_damaged = 1;
      return;
    }
  
  while (xwin)
    {
      struct window *win = xwin->win;
      record_display_damage (&xwin->display,
			     x - win->win_over,
			     y - win->win_down,
			     w, h);
      port->redisp_needed = 1;
      if (   (   (x + w >= win->win_over - win_label_cols(win, win->screen.hr))
	      && (x <= win->win_over)
	      && (y + h >= win->win_down - win_label_rows(win))
	      && (y <= win->win_down + display_height(&xwin->display)))
	  || (   (y + h >= win->win_down - win_label_rows(win))
	      && (y <= win->win_down)
	      && (x + w >= win->win_over - win_label_cols(win, win->screen.hr))
	      && (x <= win->win_over + display_width(&xwin->display))))
	xwin->label_damage = 1;
      xwin = xwin->next;
    }

  if (((Global->input + input_rows) >= y) && (Global->input <= y + h))
    port->input_view.redraw_needed = FULL_REDRAW;
}

static void
xio_pr_cell_win (struct window *win, CELLREF r, CELLREF c, CELL *cp)
{
  struct x_window *xwin = x_win (thePort, win, 0);
  struct display *disp = &xwin->display;
  
  if (pr_display_cell (disp, r, c, cp))
    {
      thePort->redisp_needed = 1;
      if (!xwin->layout_needed)
	xwin->layout_needed = damaged_display;
    }
}

static void
xio_repaint_win (struct window *win)
{
  if (! Global->display_opened)
	return;

  x_win (thePort, win, 1);
  XFillRectangle (thePort->dpy, thePort->window, thePort->neutral_gc,
		  win->win_over, win->win_down, win->numc, win->numr);
  record_damage (thePort, win->win_over, win->win_down, win->numc, win->numr);
}

static void
xio_repaint (void)
{
  struct window *win;

  if (! Global->display_opened)
	return;

  if (thePort->input_view.current_info)
    {
      thePort->input_view.info_redraw_needed = 1;
      return;
    }
  flush_x_windows (thePort);
  for (win = wins; win < &wins[nwin]; win++)
    xio_repaint_win (win);
}

static void
draw_cell (struct x_window *xwin, struct cell_display *cd_passed, int cursor)
{
  struct cell_display *cd = cd_passed->used_by;
  Xport port = xwin->port;
  struct window *win = xwin->win;
  int ov = win->win_over;
  int dn = win->win_down;
  CELLREF r = cd->r;
  CELLREF c = cd->c;
  struct display *disp = &xwin->display;
  int *rowy = disp->rowy;
  int *colx = disp->colx;
  int *widths = disp->widths;
  int *heights = disp->heights;
  struct cell_gc *cgc;
  int ri = r - disp->range.lr;
  int ci = c - disp->range.lc;
  
  if (!(widths[ci] && heights[ri]))
    return;
  
  if (!cd->unclipped)
    {
      cgc = cell_gc (port, default_font(), !cursor);
      XFillRectangle (port->dpy, port->window, cgc->gc,
		      colx[ci] + ov, rowy[ri] + dn,
		      widths[ci], heights[ri]);
      return;
    }
  
  cgc = cell_gc (port, cd->font, cursor);
  {
    int isclipped = 0;
    char *str;
    struct xx_sIntRectangle *strbox;
    
    isclipped = !xx_IRencloses_width (&cd->layout, &cd->goal); 
    if (!isclipped)
      {
	str = cd->unclipped;
	strbox = &cd->goal;
      }
    else
      {
	if (!cd->clipped)
	  {
	    int chr_scale = XTextWidth (cgc->font, "8", 1);
	    int w_avail = xx_IRw (&cd->layout) / chr_scale;
	    int cell_wid = widths[ci] / chr_scale;
	    CELL *cp = find_cell (r, c);
	    cd->clipped =
	      ck_savestr (adjust_prc (cd->unclipped, cp, w_avail, cell_wid,
				      cd->justification));
	    place_text (&cd->clip, disp, cd, cgc->font, cd->clipped);
	  }
	str = cd->clipped;
	strbox = &cd->clip;
      }
    XDrawImageString (port->dpy, port->window, cgc->gc,
		      strbox->x + ov, strbox->y + cgc->font->ascent + dn,
		      str, strlen (str));
    {
      struct xx_sIntRectangle tofill[4];
      int fillc;
      
      fillc = xx_IRsubtract (tofill, &cd->layout, strbox); 
      {
	XGCValues v;
	v.foreground = cursor ? port->fg_color_pixel : port->bg_color_pixel;
	v.background = cursor ? port->bg_color_pixel : port->fg_color_pixel;
	XChangeGC (port->dpy, cgc->gc, GCForeground | GCBackground, &v);
	cgc->cursor = !cursor;
      }
      while (fillc--)
	XFillRectangle (port->dpy, port->window, cgc->gc,
			tofill[fillc].x + ov, tofill[fillc].y + dn,
			tofill[fillc].w, tofill[fillc].h);
    }
  }
}

/* Cell values */

static void
set_cursor (int on)
{
  struct x_window *xwin = x_win (thePort, cwin, 0);
  struct display *disp = &xwin->display;
  int *rowy = disp->rowy;
  int *colx = disp->colx;
  int *widths = disp->widths;
  int *heights = disp->heights;
  int ri = curow - disp->range.lr;
  int ci = cucol - disp->range.lc;
  int ov = xwin->win->win_over;
  int dn = xwin->win->win_down;
  struct cell_display *cd = cell_display_of (disp, curow, cucol);
  struct xx_sIntRectangle clip;

  /* The cursor might not even be visible.  Though commands should never leave
   * the user in such a state, it can happen in several ways transiently.
   */
  if (!cd)
    return;

  thePort->cursor_visible = on;
  
  if (xwin->layout_needed == new_display)
    return;
  
  if (cd->used_by == cd)
    {
      clip = cd->layout;
      clip.x += ov;
      clip.y += dn;
    }
  else
    xx_IRinit (&clip,
	       colx[ci] + ov, rowy[ri] + dn,
	       widths[ci], heights[ri]);
  clip_to_intrectangle (xwin, &clip);
  draw_cell (xwin, cd, on);
}


static void
xio_hide_cell_cursor (void)
{
  if (thePort->cursor_visible)
    set_cursor (0);
}

static void
xio_display_cell_cursor (void)
{
  if (!thePort->cursor_visible)
    {
      if (   (curow < cwin->screen.lr)
	  || (cucol < cwin->screen.lc)
	  || (curow > cwin->screen.hr)
	  || (cucol > cwin->screen.hc))
	io_recenter_cur_win ();
      set_cursor (1);
    }
}

static int xx[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};


static void
draw_labels (void)
{
  struct x_window *xwin;
  XRectangle rect;
  for (xwin = thePort->xwins; xwin; xwin = xwin->next)
    if (xwin->win->lh_wid && xwin->label_damage)
      {
	XGCValues gcv;
	struct window *win = xwin->win;
	int wid2 = win->lh_wid / 2;
	if (xx[0]) {
	  XFillRectangle (thePort->dpy, thePort->window,
			  thePort->label_standout_gc,
			  win->win_over - win->lh_wid,
			  win->win_down - label_rows,
			  wid2, label_rows);
	};
	gcv.line_width = win->lh_wid;
	gcv.cap_style = CapRound;
	XChangeGC (thePort->dpy, thePort->label_gc,
		   GCLineWidth | GCCapStyle, &gcv);
	if (xx[1]) {
	  XDrawLine (thePort->dpy, thePort->window, thePort->label_gc,
		     win->win_over - wid2,
		     win->win_down - label_rows + wid2,
		     win->win_over - wid2,
		     win->win_down + win->numr - wid2);
	  
	};
	gcv.cap_style = CapButt;
	XChangeGC (thePort->dpy, thePort->label_gc, GCCapStyle, &gcv);
	if (xx[2]) {
	  XDrawLine (thePort->dpy, thePort->window, thePort->label_gc,
		     win->win_over - wid2,
		     win->win_down + wid2,
		     win->win_over - wid2,
		     win->win_down + win->numr);
	};
	
	if (xx[3]) {
	  XFillRectangle (thePort->dpy, thePort->window, thePort->label_gc,
			  win->win_over - wid2,
			  win->win_down - label_rows,
			  win->numc + wid2,
			  label_rows);
	};
	
	rect.x = 0;
	rect.y = 0;
	rect.width = Global->scr_cols;
	rect.height = Global->scr_lines;
	XSetClipRectangles (thePort->dpy, thePort->label_standout_gc,
			    0, 0, &rect, 1, YXBanded);
	{
	  char buf[100];
	  CELLREF cr;
	  int x;
	  int len;
	  
	  sprintf (buf, "#%d", 1 + win - wins);
	  for (x = len = 0; buf[x]; ++x)
	    {
	      int cwid = XTextWidth (thePort->label_font, buf, 1);
	      if (cwid + len > wid2)
		break;
	      len += cwid;
	    }
	  XDrawImageString (thePort->dpy, thePort->window,
			    thePort->label_standout_gc,
			    win->win_over - wid2,
			    win->win_down - thePort->label_font->descent,
			    buf, x);
	  
	  rect.x = win->win_over - win->lh_wid;
	  rect.y = win->win_down;
	  rect.width = win->lh_wid;
	  rect.height = win->numr;
	  XSetClipRectangles (thePort->dpy, thePort->label_standout_gc,
			      0, 0, &rect, 1, YXBanded);
	  
	  x = win->win_down + thePort->label_font->ascent;
	  for (cr = win->screen.lr; cr <= win->screen.hr; ++cr)
	    {
	      int hgt = get_scaled_height (cr);
	      if (hgt)
		{
		  if (Global->a0)
		    sprintf (buf, "%d", cr);
		  else
		    sprintf (buf, "R%d", cr);
		  XDrawImageString (thePort->dpy, thePort->window,
				    thePort->label_standout_gc,
				    win->win_over - win->lh_wid, x,
				    buf, strlen (buf));
		  x += hgt;
		}
	      if (cr == MAX_ROW)
		break;
	    }
	  rect.x = 0;
	  rect.y = 0;
	  rect.width = Global->scr_cols;
	  rect.height = Global->scr_lines;
	  XSetClipRectangles (thePort->dpy, thePort->label_standout_gc,
			      0, 0, &rect, 1, YXBanded);
	  
	  x = win->win_over;
	  for (cr = win->screen.lc; cr <= win->screen.hc; ++cr)
	    {
	      int wid = get_scaled_width (cr);
	      if (wid > win->numc)
		wid = win->numc;
	      if (wid)
		{
		  int txtwid;
		  char *ptr;
		  if (Global->a0)
		    ptr = col_to_str (cr);
		  else
		    {
		      ptr = buf;
		      sprintf (ptr, "C%u", cr);
		    }
		  txtwid = XTextWidth (thePort->label_font, ptr, strlen (ptr));
		  if (txtwid > wid - thePort->label_font->max_bounds.width)
		    {
		      int txtlen =
			((wid - thePort->label_font->max_bounds.width)
			 / XTextWidth (thePort->label_font, "#", 1));
		      txtwid = txtlen * XTextWidth (thePort->label_font,
						    "#", 1);
		      buf[txtlen] = 0;
		      while (txtlen)
			buf[txtlen--] = '#';
		      ptr = buf;
		    }
		  XDrawImageString (thePort->dpy, thePort->window,
				    thePort->label_standout_gc,
				    x + (wid - txtwid) / 2,
				    (win->win_down
				     - thePort->label_font->descent),
				    ptr, strlen (ptr));
		  x += wid;
		}
	      if (cr == MAX_COL)
		break;
	    }
	}
	xwin->label_damage = 0;
      }
}

/* Refresh the existing image. */


static void
xio_redisp (void)
{
  static int was_text = 0;
  if (thePort->input_view.current_info)
    {
      if (!was_text)
	thePort->input_view.redraw_needed = FULL_REDRAW;

      if (thePort->input_view.redraw_needed != NO_REDRAW)
	xio_redraw_input ();

      if (thePort->input_view.info_redraw_needed)
	{
	  int ipos = thePort->input_view.info_pos;
	  int top = ipos + (Global->scr_lines - input_rows) / info_rows - 1;
	  int ypos;
	  XTextItem text;

	  if (top >= thePort->input_view.current_info->len)
	    top = thePort->input_view.current_info->len - 1;

	  ypos = input_rows;

	  text.font = thePort->text_line_font->fid;
	  while (ipos <= top)
	    {
	      text.chars = thePort->input_view.current_info->text[ipos];
	      text.nchars = strlen (text.chars);
	      xdraw_text_item (thePort, 0, ypos, Global->scr_cols, input_rows,
			      thePort->text_line_font, thePort->text_line_gc,
			      &text, 0);
	      ypos += info_rows;
	      ++ipos;
	    }
	  XFillRectangle (thePort->dpy, thePort->window,
			  thePort->neutral_gc, 0, ypos,
			  Global->scr_cols, Global->scr_lines - ypos - 1);
	  thePort->input_view.redraw_needed = FULL_REDRAW;
	  xio_redraw_input ();
	  thePort->input_view.info_redraw_needed = 0;
	  was_text = 1;
	}
    }
  else
    {
      struct x_window *xwin;
      if (was_text)
	{
	  thePort->input_view.info_redraw_needed = 0;
	  was_text = 0;
	  io_repaint ();
	}
      else
	{
	  struct rng * rng = &cwin->screen;
	  if (   (curow > rng->hr)
	      || (curow < rng->lr)
	      || (cucol > rng->hc)
	      || (cucol < rng->lc))
	    io_recenter_cur_win ();
	}
      thePort->redisp_needed = 0;
      if (thePort->redisp_needed != NO_REDRAW)
	xio_redraw_input ();
      xdraw_status ();
      draw_labels ();
      for (xwin = thePort->xwins; xwin; xwin = xwin->next)
	{
	  struct display *disp = &xwin->display;
	  int ov = xwin->win->win_over;
	  int dn = xwin->win->win_down;
	  struct cell_display * cd;
	  struct cell_display * cursor_cd = 0;
	  int must_draw_cursor;
	  
	  if (xwin->layout_needed)
	    {
	      layout (disp);
	      xwin->layout_needed = 0;
	    }
	  
	  /* If the cursor cell has been damaged, 
	   * it will be redrawn.  However, if the 
	   * cursor cell is empty, then redrawing
	   * some other cell might damage the cursor.
	   * This watches for that condition and
	   * redraws the cursor if it occurs.
	   */
	  must_draw_cursor = 0;
	  if (thePort->cursor_visible
	      && (xwin->win == cwin)
	      && (curow >= disp->range.lr)
	      && (curow <= disp->range.hr)
	      && (cucol >= disp->range.lc)
	      && (cucol <= disp->range.hc))
	    {
	      cursor_cd = cell_display_of (disp, curow, cucol);
	      /* If the cursor cell is not empty, we never have
	       * to explicitly redraw the cursor.
	       */
	      if (cursor_cd->used_by == cursor_cd)
		cursor_cd = 0;
	    }
	  
	  cd = disp->damaged;
	  while (cd != (struct cell_display *) disp)
	    {
	      struct xx_sIntRectangle clip;
	      struct cell_display *owner = cd->used_by;
	      clip = owner->layout;
	      clip.x += ov;
	      clip.y += dn;
	      clip_to_intrectangle (xwin, &clip);
	      if (cursor_cd && (cd->used_by == cursor_cd->used_by))
		must_draw_cursor = 1;
	      draw_cell (xwin, owner,
			 (thePort->cursor_visible
			  && (xwin->win == cwin)
			  && (owner->r == curow)
			  && (owner->c == cucol)));
	      {	
		struct cell_display *cdt = cd;
		cd = cd->next_damaged;
		cdt->next_damaged = 0;
	      }
	    }
	  disp->damaged = (struct cell_display *) disp;
	  if (must_draw_cursor)
	    set_cursor (1);
	}
    }
}

static XFontStruct *
reasonable_font (Xport port, char *name)
{
  XFontStruct *f = XLoadQueryFont (port->dpy, name);
  
  if (!f)
    {
      fprintf(stderr, "(warning) Font %s could not be loaded.\n", name);
      f = XLoadQueryFont (port->dpy, default_font_name);
      if (!f)
	{
	  panic ("Default font %s could not be loaded.\n",
		 default_font_name);
	  exit(0);
	}
    }
  return f;
}

extern void 
xio_open_display (void)
{
  XGCValues gcv;
  XWMHints wmhints;
  
  thePort = (Xport) ck_malloc (sizeof (*thePort));
  thePort->cursor_visible = 1;
  thePort->redisp_needed = 1;
  thePort->xwins = 0;
  
  thePort->dpy = theDisplay;

  bzero (&thePort->input_view, sizeof (struct input_view));
  thePort->input_view.prompt_metric = x_input_metric;
  thePort->input_view.input_metric = x_input_metric;
  
  thePort->screen = DefaultScreen (thePort->dpy);
  thePort->color_map = DefaultColormap (thePort->dpy, thePort->screen);

  /* First try to allocate the color.  If that doesn't work, go ahead */
  /* and set it to black on white.  (white and black are always in    */
  /* the colormap, so if the color names are set with those values,   */
  /* the first test should succeed.  This replaces a bunch of tests   */
  /* that were in the 1.5 release.       PGA                          */
  
  if (default_fg_color_name && 
      XParseColor (thePort->dpy, thePort->color_map, 
                   default_fg_color_name, &thePort->fg_color) &&
      XAllocColor(thePort->dpy, thePort->color_map, &thePort->fg_color))
    thePort->fg_color_pixel = thePort->fg_color.pixel;
  else
    {
    thePort->fg_color_pixel = BlackPixel (thePort->dpy, thePort->screen);
    fprintf(stderr, "Unable to allocate colorcell for %s.  Using black instead.\n", default_fg_color_name);
    }
      	
  if (default_bg_color_name && 
      XParseColor (thePort->dpy, thePort->color_map,
		   default_bg_color_name, &thePort->bg_color) &&
      XAllocColor(thePort->dpy, thePort->color_map, &thePort->bg_color))
    thePort->bg_color_pixel = thePort->bg_color.pixel;
  else
    {
    thePort->bg_color_pixel = WhitePixel (thePort->dpy, thePort->screen);
    fprintf(stderr, "Unable to allocate colorcell for %s.  Using white instead.\n", default_bg_color_name);
    }
    
  /* May have a situation where could allocate one color, but not the other */
  if (thePort->fg_color_pixel == thePort->bg_color_pixel)
      {
      thePort->fg_color_pixel = BlackPixel (thePort->dpy, thePort->screen);      	
      thePort->bg_color_pixel = WhitePixel (thePort->dpy, thePort->screen);
      fprintf(stderr, "Foreground and background are identical.  Using black on white.\n");
      }
      
  thePort->window =
    XCreateSimpleWindow (thePort->dpy, DefaultRootWindow (thePort->dpy),
			 geom_x, geom_y, geom_w, geom_h,
			 1, thePort->fg_color_pixel, thePort->bg_color_pixel);
  /* FIXME Version number goes here */
  XStoreName (thePort->dpy, thePort->window, "Oleo!!");
  wmhints.flags = InputHint;
  wmhints.input = True;
  XSetWMHints(thePort->dpy, thePort->window, &wmhints);

  thePort->wm_delete_window = XInternAtom(thePort->dpy,
					  "WM_DELETE_WINDOW", False);
#ifdef HAVE_XSETWMPROTOCOLS
  XSetWMProtocols (thePort->dpy, thePort->window,
		   &thePort->wm_delete_window, 1);
#endif
  
#if defined(XlibSpecificationRelease) && (XlibSpecificationRelease >= 6)
  GetXIC(theDisplay);
#endif

  gcv.foreground = thePort->bg_color_pixel;
  gcv.background = thePort->bg_color_pixel;
  thePort->neutral_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground), &gcv);
  
  gcv.foreground = thePort->fg_color_pixel;
  gcv.background = thePort->bg_color_pixel;
  thePort->normal_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground), &gcv);
  
  gcv.foreground = thePort->bg_color_pixel;
  gcv.background = thePort->fg_color_pixel;
  thePort->standout_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground), &gcv);
  
  thePort->input_font = reasonable_font (thePort, input_font_name);
  gcv.font = thePort->input_font->fid;
  gcv.foreground = thePort->fg_color_pixel;
  gcv.background = thePort->bg_color_pixel;
  thePort->input_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont), &gcv);
  
  gcv.foreground = thePort->bg_color_pixel;
  gcv.background = thePort->fg_color_pixel;
  thePort->standout_input_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont), &gcv);
  
  gcv.foreground = thePort->fg_color_pixel;
  gcv.background = thePort->bg_color_pixel;
  thePort->status_font = reasonable_font (thePort, status_font_name);
  gcv.font = thePort->status_font->fid;
  thePort->status_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont), &gcv);
  
  thePort->label_font = reasonable_font (thePort, label_font_name);
  gcv.font = thePort->label_font->fid;
  gcv.cap_style = CapRound;
  thePort->label_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont | GCCapStyle), &gcv);
  
  gcv.background = thePort->fg_color_pixel;
  gcv.foreground = thePort->bg_color_pixel;
  thePort->label_standout_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont | GCCapStyle), &gcv);
  
  gcv.background = thePort->bg_color_pixel;
  gcv.foreground = thePort->fg_color_pixel;
  thePort->text_line_font = reasonable_font (thePort, text_line_font_name);
  gcv.font = thePort->text_line_font->fid;
  thePort->text_line_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont), &gcv);

  info_rows = (thePort->text_line_font->max_bounds.ascent
	       + thePort->text_line_font->max_bounds.descent);
  
  gcv.background = thePort->fg_color_pixel;
  gcv.foreground = thePort->bg_color_pixel;
  thePort->text_line_standout_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont), &gcv);
  
  gcv.background = thePort->fg_color_pixel;
  gcv.foreground = thePort->bg_color_pixel;
  thePort->text_line_standout_gc =
    XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
	       (GCForeground | GCBackground | GCFont), &gcv);
  
  gcv.background = thePort->bg_color_pixel;
  gcv.foreground = thePort->fg_color_pixel;
  
  {
    int x;
    for (x = 0; x < GC_CACHE_SIZE; ++x)
      {
	struct cell_gc *cg =
	  (struct cell_gc *) ck_malloc (sizeof (struct cell_gc));
	cg->font_name = 0;
	cg->font = 0;
	cg->clipped_to = 0;
	cg->cursor = 0;
	cg->gc = XCreateGC (thePort->dpy, DefaultRootWindow (thePort->dpy),
			    (GCForeground | GCBackground), &gcv);
	if (!cell_gc_cache)
	  {
	    cg->next = cg;
	    cg->prev = cg;
	  }
	else
	  {
	    cg->next = cell_gc_cache;
	    cg->prev = cg->next->prev;
	    cg->next->prev = cg;
	    cg->prev->next = cg;
	  }
	cell_gc_cache = cg;
      }
  }
  
  XSelectInput (thePort->dpy, thePort->window,
		(ExposureMask | StructureNotifyMask | KeyPressMask | FocusChangeMask
		 | ButtonPressMask));
  
  io_init_windows (geom_h, geom_w, 1, 2,
		   thePort->input_font->ascent + thePort->input_font->descent,
		   (thePort->status_font->ascent
		    + thePort->status_font->descent),
		   thePort->label_font->ascent + thePort->label_font->descent,
		   thePort->label_font->max_bounds.width);
  x11_opened = 1;
  
  {
    struct cell_gc *cgc = cell_gc (thePort, default_font(), 0);
    Global->height_scale = cgc->font->ascent + cgc->font->descent;
    Global->width_scale = XTextWidth (cgc->font, "M", 1);
  }

  /* Setup the arrow keys.  Modifiers effect the last character
   * of the translation.
   */
  {
    static int meta_shift_ctrl[]
      = {Mod1MapIndex, ShiftMapIndex, ControlMapIndex, -1};
#define shift_ctrl &meta_shift_ctrl[1]
#define ctrl  &meta_shift_ctrl[2]
    static int meta_shift[] = {Mod1MapIndex, ShiftMapIndex, -1};
#define shift &meta_shift[1]
    static int ctrl_meta[] = {ControlMapIndex, Mod1MapIndex, -1};
#define meta &ctrl_meta[1]

    static int * mod_combos [] =
      {
	0,
	ctrl, shift, meta,
	shift_ctrl, ctrl_meta, meta_shift,
	meta_shift_ctrl
      };
    static char meta_modp [] = { 0, 0, 0, 1, 0, 1, 1, 1 };
    static char base_char [] =
      {
	'A', '\001', 'a', 'A',
	'\001', '\001', 'a', '\001'
      };
    /* In ansi order: */
    static KeySym arrows[] = {XK_Up, XK_Down, XK_Right, XK_Left};
    int arrow;

	/* Figure out the KeySyms for our modifiers */
	XModifierKeymap *OurModKeymap;
	KeySym *ModifierKeys;

	/* First get the modifiers */
	OurModKeymap = XGetModifierMapping(thePort->dpy);
	ModifierKeys = malloc(OurModKeymap->max_keypermod * sizeof(KeySym) * 8);

    for (arrow = 0; arrow < 4; ++arrow)
      {
	int mod;
	for (mod = 0; mod < 8; ++mod)
	  {
	    char string[10];
	    KeyCode *tmpptr;
	    int *modptr;
	    int i, n;

	    /* Build list of appropriate KeyCodes */
	    n = 0;
	    /* Point to index of first wanted modifier */
	    modptr = mod_combos[mod];
	    if (modptr != 0) {
	    while (*modptr != -1) {
			/* Point to wanted modifier */
			tmpptr = &(OurModKeymap->modifiermap[OurModKeymap->max_keypermod * (*modptr)]);
			/* Loop through all keycodes in map */
			for (i=0; i < OurModKeymap->max_keypermod; i++) {
				if (tmpptr[i] != 0) {
					ModifierKeys[n] = 
					XKeycodeToKeysym(thePort->dpy,tmpptr[i], 0);
					n++; 
				} 
			}
			modptr++;	/* Next modifier */
		}
	    }
		
	    sprintf (string, "\033[%s%c",
		     meta_modp[mod] ? "\033" : "",
		     base_char[mod] + arrow);
	    XRebindKeysym (thePort->dpy, arrows[arrow],
			   ModifierKeys, n,
			   string, strlen(string));
	  }
      }
	XFreeModifiermap(OurModKeymap);
	free(ModifierKeys);
  }
  
  XMapWindow (thePort->dpy, thePort->window);
}


extern void
xio_close_display(int e)
{
  XCloseDisplay (thePort->dpy);
  x11_opened = 0;
}

static void
xio_flush (void)
{
  XFlush (theDisplay);
}


void
xio_command_loop (int i)
{
  command_loop (i, 0);
}

void
x11_graphics (void)
{
  io_command_loop = xio_command_loop;
  io_open_display = xio_open_display;
  io_redisp = xio_redisp;
  io_repaint = xio_repaint;
  io_repaint_win = xio_repaint_win;
  io_close_display = xio_close_display;
  io_input_avail = xio_input_avail;
  io_scan_for_input = xio_scan_for_input;
  io_wait_for_input = xio_wait_for_input;
  io_read_kbd = xio_read_kbd;
  io_nodelay = xio_nodelay;
  io_getch = xio_getch;
  io_bell = xio_bell;
  io_get_chr = xio_get_chr;
  io_update_status = xio_update_status;
  io_fix_input = xio_fix_input;
  io_move_cursor = xio_move_cursor;
  io_erase = xio_erase;
  io_insert = xio_insert;
  io_over = xio_over;
  io_flush = xio_flush;
  io_clear_input_before = xio_clear_input_before;
  io_clear_input_after = xio_clear_input_after;
  io_pr_cell_win = xio_pr_cell_win;
  io_hide_cell_cursor = xio_hide_cell_cursor;
  io_display_cell_cursor = xio_display_cell_cursor;
  io_cellize_cursor = xio_cellize_cursor;
  io_inputize_cursor = xio_inputize_cursor;
}

void
set_x_default_point_size (int l)
{

/* If the display is not open (ie, we're running on a non x screen, */
/* silently return - allows stuff in startup file to work right.PGA */

  if (!x11_opened) return;
  
  if (l > 4)
    {
      Global->cell_font_point_size = l;
      {
	struct cell_gc *cgc = cell_gc (thePort, default_font(), 0);
	Global->height_scale = cgc->font->ascent + cgc->font->descent;
	Global->width_scale = cgc->font->max_bounds.width;
      }
      io_set_scr_size (Global->scr_lines, Global->scr_cols);
    }
}


#endif /* X_DISPLAY_MISSING */
#endif	/* HAVE_MOTIF */
