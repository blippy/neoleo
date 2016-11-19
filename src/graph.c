#define	GRAPH_VERBOSE
/*
 * Copyright © 1993, 1999, 2000, 2001 Free Software Foundation, Inc.
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

/*
 * $Id: graph.c,v 1.22 2001/02/13 23:38:05 danny Exp $
 *
 * This file contains the functions to maintain the internal graphing
 * data structures in Oleo.
 */

#undef	GRAPH_VERBOSE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include <ctype.h>
#include <stdlib.h>

#include "global.h"
#include "graph.h"
#include "cmd.h"
#include "line.h"
#include "io-term.h"
#include "info.h"
#include "cell.h"
#include "regions.h"
#include "ref.h"
#include "io-utils.h"

char * graph_axis_name [graph_num_axis] =
{
  "x", "y"
};

static char * graph_plot_styles [] =
{
  "lines",
  "points",
  "linespoints",
  "impulses",
  "dots",
   0
};

int graph_ornt_row_magic[graph_num_pair_orientations] = { 0, 1, 0 };
int graph_ornt_col_magic[graph_num_pair_orientations] = { 1, 0, 0 };

enum graph_axis
chr_to_axis (int c)
{
  if (isupper (c))
    c = tolower (c);
  switch (c)
    {
    case 'x':
      return graph_x;
    case 'y':
      return graph_y;
    }
  io_error_msg ("Unkown graph-axis `%c' (try `x' or `y')", c);
  return graph_x;		/* not reached, actualy. */
}

char *
line_to_q_char (struct line line)
{
  static struct line tmp_line;
  char *str;
  
  str = line.buf;

  set_line (&tmp_line, "\"");
  while (*str)
    {
      switch (*str)
	{
	case '"':
/* gnuplot doesn't like escaped quotation marks in titles anyway.
 * If you want to support them, you'll have to protect against
 * progressive buildup, since this stores the post-escaping string
 * back into the original char.
 */
/*        catn_line (&tmp_line, "\\\"", 2); */
	  break;
	default:
	  catn_line (&tmp_line, str, 1);
	  break;
	}
      ++str;
    }
  catn_line (&tmp_line, "\"", 1);

  set_line(&line, tmp_line.buf);

  return line.buf;
}

char *
char_to_q_char (char *str)
{
  char *tmp;

  static struct line tmp_line;
  set_line (&tmp_line, str);
  tmp = line_to_q_char (tmp_line);
  return (tmp);
}

void
graph_set_axis_title (int axis_c, char * title)
{
  enum graph_axis axis = chr_to_axis (axis_c);
#ifdef	GRAPH_VERBOSE
  fprintf(stderr, "graph_set_axis_title(%c,%s)\n", axis_c, title);
#endif
  set_line (&Global->PlotGlobal->graph_axis_title [axis], char_to_q_char (title));
}

char *
graph_get_axis_title (int axis_c)
{
  enum graph_axis axis = chr_to_axis (axis_c);
  return Global->PlotGlobal->graph_axis_title[axis].buf;
}

void
graph_set_logness (int axis_c, int explicit, int newval)
{
  enum graph_axis axis = chr_to_axis (axis_c);
  static struct line msg_buf;

  if (!explicit)
    newval = !Global->PlotGlobal->graph_logness [axis];
  else
    newval = (newval > 0);

  Global->PlotGlobal->graph_logness [axis] = newval;
  sprint_line (&msg_buf, "%slogarithmic %s%s",
	       ((Global->PlotGlobal->graph_logness [graph_x] || Global->PlotGlobal->graph_logness [graph_y])
		? "" : "no"),
	       Global->PlotGlobal->graph_logness[graph_x] ? "x" : "",
	       Global->PlotGlobal->graph_logness[graph_y] ? "y" : "");
#if 0
  io_info_msg ("set %s", msg_buf.buf);
#endif
}

int
graph_get_logness(int axis_c)
{
  enum graph_axis axis = chr_to_axis (axis_c);
  return Global->PlotGlobal->graph_logness [axis];
}

void
graph_check_range (char * val)
{
  if (says_default (val))
    return;
  while (isspace (*val)) ++val;
  if (*val == '-') ++val;
  while (isspace (*val)) ++val;
  if (!isdigit (*val))
    io_error_msg
      ("Illegal range specifier %s (should be a number or `def').", val);
  else
    {
      while (*val)
	if (isspace (*val) || isdigit (*val))
	  ++val;
	else
	  break;
      while (isspace (*val)) ++val;
      if (*val == '.') ++val;
      while (*val)
	if (isspace (*val) || isdigit (*val))
	  ++val;
	else
	  io_error_msg
	    ("Illegal range specifier %s (should be a number or `def').", val);
    }
	    
}


void
graph_set_axis_lo (int axis_c, char * val)
{
  enum graph_axis axis = chr_to_axis (axis_c);
#ifdef	GRAPH_VERBOSE
  fprintf(stderr, "graph_set_axis_lo(%c,%s)\n", axis_c, val);
#endif
  graph_check_range (val);
  set_line (&Global->PlotGlobal->graph_rng_lo [axis], val);
  Global->PlotGlobal->XYMin[axis] = atof(val);
  Global->PlotGlobal->graph_axis_symbols [axis].lr = NON_ROW;
}


void
graph_set_axis_hi (int axis_c, char * val)
{
  enum graph_axis axis = chr_to_axis (axis_c);
#ifdef	GRAPH_VERBOSE
  fprintf(stderr, "graph_set_axis_hi(%c,%s)\n", axis_c, val);
#endif
  graph_check_range (val);
  set_line (&Global->PlotGlobal->graph_rng_hi [axis], val);
  Global->PlotGlobal->XYMax[axis] = atof(val);
  Global->PlotGlobal->graph_axis_symbols [axis].lr = NON_ROW;
}

void
graph_default_axis_labels (int axis_c)
{
  enum graph_axis axis = chr_to_axis (axis_c);
  Global->PlotGlobal->graph_axis_labels [axis].lr = NON_ROW;
}

int
graph_check_style (char * name)
{
  int x =
    words_member (graph_plot_styles, parray_len (graph_plot_styles), name);
  if (x < 0)
    io_error_msg
      ("Invalid line style %s. (Try M-x describe-function set-graph-style).");
  return x;
}


void
graph_set_style(int data_set, char * style)
{
  if ((data_set < 0) || (data_set >= NUM_DATASETS))
    io_error_msg
      ("set-graph-style -- data-set out of range: %d (should be in [0..%d])",
       data_set, NUM_DATASETS);
#if 0
  {
  int x = graph_check_style (style);
  set_line (&Global->PlotGlobal->graph_style[data_set], graph_plot_styles [x]);
  }
#endif
}

void
graph_set_data_title (int data_set, char * title)
{
  if ((data_set < 0) || (data_set >= NUM_DATASETS))
    io_error_msg
      ("set-graph-title -- data-set out of range: %d (should be in [0..%d])",
       data_set, NUM_DATASETS);
#ifdef	GRAPH_VERBOSE
  fprintf(stderr, "graph_set_data_title(%d,%s)\n", data_set, title);
#endif
  set_line (&Global->PlotGlobal->graph_title [data_set], title);
}

char *
graph_get_data_title(int data_set)
{
  if ((data_set < 0) || (data_set >= NUM_DATASETS))
    return NULL;
  return Global->PlotGlobal->graph_title[data_set].buf;
}

void
plotutils_set_data(int data_set, struct rng *rng)
{
  if ((data_set < 0) || (data_set >= NUM_DATASETS))
    io_error_msg
      ("set-graph-data -- data-set out of range: %d (should be in [0..%d])",
       data_set, NUM_DATASETS);
  Global->PlotGlobal->graph_data[data_set] = *rng;
}

void
graph_set_data(int data_set, struct rng * rng)
{
  if ((data_set < 0) || (data_set >= NUM_DATASETS))
    io_error_msg
      ("set-graph-data -- data-set out of range: %d (should be in [0..%d])",
       data_set, NUM_DATASETS);
  Global->PlotGlobal->graph_data[data_set] = *rng;
}

struct rng
graph_get_data(int data_set)
{
	return Global->PlotGlobal->graph_data[data_set];
}


void
graph_presets (void)
{
  enum graph_axis axis;

  if (using_curses) {
	if (getenv("DISPLAY")) {
		/*
		 * We're using curses in an X environment, let's
		 * default to Tektronix on stdout.
		 */
		plotutils_tek();
		plotutils_set_filename("-");
	} else {
		graph_postscript ("#plot.ps", 'd', 'm', "TimesRoman", 12);
	}
  } else {
#if 0
    graph_x11_mono ();
#endif
  }

    for (axis = graph_x; axis < graph_num_axis; ++axis)
      {
	int axis_c = graph_axis_name [axis][0];
	Global->PlotGlobal->graph_logness [axis] = 0;
	graph_set_axis_title (axis_c, "");
	graph_set_axis_lo (axis_c, "def");
	graph_set_axis_hi (axis_c, "def");
	graph_set_axis_title (axis_c, "");
	graph_default_axis_labels (axis_c);
      }
}

void
graph_clear_datasets (void)
{
  int x;
  for (x = 0; x < NUM_DATASETS; ++x)
    {
      graph_set_style(x, "lines");
      graph_set_data_title(x, "");
      Global->PlotGlobal->graph_data[x].lr = NON_ROW;
    }
}


void
init_graphing (void)
{
  PlotInit();

  graph_presets ();
  graph_clear_datasets ();

  XYxAuto = XYyAuto = 1;
}

void
for_pairs_in (struct rng * rng, enum graph_pair_ordering order, fpi_thunk thunk, void * frame)
{
  CELLREF r;
  CELLREF c;
  enum graph_pair_orientation ornt = order % graph_num_pair_orientations;
  enum graph_ordering dir = ORDER_OF_PAIRS(order);
  int r_inc = 1 + graph_ornt_row_magic [ornt];
  int c_inc = 1 + graph_ornt_col_magic [ornt];
  if (dir == graph_rows)
    {
      r = rng->lr;
      while (1)
	{
	  c = rng->lc;
	  while (1)
	    {
	      CELLREF y_r = r + graph_ornt_row_magic [ornt];
	      CELLREF y_c = c + graph_ornt_col_magic [ornt];
	      CELL * cell = find_cell (y_r, y_c);
	      thunk (frame, cell, y_r, y_c);
	      if ((rng->hc - c) < c_inc)
		break;
	      c += c_inc;
	    }
	  if ((rng->hr - r) < r_inc)
	    break;
	  r += r_inc;
	}
    }
  else
    {
      c = rng->lc;
      while (1)
	{
	  r = rng->lr;
	  while (1)
	    {
	      CELLREF y_r = r + graph_ornt_row_magic [ornt];
	      CELLREF y_c = c + graph_ornt_col_magic [ornt];
	      CELL * cell = find_cell (y_r, y_c);
	      thunk (frame, cell, y_r, y_c);
	      if ((rng->hr - r) < r_inc)
		break;
	      r += r_inc;
	    }
	  if ((rng->hc - c) < c_inc)
	    break;
	  c += c_inc;
	}
    }
}

static char *graph_plot_title = NULL;

void
graph_set_title(char *t)
{
	if (graph_plot_title)
		free(graph_plot_title);
	graph_plot_title = strdup(t);
#ifdef	GRAPH_VERBOSE
	fprintf(stderr, "graph_set_title(%s)\n", t);
#endif
}

char *
graph_get_title(void)
{
	return	graph_plot_title;
}

void
graph_set_axis_auto(int axis, int set)
{
	if (axis == 0  || axis == 1)
		Global->PlotGlobal->XYAuto[axis] = set;
	else {
		io_error_msg("Invalid graph axis %d, must be 0 or 1", axis);
	}
#ifdef	GRAPH_VERBOSE
	fprintf(stderr, "graph_set_axis_auto(%d,%d)\n", axis, set);
#endif
}

int
graph_get_axis_auto(int axis)
{
	if (axis == 0  || axis == 1)
		return Global->PlotGlobal->XYAuto[axis];

	io_error_msg("Invalid graph axis %d, must be 0 or 1", axis);
	return 1;
}

double
graph_get_axis_lo(int axis)
{
	if (axis < 0 || axis > graph_num_axis) {
		io_error_msg("graph_get_axis_lo : invalid graph axis %d, must be 0 or 1", axis);
		return 0.0;
	}
	return Global->PlotGlobal->XYMin[axis];
}

double
graph_get_axis_hi(int axis)
{
	if (axis < 0 || axis > graph_num_axis) {
		io_error_msg("graph_get_axis_hi: invalid graph axis %d, must be 0 or 1", axis);
		return 0.0;
	}
	return Global->PlotGlobal->XYMax[axis];
}

void
graph_set_linetooffscreen(int set)
{
	Global->PlotGlobal->LineToOffscreen = set;
#ifdef	GRAPH_VERBOSE
	fprintf(stderr, "graph_set_linetooffscreen(%d)\n", set);
#endif
}

int
graph_get_linetooffscreen(void)
{
	return Global->PlotGlobal->LineToOffscreen;
}

/*
 * Axis tick marks
 */
void
graph_set_axis_ticks(int axis, int ticktype, char *fmt)
{
#ifdef	GRAPH_VERBOSE
	fprintf(stderr, "graph_set_axis_ticks(%d, %d, %s)\n", axis, ticktype, fmt);
#endif

	if (axis < 0 || axis > graph_num_axis) {
		io_error_msg("graph_set_axis_ticks: invalid graph axis %d, must be 0 or 1", axis);
		return;
	}

	Global->PlotGlobal->ticktype[axis] = ticktype;
	Global->PlotGlobal->tickformat[axis] = fmt;
}

int
graph_get_axis_ticktype(int axis)
{
	if (axis < 0 || axis > graph_num_axis) {
		io_error_msg("graph_get_axis_ticktype: invalid graph axis %d, must be 0 or 1", axis);
		return 0;
	}

	return Global->PlotGlobal->ticktype[axis];
}

char *
graph_get_axis_tickformat(int axis)
{
	if (axis < 0 || axis > graph_num_axis) {
		io_error_msg("graph_get_axis_tickformat: invalid graph axis %d, must be 0 or 1", axis);
		return 0;
	}

	return Global->PlotGlobal->tickformat[axis];
}

void
graph_postscript (char * file, int kind, int spectrum, char * font, int pt_size)
{
  if (isupper (kind))
    kind = tolower (kind);
  if (isupper (spectrum))
    spectrum = tolower (spectrum);
  if (!index ("led", kind))
    io_error_msg
      ("Incorrect postscript graph type %c (should be one of l, e, or c)",
       kind);
  if (!index ("mc", spectrum))
    io_error_msg
      ("Incorrect postscript color type %c (should be either m or c)",
       spectrum);
#if 0
  sprint_line (&graph_term_cmd,
               "postscript %c %c %s %d  # Postscript",
               kind, spectrum, char_to_q_char (font), pt_size);
  set_line (&graph_output_file, file);
#endif
}

/*
 * Show a GNU Plotutils chart.
 *
 * In the Motif interface, ...
 * The client parameter is the function (from plot.c) which will
 * handle the plotting. We pass it to RedrawPlotutilsWindow() too.
 */
void
graph_plot(void)
{
#ifdef	HAVE_LIBPLOT
	FILE	*fp = NULL;
	int	havepipe = 0;

	if (Global->PlotGlobal->output_file) {
	    /* Treat stdout ("-") case */
	    if (strcmp(Global->PlotGlobal->output_file, "-") == 0)
		fp = stdout;
	    else if (Global->PlotGlobal->output_file[0] == '|') {
		/* FIX ME treat pipe */
		havepipe = 1;
	    } else {
		fp = fopen(Global->PlotGlobal->output_file, "w");
	    }
	}

	if (fp == NULL) {
		io_error_msg("Cannot open file '%s'", Global->PlotGlobal->output_file);
		return;
	}

	PuPlot(Global->PlotGlobal->graph_type, Global->PlotGlobal->device, fp);

	if (havepipe) {
		/* FIX ME */
	} else if (fp == stdout && using_curses) {
		;	/* Don't do a thing, curses would die. */
	} else {
		fclose(fp);
	}
#endif
}

void
plotutils_set_device(enum graph_device d)
{
	Global->PlotGlobal->device = d;
}

void
plotutils_metaplot(void)
{
	Global->PlotGlobal->device = GRAPH_METAFILE;
}

void
plotutils_illustrator(void)
{
	Global->PlotGlobal->device = GRAPH_ILLUSTRATOR;
}

void
plotutils_fig(void)
{
	Global->PlotGlobal->device = GRAPH_FIG;
}

void
plotutils_x_mono(void)
{
	Global->PlotGlobal->device = GRAPH_X_MONO;
}

void
plotutils_x_color(void)
{
	Global->PlotGlobal->device = GRAPH_X;
}

void
plotutils_png(void)
{
	Global->PlotGlobal->device = GRAPH_PNG;
}

void
plotutils_gif(void)
{
	Global->PlotGlobal->device = GRAPH_GIF;
}

void
plotutils_pcl(void)
{
	Global->PlotGlobal->device = GRAPH_PCL;
}

void
plotutils_hpgl(void)
{
	Global->PlotGlobal->device = GRAPH_HPGL;
}

void
plotutils_tek(void)
{
	Global->PlotGlobal->device = GRAPH_TEK;
}

void
plotutils_regis(void)
{
	Global->PlotGlobal->device = GRAPH_REGIS;
}

void
plotutils_make_info(void)
{
#ifdef	HAVE_LIBPLOT
  struct info_buffer	*ib = find_or_make_info ("graphing-parameters");
  enum graph_axis	axis;
  int			x;

  clear_info (ib);

  print_info(ib, "");
  switch (Global->PlotGlobal->graph_type) {
	case GRAPH_XY:
		print_info(ib, "Graph type is XY plot.");
		break;
	case GRAPH_BAR:
		print_info(ib, "Graph type is BAR chart.");
		break;
	case GRAPH_PIE:
		print_info(ib, "Graph type is PIE chart.");
		break;
  }
  print_info(ib, "");
  print_info(ib,
	"Parameter		Value");
  print_info(ib, "");
  switch (Global->PlotGlobal->device) {
  case GRAPH_TEK:
    print_info(ib, "output type		%s", "Tektronix");
    break;
  case GRAPH_X:
  case GRAPH_X_MONO:
    print_info(ib, "output type		%s", "X window");
    break;
  case GRAPH_PNG:
    print_info(ib, "output type		%s", "PNG");
    break;
  case GRAPH_GIF:
    print_info(ib, "output type		%s", "GIF");
    break;
  case GRAPH_METAFILE:
    print_info(ib, "output type		%s", "GNU metafile");
    break;
  case GRAPH_ILLUSTRATOR:
    print_info(ib, "output type		%s", "Adobe Illustrator");
    break;
  case GRAPH_FIG:
    print_info(ib, "output type		%s", "FIG");
    break;
  case GRAPH_PCL:
    print_info(ib, "output type		%s", "PCL");
    break;
  case GRAPH_HPGL:
    print_info(ib, "output type		%s", "HP GL");
    break;
  case GRAPH_POSTSCRIPT:
    print_info(ib, "output type		%s", "PostScript");
    break;
  case GRAPH_REGIS:
    print_info(ib, "output type         %s", "ReGIS");
    break;
  case GRAPH_NONE:
  default:
    print_info(ib, "output type		???");
    break;
  };

  if (Global->PlotGlobal->output_file)
    print_info(ib,
       "output file		%s",
	Global->PlotGlobal->output_file);
  
  for (axis = graph_x; axis <= graph_y; ++axis)
    print_info(ib,
       "%s-axis title		%s",
       graph_axis_name[axis], Global->PlotGlobal->graph_axis_title[axis].buf);

  print_info(ib,
       "logarithmic axes	%s",
       (Global->PlotGlobal->graph_logness[graph_x]
	? (Global->PlotGlobal->graph_logness[graph_y] ? "x,y" : "x")
	: (Global->PlotGlobal->graph_logness[graph_y] ? "y" : "-neither-")));

  for (axis = graph_x; axis <= graph_y; ++axis) {
	print_info(ib,
		"%s-axis range		[%s..%s]",
	   graph_axis_name [axis],
	   Global->PlotGlobal->graph_rng_lo[axis].buf, Global->PlotGlobal->graph_rng_hi[axis].buf,
	   Global->PlotGlobal->graph_rng_hi[axis].buf, Global->PlotGlobal->graph_rng_hi[axis].buf);
    }

  for (axis = graph_x; axis <= graph_y; ++axis) {
      if (Global->PlotGlobal->graph_axis_labels[axis].lr != NON_ROW)
	print_info(ib,
	   	"%s-axis labels		%s",
	   graph_axis_name[axis],
	   range_name(&Global->PlotGlobal->graph_axis_labels[axis]));
    }

    for (x = 0; x < NUM_DATASETS; ++x)
      if (Global->PlotGlobal->graph_data [x].lr != NON_ROW)
      {
	print_info (ib, "");
	print_info (ib,"Data Set %d%s%s",
		    x,
		    Global->PlotGlobal->graph_title[x].buf[0] ? " entitled " : "",
		    Global->PlotGlobal->graph_title[x].buf);
	print_info (ib,"  data for this set: %s",
		    range_name (&Global->PlotGlobal->graph_data[x]));
	print_info (ib,"  style for this set: %s",
		    (Global->PlotGlobal->style[x] == GRAPH_STYLE_DEFAULT) ? "default" :
		    (Global->PlotGlobal->style[x] == GRAPH_STYLE_LINES) ? "lines" :
		    (Global->PlotGlobal->style[x] == GRAPH_STYLE_MARKS) ? "marks" :
		    (Global->PlotGlobal->style[x] == GRAPH_STYLE_BOTH) ? "both" :
		    (Global->PlotGlobal->style[x] == GRAPH_STYLE_NONE) ? "none" : "???"
		);
	print_info (ib,"");
      }
#endif
}

void
plotutils_set_axis_labels(int axis_c, struct rng * rng)
{
  enum graph_axis axis = chr_to_axis (axis_c);
  Global->PlotGlobal->graph_axis_labels [axis] = *rng;
}

void
plotutils_set_filename(char *file)
{
  if (Global->PlotGlobal->output_file)
	free(Global->PlotGlobal->output_file);
  Global->PlotGlobal->output_file = strdup(file);
}

void plotutils_xy(void)
{
	Global->PlotGlobal->graph_type = GRAPH_XY;
	io_info_msg("Graph type is now XY chart");
}

void plotutils_pie(void)
{
	Global->PlotGlobal->graph_type = GRAPH_PIE;
	io_info_msg("Graph type is now PIE chart");
}

void plotutils_bar(void)
{
	Global->PlotGlobal->graph_type = GRAPH_BAR;
	io_info_msg("Graph type is now BAR chart");
}

void plotutils_set_graph_type(enum graph_type gt)
{
	Global->PlotGlobal->graph_type = gt;
}
