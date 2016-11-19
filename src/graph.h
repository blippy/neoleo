#ifndef GRAPHH
#define GRAPHH

/*
 * $Id: graph.h,v 1.10 2000/08/10 21:02:50 danny Exp $
 *
 * Copyright © 1993-2000 Free Software Foundation, Inc.
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
#include "cell.h"

extern char * graph_axis_name [graph_num_axis];
extern char * graph_order_name [graph_num_orders];


extern int graph_ornt_row_magic [graph_num_pair_orientations];
extern int graph_ornt_col_magic [graph_num_pair_orientations];

extern char *pair_order_name [graph_num_pair_orders];


typedef void (*fpi_thunk) (void * rock, CELL * cp, CELLREF, CELLREF);


extern enum graph_axis chr_to_axis (int c);
extern char * graph_quoted_str (char *str);
extern void graph_postscript (char * file, int kind, int spectrum, char * font, int pt_size);
extern void graph_set_axis_title (int axis_c, char * title);
extern void graph_set_logness (int axis_c, int explicit, int newval);
extern void graph_check_range (char * val);
extern void graph_set_axis_lo (int axis_c, char * val);
extern void graph_set_axis_hi (int axis_c, char * val);
extern void graph_default_axis_labels (int axis_c);
extern int graph_check_style (char * name);
extern void graph_set_style (int data_set, char * style);
extern void graph_set_data_title (int data_set, char * title);
extern void graph_set_data(int data_set, struct rng * rng);
extern void graph_presets (void);
extern void graph_clear_datasets (void);
extern void init_graphing (void);
extern void graph_make_info (void);
extern void for_pairs_in (struct rng * rng, enum graph_pair_ordering order, fpi_thunk thunk, void * frame);
extern void graph_plot (void);

extern void plotutils_set_device(enum graph_device d);
extern void plotutils_set_filename(char *file);
extern void plotutils_tek(void);
extern void plotutils_regis(void);
extern void plotutils_fig(void);
extern void plotutils_x_color(void);
extern void plotutils_x_mono(void);
extern void plotutils_gif(void);
extern void plotutils_png(void);
extern void plotutils_illustrator(void);
extern void plotutils_metaplot(void);
extern void plotutils_pcl(void);
extern void plotutils_hpgl(void);
extern void plotutils_set_data(int data_set, struct rng * rng);
extern void plotutils_make_info(void);
extern void plotutils_set_axis_labels(int axis_c, struct rng * rng);
extern void plotutils_set_graph_type(enum graph_type gt);

extern void plotutils_xy(void);
extern void plotutils_pie(void);
extern void plotutils_bar(void);

extern char *graph_get_axis_title(int axis_c);
extern void graph_set_title(char *);
extern char *graph_get_title(void);
extern int graph_get_logness(int axis_c);
extern char *graph_get_data_title (int data_set);
extern struct rng graph_get_data(int data_set);

void graph_set_axis_auto(int axis, int set);
int graph_get_axis_auto(int axis);

double graph_get_axis_lo(int axis);
double graph_get_axis_hi(int axis);
void graph_set_linetooffscreen(int set);
int graph_get_linetooffscreen(void);

void graph_set_axis_ticks(int axis, int ticktype, char *fmt);
int graph_get_axis_ticktype(int axis);
char *graph_get_axis_tickformat(int axis);

typedef void (*plotter) (void);

#endif  /* GRAPHH */
