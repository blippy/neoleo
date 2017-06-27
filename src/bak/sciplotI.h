/*
 * $Header: /cvs/oleo/src/sciplotI.h,v 1.3 2000/08/10 21:02:49 danny Exp $
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

#ifndef __SCIPLOT_I_H__
#define __SCIPLOT_I_H__

/* #include "sys-defines.h"	*/
#include "plot.h"

/**************************************************************************/
/* In both C and C++, support the `bool' datatype.                        */
/**************************************************************************/

/* we are logical */
#ifdef __cplusplus 
#ifndef HAVE_BOOL       /* old C++ compiler, must declare bool */
typedef enum { false = 0, true = 1 } bool;
#endif  
#else  /* not __cplusplus */ 
#ifdef __STDC__
typedef enum { false = 0, true = 1 } bool;
#else  /* not __STDC__, do things the old-fashioned way */
typedef int bool;
#define false 0
#define true 1
#endif
#endif /* not __cplusplus */

/* ____P() is a macro used in our source code to wrap function prototypes,
   so that compilers that don't understand ANSI C prototypes still work,
   and ANSI C compilers can issue warnings about type mismatches. */
#ifdef ____P 
#undef ____P
#endif
#ifdef _HAVE_PROTOS 
#undef _HAVE_PROTOS
#endif
#if defined (__STDC__) || defined (_AIX) \
        || (defined (__mips) && defined (_SYSTYPE_SVR4)) \
        || defined(WIN32) || defined(__cplusplus)
#ifdef _SUPPRESS_PROTOS
#define ____P(protos) ()
#else  /* not _SUPPRESS_PROTOS */ 
#define ____P(protos) protos
#define _HAVE_PROTOS 1
#endif /* not _SUPPRESS_PROTOS */
#else 
#define ____P(protos) ()
#endif

#ifdef __GNUC__
#define DMAX(a,b) ({double _a = (a), _b = (b); _a > _b ? _a : _b; })
#define DMIN(a,b) ({double _a = (a), _b = (b); _a < _b ? _a : _b; })
#define IROUND(x) ({double _x = (x); int _i; \
		if (_x >= INT_MAX) _i = INT_MAX; \
		else if (_x <= -(INT_MAX)) _i = -(INT_MAX); \
		else _i = (_x > 0.0 ? (int)(_x + 0.5) : (int)(_x - 0.5)); \
		_i;})
#else
#define DMAX(a,b) ((a) > (b) ? (a) : (b))
#define DMIN(a,b) ((a) < (b) ? (a) : (b))
#define IROUND(x) ((int) ((x) > 0 ? (x) + 0.5 : (x) - 0.5))
#endif



/* we use floating point libplot coordinates in the range [0,PLOT_SIZE] */
#define PLOT_SIZE 4096.0

/* The x_trans and y_trans elements of a Multigrapher specify the current
   linear transformation from user coordinates to device coordinates.  They
   are used both in the plotting of a graph frame, and in the plotting of
   data points within a graph. */

typedef struct
{
  /* Input (user) coordinates, all floating point.  These are the
     coordinates used in the original data points (or their base-10 logs,
     for an axis of log type).  We'll map them to the unit interval
     [0.0,1.0]. */
  double input_min, input_max;	/* min, max */
  double input_range;		/* max - min, precomputed for speed */
  /* If we're reversing axes, we'll then map [0.0,1.0] to [1.0,0.0] */
  bool reverse;
  /* We'll map [0.0,1.0] to another (smaller) interval, linearly */
  double squeezed_min, squeezed_max; /* min, max */
  double squeezed_range;	/* max - min */
  /* Output [i.e., libplot] coordinates.  The interval [0.0,1.0] will be
     mapped to this range, and the squeezed interval to a sub-range.  This
     is so that the box within which points are plotted will be smaller
     than the full area of the graphics display. */
  double output_min, output_max; /* min */
  double output_range;		/* max - min */
} Transform;

/* The `x_axis' and `y_axis' elements of a Multigrapher, which are of type
   `Axis', specify the layout of the two axes of a graph.  They are used in
   the drawing of a graph frame.  All elements that are doubles are
   expressed in user coordinates (unless the axis is logarithmic, in which
   case logs are taken before this structure is filled in). */

/* The `x_axis' and `y_axis' elements are filled in by calls to
   prepare_axis() when set_graph_parameters() is called.  The only
   exceptions to this are the elements `max_width' and `non_user_ticks',
   which are filled in by draw_frame_of_graph(), as the frame for a graph
   is being drawn. */

typedef struct 
{
  const char *font_name;	/* fontname for axis label and tick labels */
  double font_size;		/* font size for axis label and tick labels */
  const char *label;		/* axis label (a string) */
  int type;			/* axis layout type (A_LINEAR or A_LOG10) */
  double tick_spacing;		/* distance between ticks */
  int min_tick_count, max_tick_count; /* tick location = count * spacing */
  bool have_lin_subticks;	/* does axis have linearly spaced subticks? */
  double lin_subtick_spacing;	/* distance between linearly spaced subticks */
  int min_lin_subtick_count, max_lin_subtick_count; 
  bool have_normal_subsubticks; /* does axis have logarithmic subsubticks?*/
  bool user_specified_subsubticks; /* axis has user-spec'd subsubticks? */
  double subsubtick_spacing;	/* spacing for user-specified ones */
  double other_axis_loc;	/* location of intersection w/ other axis */
  double alt_other_axis_loc;	/* alternative loc. (e.g. right end vs. left)*/
  bool switch_axis_end;		/* other axis at right/top, not left/bottom? */
  bool omit_ticks;		/* just plain omit them (and their labels) ? */
  double max_label_width;	/* max width of labels placed on axis, in
				   libplot coors (we update this during
				   plotting, for y axis only) */
  int labelled_ticks;		/* number of labelled ticks, subticks, and
				   subsubticks drawn on the axis
				   (we update this during plotting, so we
				   can advise the user to specify a tick
				   spacing by hand if labelled_ticks <= 2) */

  /*
   * Custom ticks
   */
  tick_type_e		tick_type;
  axis_xlate_tick	xlate_tick;
  char			*tick_format;
} Axis;


/*
 * The Multigrapher structure.
 * A pointer to one of these is passed as the first argument to
 *	each Multigrapher method (e.g., plot_point()).
 */

struct MultigrapherStruct
{
  /* multigrapher parameters (not updated over a multigrapher's lifetime) */
  plPlotter	*plotter;		/* GNU libplot Plotter handle */
  const char	*display_type;		/* type of libplot device driver [unused] */
  const char	*bg_color;		/* color of background, if non-NULL */
  bool		save_screen;		/* erase display when opening plotter? */
  SpPlotType	plot_type;		/* SP_PLOT_XY, SP_PLOT_BAR, SP_PLOT_PIE, .. */

  /* graph parameters (constant over any single graph) */
  Transform	x_trans, y_trans;	/* user->device coor transformations */
  Axis		x_axis, y_axis;		/* information on each axis */
  grid_type	grid_spec;		/* frame specification */
  double	blankout_fraction;	/* 1.0 means blank whole box before drawing */
  bool		no_rotate_y_label;	/* useful for pre-X11R6 X servers */
  double	tick_size;		/* fractional tick size */
  double	subtick_size;		/* fractional subtick size (for linear axes) */
  double	frame_line_width;	/* fractional width of lines in the frame */
  double	half_line_width;	/* approx. half of this, in libplot coors */
  const char	*frame_color;		/* color for frame (and graph, if no -C) */
  const char	*title;			/* graph title */
  const char	*title_font_name;	/* font for graph title */
  double	title_font_size;	/* fractional height of graph title */
  int		clip_mode;		/* 0, 1, or 2 (cf. clipping in gnuplot) */

  /* These are here just to support transposed axes */
  bool		transpose_axes;		/* Whether to do it */
  Axis		x_axis_orig, y_axis_orig;

  /* Input parameters */
  double	x_min, x_max, x_spacing, x_subsubtick_spacing;
  double	y_min, y_max, y_spacing, y_subsubtick_spacing;
  int		user_specified_subsubticks,
		round_to_next_tick,
		log_axis,
		reverse_axis,
		switch_axis_end,
		omit_ticks;

  /*
   * following elements are updated during plotting of points;
   * they're the chief repository for internal state
   */
  bool		first_point_of_polyline;	/* true only at beginning of each polyline */
  double	oldpoint_x, oldpoint_y;	/* last-plotted point */
  int		symbol;			/* symbol being plotted at each point */
  int		linemode;		/* linemode used for polyline */

  int		datasetnum;			/* Easy way to keep track of datasets */
  Point		point;

  /*
   * For plots that require a set of points to be remembered.
   *	Examples are PIE.
   */
  int		npoints;
  Point		*data;

  /*
   * Legend
   */
  int		nlegend;
  char		**legend;
};

static void sp_bar_plot_point(Multigrapher *mg, const Point *point);
static void sp_pie_plot_point(Multigrapher *mg, const Point *point);

static void sp_xy_begin_graph(Multigrapher *multigrapher, double scale,
	double trans_x, double trans_y);
static void sp_pie_begin_graph(Multigrapher *mg, double scale, double x, double y);
static void sp_bar_begin_graph(Multigrapher *mg, double scale, double x, double y);

static void sp_xy_end_graph(Multigrapher *mg);
static void sp_pie_end_graph(Multigrapher *mg);
static void sp_bar_end_graph(Multigrapher *mg);
#endif	/* __SCIPLOT_I_H__ */
