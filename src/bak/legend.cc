/*
 * $Header: /cvs/oleo/src/legend.c,v 1.6 2000/08/10 21:02:51 danny Exp $
 *
 * This file is part of libsciplot, one of the libraries in the GNU PlotUtils package.
 *
 * Copyright © 2000 by Danny Backx
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef	HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef	HAVE_LIBPLOT
#ifndef	HAVE_LIBSCIPLOT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sciplot.h>
#include <sciplotI.h>

#define	POS_X_MIN	0.800
#define	POS_X_MIN_LINE	0.810
#define	POS_X_MAX_LINE	0.850
#define	POS_X_MIN_TXT	0.870
#define	POS_X_INC	0.0125
#define	POS_X_MAX	0.875

#define	POS_Y_MIN	0.900
#define	POS_Y_INC	0.025
#define	POS_Y_MAX	0.975

void sp_legend_draw(Multigrapher *mg)
{
	double	y;
	int	i, nl;

        switch (mg->plot_type) {
        case SP_PLOT_XY:
        case SP_PLOT_PIE:
                break;
        case SP_PLOT_BAR:
        default:
                return;	/* No legend */
        }

	/* Figure out whether there's something to show. */
	for (nl=0, i=1; i<=mg->datasetnum; i++) {
		if (mg->legend[i] && strlen(mg->legend[i]) != 0)
			nl++;
	}
	if (nl == 0)
		return;
	
	pl_pencolorname_r(mg->plotter, "black");	/* FIX ME */

	/* Draw rectangle */
	pl_fbox_r(mg->plotter,
		PLOT_SIZE * POS_X_MIN, PLOT_SIZE * POS_Y_MIN,
		PLOT_SIZE, PLOT_SIZE);

#if 0
	fprintf(stderr, "sp_legend_draw(%d datasets, %d legend texts)\n",
		mg->datasetnum, mg->nlegend);
#endif

	for (y = POS_Y_MAX * PLOT_SIZE, i=1;
			(i <= mg->datasetnum) && (i <= mg->nlegend);
			i++, y -= POS_Y_INC * PLOT_SIZE) {
		if (mg->legend[i]) {
			int	ly = y + 0.0125 * PLOT_SIZE;

			pl_colorname_r(mg->plotter, colorstyle[(i-1) % NO_OF_LINEMODES]);
			pl_fline_r(mg->plotter,
					POS_X_MIN_LINE * PLOT_SIZE, ly,
					POS_X_MAX_LINE * PLOT_SIZE, ly);
			pl_fmove_r(mg->plotter, POS_X_MIN_TXT * PLOT_SIZE, y);
			pl_alabel_r(mg->plotter, 'l', 'x', mg->legend[i]);
			pl_fmarker_r(mg->plotter,
				(POS_X_MIN_LINE + POS_X_MAX_LINE) * PLOT_SIZE / 2, ly,
				4 + i, PLOT_SIZE * 0.02);

		} else {
#if 0
			fprintf(stderr, "Legend no %d - no text\n", i);
#endif
		}
	}
}

void sp_legend_label(Multigrapher *mg, int i, char *s)
{
	int	j;

/*	fprintf(stderr, "sp_legend_label(%d,%s)\n", i, s);	*/

	if (i < 0)
		return;

	if (mg->nlegend < i) {
		mg->legend = realloc(mg->legend, sizeof(char *) * (i + 1));
		for (j=mg->nlegend+1; j < i; j++) {
			mg->legend[j] = NULL;
		}
		mg->nlegend = i;
	}

	mg->legend[i] = strdup(s);
}

#endif	/* HAVE_LIBSCIPLOT */
#endif	/* HAVE_LIBPLOT */
