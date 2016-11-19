/*
 * $Header: /cvs/oleo/src/sciplot.h,v 1.6 2000/11/22 19:33:01 danny Exp $
 *
 * This file is part of GNU plotutils.
 *
 * Copyright © 2000 Free Software Foundation, Inc.
 * Written by Danny Backx <danny@gnu.org>, 2000.
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#ifndef	__SCIPLOT_H__
#define	__SCIPLOT_H__

#include <stdio.h>
#include <plot.h>

/*
 * Enumerations and macros for parameter values
 */
typedef enum SpPlotType {
	SP_PLOT_NONE = 0,
	SP_PLOT_XY,
	SP_PLOT_BAR,
	SP_PLOT_PIE,

	SP_PLOT_LAST	/* Don't add any after this */
} SpPlotType;

/* style of graph frame; the 1st four of these are increasingly fancy, but
   the last (AXES_AT_ORIGIN) is an altogether different style */
typedef enum
{
  NO_AXES = 0, AXES = 1, AXES_AND_BOX = 2, AXES_AND_BOX_AND_GRID = 3, AXES_AT_ORIGIN = 4
} grid_type;

/* bit fields in portmanteau variables */
enum { X_AXIS = 0x1, Y_AXIS = 0x2 };

#define NO_OF_LINEMODES 5	/* see linemode.c */
#define MAX_COLOR_NAME_LEN 32	/* long enough for all of libplot's colors */

/* types of line */
extern const char *linemodes[NO_OF_LINEMODES];
extern const char *colorstyle[NO_OF_LINEMODES];

/*
 * Structures used in the API
 */
typedef struct MultigrapherStruct Multigrapher;

/* Definition of the Point structure.  The point-reader (in reader.c)
   returns a list of these from a specified input stream, and the
   multigrapher (in plotter.c) interprets them as polyline vertices, and
   plots the resulting polyline(s).  Each polyline comprises a run of
   points, each (except the first) connected to the previous point,
   provided that `pendown' is true.  The final seven fields should be the
   same for each point in a polyline. */

typedef struct
{
	double	x, y;		/* location of the point in user coordinates */
	int	have_x_errorbar, have_y_errorbar;
	double	xmin, xmax;	/* meaningful only if have_x_errorbar field is set */
	double	ymin, ymax;	/* meaningful only if have_y_errorbar field is set */
	int	pendown;	/* connect to previous point? (if false, polyline ends) */
/* following fields are polyline attributes: constant over a polyline */
	int	symbol;		/* either a number indicating which standard marker
				 * symbol is to be plotted at the point (<0 means none)
				 * or an character to be plotted, depending on the value:
				 * 0-31: a marker number, or 32-up: a character. */
	double	symbol_size;	/* symbol size, as frac. of size of plotting area */
	const char *symbol_font_name; /* font from which symbols >= 32 are taken */
	int	linemode;	/* linemode of polyline (<0 means no polyline) */
	double	line_width;	/* line width as fraction of size of the display */
	double	fill_fraction;	/* in interval [0,1], <0 means polyline isn't filled */
	int	use_color;	/* color/monochrome interpretation of linemode */

	char	*label;		/* For pie charts */
	int	dataset;	/* For XY, bar charts */
} Point;

/*
 * Public functions
 */
Multigrapher * sp_create_plot(plPlotter *plotter, const SpPlotType	plot_type);

void sp_destroy_plot(Multigrapher *);

void sp_begin_plot(Multigrapher *mg, double scale, double trans_x, double trans_y);
void sp_end_plot(Multigrapher *);

void sp_set_parameters(void);	/* set all parameters */
void sp_set_plot_type(Multigrapher *, SpPlotType);	/* Set the plot type to e.g. Bar Chart */

void sp_draw_frame(Multigrapher *multigrapher, int draw_canvas);
void sp_plot_point(Multigrapher *multigrapher, const Point *point);
void sp_plot_point_simple(Multigrapher *mg, int connected, double x, double y);

void sp_flush(Multigrapher *);		/* ??? end_polyline_and_flush */

void sp_set_title(Multigrapher *mg, const char *title);
void sp_set_axis_title(Multigrapher *mg, const int axis, const char *title);
void sp_set_axis_range(Multigrapher *mg, int axis, double min, double max, double spacing, int log_axis);

void sp_first_dataset(Multigrapher *mg);
void sp_next_dataset(Multigrapher *mg);

void sp_setcolor(Multigrapher *mg, char *color);

/*
 * Legend
 */
void sp_legend_draw(Multigrapher *mg);
void sp_legend_label(Multigrapher *mg, int i, char *s);

/*
 * Ticks on axes
 */

typedef enum {
	SP_TICK_DEFAULT = 0,
	SP_TICK_PRINTF = 1,
	SP_TICK_STRFTIME = 2,
	SP_TICK_CUSTOM = 3,
	SP_TICK_NONE = 4,

	SP_TICK_LAST
} tick_type_e;


typedef char *(*axis_xlate_tick)(double value);
void sp_set_axis_ticktype(Multigrapher *mg, int axis,
	double round_to, double incr, axis_xlate_tick xlate_tick);

/*
 * Historic functions
 */
void begin_graph(Multigrapher *multigrapher, double scale, double trans_x, double trans_y);
void end_graph(Multigrapher *mg);
Multigrapher *new_multigrapher(const char *display_type, const char *bg_color,
	const char *bitmap_size, const char *max_line_length,
	const char *meta_portable, const char *page_size, const char *rotation_angle,
	int save_screen);

int delete_multigrapher (Multigrapher *multigrapher);

void set_graph_parameters(Multigrapher *multigrapher, double frame_line_width,
	const char *frame_color, const char *title, const char *title_font_name,
	double title_font_size, double tick_size, grid_type grid_spec, double x_min,
	double x_max, double x_spacing, double y_min, double y_max, double y_spacing,
	int spec_x_spacing, int spec_y_spacing, double width, double height, double up,
	double right, const char *x_font_name, double x_font_size, const char *x_label,
	const char *y_font_name, double y_font_size, const char *y_label,
	int no_rotate_y_label, int log_axis, int round_to_next_tick,
	int switch_axis_end, int omit_labels, int clip_mode, double blankout_fraction,
	int transpose_axes);

void draw_frame_of_graph(Multigrapher *multigrapher, int draw_canvas);

void plot_point(Multigrapher *multigrapher, const Point *point);

void plot_point_array(Multigrapher *multigrapher, const Point *p, int length);
void end_polyline_and_flush(Multigrapher *multigrapher);
void sp_set_axis_label_font_size(Multigrapher *mg, int axis, double s);

/* Don't add anything after this */

#endif	/* __SCIPLOT_H__ */
