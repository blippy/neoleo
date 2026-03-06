/* Copywrite 1997 Free Software Foundation, Inc.

This file is part of Oleo, the GNU Spreadsheet.

Oleo is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later verion.

oleo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Oleo; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBGTK
#include "gtk/gtk.h"
#include "io-abstract.h"
#include "input.h"
#include "cmd.h"
#include "io-abstract.h"
#include "basic.h"
#include "io-gtk.h"
#include "io-utils.h"
#include "regions.h"

/* FIXME - Put this in an .oleorc file */

static char *emergency_font_name = "8x13";
static char *cell_font_name = "times_roman12";
static char *default_font_name = "8x13";
static char *input_font_name = "8x13";
static char *status_font_name = "6x10";
static char *text_line_font_name = "8x13";
static char *label_font_name = "5x8";
static char *default_bg_color_name = "black";
static char *default_fg_color_name = "white";


static GdkPixmap *pixmap = NULL;
static GtkWidget *mainWindow;
static struct sGport *gtkPort;
typedef struct sGport *TgtkPort;

void
gdestroy_window (GtkWidget *widget,
		GtkWidget **window)
{
	*window = NULL;
}

int
g_input_metric (char *str, int len)
{
	/* FIXME: Should match x_input_metric function */
	return 5;
}
void
gfile_selection_ok (GtkWidget *widget, GtkFileSelection *file)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: %s\n", gtk_file_selection_get_filename (GTK_FILE_SELECTION (file)));
#endif
	gdestroy_window (NULL, &widget);
}

void
gfile_selection ()
{
	GtkWidget *window = NULL;

#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: In gfile_selection\n");
#endif
	
	window = gtk_file_selection_new ("file selection dialog");
	gtk_window_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_signal_connect (GTK_OBJECT (window), "destroy",
			    (GtkSignalFunc) gdestroy_window,
			    &window);

	gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (window)->ok_button),
			    "clicked", (GtkSignalFunc) gfile_selection_ok,
			    GTK_OBJECT (window));

	gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (window)->cancel_button),
			    "clicked", (GtkSignalFunc) gtk_widget_destroy,
			    GTK_OBJECT (window));

	gtk_widget_show (window);

}


static void
gdraw_text_item(GtkWidget *widget, int start, int ypos, int cwid, int input_rows, GdkFont *font, gtkTextItem *cursor_text, int a)
{

		gdk_draw_text(widget->window,
			      font,
			      widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			      start,  
			      (ypos + 10), /* FIXME - make calculated! */
			      cursor_text->text,
			      cursor_text->lentext);

}

static void
gio_redraw_input_cursor (GtkWidget *widget, int on)
{
	struct input_view * iv = &gtkPort->input_view;
	int offset = iv->input_cursor - iv->visibility_begin;
	int start = 0;
	int cwid = 0;
	int ypos = (iv->current_info ? 0 : input);
	char * inp;
	gtkTextItem cursor_text;

	inp = (iv->input_area
		? iv->input_area->buf + iv->visibility_begin
		: "");
	start = 5;

	cursor_text.lentext = 1;
	cursor_text.text = " ";

#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: Before gdraw_text_item\n");
#endif
	gdraw_text_item((GtkWidget*)widget, start, ypos, cwid, input_rows,
			gtkPort->input_font, &cursor_text, 1);
} 

static void
gio_redraw_input (GtkWidget *widget)
{
	struct input_view * iv = &gtkPort->input_view;
	int ypos = (iv->current_info ? 0 : input);
	gtkTextItem text;

#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: Beginning of gio_redraw_input\n");
#endif

	/* FIXME */
	if (iv->redraw_needed == NO_REDRAW) return;
	if (iv->redraw_needed == FULL_REDRAW) {
#ifdef DEBUG
		fprintf(stderr, "io-gtk.c: In FULL_REDRAW\n");
#endif
		if (iv->expanded_keymap_prompt) {
			fprintf(stderr, "io-gtk.c: iv->expanded_keymap_prompt\n");
			text.text = iv->expanded_keymap_prompt;
			text.lentext = strlen (iv->expanded_keymap_prompt);
			gdraw_text_item((GtkWidget*)widget, 0, ypos,
			iv->prompt_wid, input_rows, gtkPort->input_font,
			&text, 0);
			iv->redraw_needed = NO_REDRAW;
			gio_redraw_input_cursor ((GtkWidget*)widget, 1);
			return;	
		} else if (iv->prompt_wid) {
#ifdef DEBUG
			fprintf(stderr, "io-gtk.c: Not FULL_REDRAW, Not NO_REDRAW\n");
#endif
			text.text = iv->prompt;
			text.lentext = prompt_len (text.text);
			gdraw_text_item((GtkWidget*)widget, 0, ypos,
			iv->prompt_wid, input_rows, gtkPort->input_font, 
			&text, 0);
		}
	}			

}

gint
gio_expose_backing (GtkWidget *widget, GdkEventExpose *event)
{
	gtkTextItem cursor_text;

#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: Beginning of gio_expose_backing\n");
#endif

	gdk_draw_pixmap(widget->window,
			widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			pixmap,
			event->area.x, event->area.y,
			event->area.x, event->area.y,
			event->area.width, event->area.height);

	gio_redraw_input((GtkWidget*)widget);

	io_update_status();

	return FALSE;
}

static gint
gio_configure_backing (GtkWidget *widget, GdkEventConfigure *event)
{
	if (pixmap) {
		gdk_pixmap_unref (pixmap);
	}
	
	pixmap = gdk_pixmap_new(widget->window,
				widget->allocation.width,
				widget->allocation.height,
				-1);
	
	gdk_draw_rectangle (pixmap,
			    widget->style->white_gc,
			    TRUE,
			    0, 0,
			    widget->allocation.width,
			    widget->allocation.height);

	return TRUE;
}

static GdkFont *
reasonable_font (char *fontname)
{
	/* FIXME - needs flushing out */
	GdkFont *font = gdk_font_load (fontname);

	return font;
}


void
gio_open_display()
{
	GtkWidget *window;
	GtkWidget *menu;
	GtkWidget *menu_bar;
	GtkWidget *root_menu;
	GtkWidget *menu_items;
	GtkWidget *box1;
	GtkWidget *box2;
	GtkWidget *box3;
	GtkWidget *scrollbar;
	GtkWidget *drawing_area;
	GtkObject *adjustment;

#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: In gio_open_display\n");
#endif
	
	gtkPort = (TgtkPort) ck_malloc (sizeof (*gtkPort));

	gtkPort->cursor_visible = 1;
	gtkPort->redisp_needed = 1;

	bzero (&gtkPort->input_view, sizeof (struct input_view));

	gtkPort->input_view.prompt_metric = g_input_metric;
	gtkPort->input_view.input_metric = g_input_metric;
	
	gtkPort->input_font = reasonable_font (input_font_name);

	gtkPort->status_font = reasonable_font (status_font_name);

	gtkPort->label_font = reasonable_font (label_font_name);

	gtkPort->text_line_font = reasonable_font (text_line_font_name);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW (window), GNU_PACKAGE);
	gtk_container_border_width (GTK_CONTAINER (window), 0);
	gtk_signal_connect (GTK_OBJECT (window), "destroy",
			    GTK_SIGNAL_FUNC (kill_oleo), NULL);

	box1 = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), box1);
	gtk_widget_show (box1);

	box2 = gtk_vbox_new (FALSE, 0);
	gtk_container_border_width (GTK_CONTAINER (box2), 0);
	gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
	gtk_widget_show (box2);

	menu_bar = gtk_menu_bar_new ();
	gtk_box_pack_start (GTK_BOX (box2), menu_bar, FALSE, TRUE, 0);
	gtk_widget_show (menu_bar);

	menu = gtk_menu_new ();
	root_menu = gtk_menu_item_new_with_label("File");
	gtk_widget_show (root_menu);
	menu_items = gtk_menu_item_new_with_label("New");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label("Open");
	gtk_menu_append(GTK_MENU (menu), menu_items);
	gtk_signal_connect (GTK_OBJECT (menu_items), "activate",
			    (GtkSignalFunc) gfile_selection,
			    NULL);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label("Close");
	gtk_menu_append(GTK_MENU (menu), menu_items);
	gtk_signal_connect (GTK_OBJECT (menu_items), "activate",
			    (GtkSignalFunc) upper_right,
			    NULL);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label("Save");
	gtk_menu_append(GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label("Save As");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label("Print");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label("Exit");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_signal_connect (GTK_OBJECT (menu_items), "activate",
			(GtkSignalFunc) kill_oleo,
			NULL);
	gtk_widget_show (menu_items);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (root_menu), menu);
	gtk_menu_bar_append (GTK_MENU_BAR (menu_bar), root_menu);

	menu = gtk_menu_new ();
	root_menu = gtk_menu_item_new_with_label ("Edit");
	gtk_widget_show (root_menu);
	menu_items = gtk_menu_item_new_with_label ("Copy");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label ("Paste");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show(menu_items);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (root_menu), menu);
	gtk_menu_bar_append (GTK_MENU_BAR (menu_bar), root_menu);
	menu = gtk_menu_new ();
	root_menu = gtk_menu_item_new_with_label ("Help");
	gtk_widget_show (root_menu);
	menu_items = gtk_menu_item_new_with_label ("Help");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label ("About");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (root_menu), menu);
	gtk_menu_item_right_justify (GTK_MENU_ITEM (root_menu));
	gtk_menu_bar_append (GTK_MENU_BAR (menu_bar), root_menu);

	box3 = gtk_hbox_new (FALSE, 0);
	gtk_container_border_width (GTK_CONTAINER (box3), 0);
	gtk_box_pack_start (GTK_BOX (box2), box3, TRUE, TRUE, 0);
	gtk_widget_show (box3);

	drawing_area = gtk_drawing_area_new ();
	gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area), 675, 350);
	gtk_box_pack_start (GTK_BOX (box3), drawing_area, TRUE, TRUE, 0);
	gtk_widget_show (drawing_area);

	/* FIXME remove global variables!!! UGH!!*/
	mainWindow = drawing_area;
	
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: Before first signal connect\n");
#endif

	gtk_signal_connect (GTK_OBJECT (drawing_area), "configure_event",
			    (GtkSignalFunc) gio_configure_backing, NULL);
	gtk_signal_connect (GTK_OBJECT (drawing_area), "expose_event",
			    (GtkSignalFunc) gio_expose_backing, NULL);

	gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK
			       | GDK_PROXIMITY_OUT_MASK);

	adjustment = gtk_adjustment_new (0.0, 0.0, 101.0, 0.1, 1.0, 1.0);
	scrollbar = gtk_vscrollbar_new (GTK_ADJUSTMENT (adjustment));
	gtk_range_set_update_policy (GTK_RANGE (scrollbar), GTK_UPDATE_CONTINUOUS);
	gtk_box_pack_start (GTK_BOX (box3), scrollbar, FALSE, FALSE, 0);
	gtk_widget_show (scrollbar);

	adjustment = gtk_adjustment_new (0.0, 0.0, 101.0, 0.1, 1.0, 1.0);
	scrollbar = gtk_hscrollbar_new (GTK_ADJUSTMENT (adjustment));
	gtk_range_set_update_policy (GTK_RANGE (scrollbar), GTK_UPDATE_CONTINUOUS);
	gtk_box_pack_start (GTK_BOX (box2), scrollbar, FALSE, FALSE, 0);
	gtk_widget_show (scrollbar);

	gtk_widget_show (window);

	io_init_windows (25, 80, 1, 2, 1, 1, 1, 1);

}

void
gio_inputize_cursor(void)
{
	gio_redraw_input_cursor (mainWindow, 1);
}

void
gio_cellize_cursor(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_cellize_cursor\n");
#endif
}

void
gio_hide_cell_cursor(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_hide_cell_cursor\n");
#endif
}

void
gio_pr_cell_win(struct window *win, CELLREF r, CELLREF c, CELL *cp)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_pr_cell_win\n");
#endif
}

void
gio_clear_input_after(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_clear_input_after\n");
#endif
}

void
gio_clear_input_before(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_clear_input_before\n");
#endif
}

void
gio_flush(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_flush\n");
#endif
}

void
gio_over(char * str, int len)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_over\n");
#endif

}

void
gio_insert(int len)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_insert\n");
#endif
}

void
gio_erase(int len)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_erase\n");
#endif
}

void
gio_move_cursor(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_move_cursor\n");
#endif
}

void
gio_fix_input(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: Entering gio_fix_input\n");
#endif
	iv_fix_input (&gtkPort->input_view);
}

gtkTextItem status_text;

void
gset_text (gtkTextItem *Textstruct, char *text, int len)
{
	if (Textstruct->lentext < len) {
		Textstruct->text = ck_remalloc (Textstruct->text, len);
		Textstruct->lentext = len;
	}
	if (len)
	   bcopy (text, Textstruct->text, len);
	while (len < Textstruct->lentext)
	       Textstruct->text[len++] = ' ';
}

void
gdraw_status (void)
{
	if (user_status)
		gdraw_text_item((GtkWidget*)mainWindow, 0, status, 0,
				0, gtkPort->status_font, &status_text, 0);
}

void
gset_status (char *text)
{
	gset_text (&status_text, text, strlen (text));
	gdraw_status ();
}

static void
gio_update_status(void)
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

	if (mkrow != NON_ROW) {
		struct rng r;
		*pos++ = '*';
		set_rng (&r, curow, cucol, mkrow, mkcol);
		ptr = range_name (&r);
	} else {
		ptr = cell_name (curow, cucol);
	}
	if (*ptr)
	   bcopy (ptr, pos, strlen (ptr));
	pos += strlen (ptr);
	if (how_many != 1) {
		sprintf (pos, " {%d}", how_many);
		pos += strlen (pos);
	}
	*pos++ = ' ';
	mplen = pos - buf;

	if ((cp = find_cell (curow, cucol)) && cp->cell_formula) {
		dec = decomp (curow, cucol, cp);
		dlen = strlen (dec);
	} else {
		dec = 0;
		dlen = 0;
	}
	ptr = cell_value_string (curow, cucol, 1);
	plen = strlen (ptr);

	assembled = (char *) ck_malloc (plen + dlen + mplen);
	if (pos - buf)
	   bcopy (buf, assembled, pos - buf);
	pos = assembled + (pos - buf);

	*pos++ = '\0';
	gset_status (assembled); 
	gdraw_status ();
	free (assembled);
}

int
gio_get_chr(char *prompt)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_get_chr\n");
#endif
}

void
gio_bell(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_bell\n");
#endif
}

int
gio_getch(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_getch\n");
#endif
}

void
gio_nodelay(int delayp)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_nodelay\n");
#endif
}

int
gio_read_kbd(char *buffer, int size)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_read_kbd\n");
#endif
}

void
gio_wait_for_input(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_wait_for_input\n");
#endif
}

void
gio_scan_for_input(int blockp)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_scan_for_input\n");
#endif
}

int
gio_input_avail(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_input_avail\n");
#endif
}

void
gio_close_display(void)
{
	gtk_exit(0);
}

void
gio_repaint_win(struct window *win)
{
	/* FIXME - jb flush out */
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: In gio_repaint_win\n");
#endif
	gtkPort->input_view.redraw_needed = FULL_REDRAW;	
}

void
gio_repaint(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_repaint\n");
#endif
}

void
gio_redisp(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_redisp\n");
#endif
}

void
gio_display_cell_cursor(void)
{
#ifdef DEBUG
	fprintf(stderr, "io-gtk.c: STUB gio_display_cell_cursor\n");
#endif
}

void
gio_command_loop(int i)
{
	gtk_main();
}

void
gtk_graphics()
{
	io_command_loop = gio_command_loop;
	io_open_display = gio_open_display;
	io_redisp = gio_redisp;
	io_repaint = gio_repaint;
	io_repaint_win = gio_repaint_win;
	io_close_display = gio_close_display;
	io_input_avail = gio_input_avail;
	io_scan_for_input = gio_scan_for_input;
	io_wait_for_input = gio_wait_for_input;
	io_read_kbd = gio_read_kbd;
	io_nodelay = gio_nodelay;
	io_getch = gio_getch;
	io_bell = gio_bell;
	io_get_chr = gio_get_chr;
	io_update_status = gio_update_status;
	io_fix_input = gio_fix_input;
	io_move_cursor = gio_move_cursor;
	io_erase = gio_erase;
	io_insert = gio_insert;
	io_over = gio_over;
	io_flush = gio_flush;
	io_clear_input_before = gio_clear_input_before;
	io_clear_input_after = gio_clear_input_after;
	io_pr_cell_win = gio_pr_cell_win;
	io_hide_cell_cursor = gio_hide_cell_cursor;
	io_cellize_cursor = gio_cellize_cursor;
	io_inputize_cursor = gio_inputize_cursor;
	io_display_cell_cursor = gio_display_cell_cursor;
}

#endif /* Have HAVE_LIBGTK */

