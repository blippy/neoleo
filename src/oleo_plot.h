/*
 * $Id: oleo_plot.h,v 1.10 2000/08/10 21:02:51 danny Exp $
 * 
 * Copyright © 1998-1999 Free Software Foundation, Inc.
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

#ifndef _OLEO_PLOT_H_
#define _OLEO_PLOT_H_

#include "line.h"

/* Define the devices that we can generate plots for. */
enum graph_device {
	GRAPH_NONE,
	GRAPH_POSTSCRIPT,
	GRAPH_TEK,
	GRAPH_X,
	GRAPH_X_MONO,	/* ??? */
	GRAPH_PNG,
	GRAPH_GIF,
	GRAPH_METAFILE,
	GRAPH_ILLUSTRATOR,
	GRAPH_FIG,
	GRAPH_PCL,
	GRAPH_HPGL,
	GRAPH_REGIS,
	GRAPH_PNM,
	GRAPH_SVG,
	GRAPH_CGM,
};

enum graph_type {
	GRAPH_XY,
	GRAPH_PIE,
	GRAPH_BAR,
};

enum graph_style {
	GRAPH_STYLE_DEFAULT,
	GRAPH_STYLE_LINES,
	GRAPH_STYLE_MARKS,
	GRAPH_STYLE_BOTH,
	GRAPH_STYLE_NONE	/* to hide ?? */
};

struct PlotutilsDevices {
	enum graph_type	t;
	char		*pus;	/* plotutils string */
	char		*ext;	/* filename suffix */
	char		*desc;	/* description */
};
extern struct PlotutilsDevices PlotutilsDeviceArray[];

/*
 * The public functions in plot.c should all have the same signature,
 * as indicated in the typedef PuFunction.
 */
typedef void (*PuFunction)(char *, FILE *);

/*
 * The first argument is the PlotUtils plotter (can be one of several character
 * strings such as "ps"; see the Plotutils documentation).
 *
 * The second argument, if relevant, is a standard file pointer which has
 * already been opened for writing.
 */
void PuPieChart(char *plotter, FILE *outfile);
void PuBarChart(char *plotter, FILE *outfile);
void PuXYChart(char *plotter, FILE *outfile);

void PlotInit(void);
void PuPlot(enum graph_type gt, enum graph_device gd, FILE *fp);

struct PlotGlobalType {
	double			XYMin[graph_num_axis], XYMax[graph_num_axis];
	int			XYAuto[graph_num_axis];
	int			LineToOffscreen;
	int			img_width, img_height;
	struct line		graph_axis_title[graph_num_axis];
	struct line		graph_rng_lo [graph_num_axis];
	struct line		graph_rng_hi [graph_num_axis];
	int			graph_logness [graph_num_axis]; /* set logarithmic */
	/* The ranges (if any) of the symbolic names
	 * for integer coordinates starting at 0.
	 * If none, these will have lr == NON_ROW.
	 */
	struct rng		graph_axis_symbols [graph_num_axis];
	enum graph_ordering	graph_axis_ordering [graph_num_axis];

	/* Names to print along the axes */
	struct rng		graph_axis_labels [graph_num_axis];

	/* plot .... with %s */
	struct line		graph_style[NUM_DATASETS];
	struct line		graph_title[NUM_DATASETS];
	struct rng		graph_data[NUM_DATASETS];

/* SciPlot */
	int			ticktype[graph_num_axis];
	char			*tickformat[graph_num_axis];

/* Plotutils */
	enum graph_device	device;
	enum graph_type		graph_type;
	char			*output_file;
	char			*dashdot[NUM_DATASETS];
	enum graph_style	style[NUM_DATASETS];
};

#define	XYxMin			Global->PlotGlobal->XYMin[0]
#define	XYxMax			Global->PlotGlobal->XYMax[0]
#define	XYyMin			Global->PlotGlobal->XYMin[1]
#define	XYyMax			Global->PlotGlobal->XYMax[1]
#define	XYxAuto			Global->PlotGlobal->XYAuto[0]
#define	XYyAuto			Global->PlotGlobal->XYAuto[1]

/* Defaults */
#define	PLOT_WINDOW_WIDTH	600
#define	PLOT_WINDOW_HEIGHT	600

#endif  /* _OLEO_PLOT_H_ */
