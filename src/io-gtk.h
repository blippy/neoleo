#ifndef IO_X11H
#define IO_X11H

#ifdef HAVE_LIBGTK

struct sGport {
	int cursor_visible;
	int redisp_needed;
	struct input_view input_view;
	GdkFont *input_font;
	GdkFont *text_line_font;
	GdkFont *label_font;
	GdkFont *status_font;
};

struct gtkHandle {
	GtkWidget *mainWindow;
};

typedef struct {
	char *text;
	int lentext;
} gtkTextItem;

#endif /* HAVE_LIBGTK */

#endif /* IO_X11H */
