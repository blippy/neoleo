/*
 *  $Id: io-motif.h,v 1.17 2000/08/10 21:02:50 danny Exp $
 *
 *  This file is part of GNU Oleo, a free spreadsheet.
 *
 *  Copyright © 1998-2000 Free Software Foundation, Inc.
 *  Written by Danny Backx <danny@gnu.org>.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef	_IO_MOTIF_H_
#define	_IO_MOTIF_H_

#include "config.h"
#include "global.h"
#include <Xm/Xm.h>
#include <X11/Intrinsic.h>

extern void versionCB(Widget, XtPointer, XtPointer);
extern void QuitCB(Widget, XtPointer, XtPointer);
extern void motif_init(int *argc, char **argv);
extern void ResetColumnWidths(void);
extern void MotifUpdateWidth(int, int);
extern void motif_build_gui(void);
extern void motif_main_loop(void);
extern void MotifSetWindowName(const char *);
extern void MessageAppend(Boolean beep, char *fmt, ...); 
extern void MotifGlobalInitialize(void);

extern void MotifButton(int, int, char *, char *);

struct MotifGlobalType {
	XtAppContext app_c;
	Widget	toplevel_w, SplashShell_w, plot_w;
	Widget	mw_w, mat_w, mb_w, filemenu_w, editmenu_w, stylemenu_w,
		optionsmenu_w, helpmenu_w, graphmenu_w, dbmenu_w, testmenu_w;
	Widget  msgtext_w, statustf_w, formulatf_w;
	Widget	pfsd_w, fsd_w, hd_w, html_w, gs_w;
	Widget	FormatD_w, MySQLDialog_w, PrintDialog_w;
	Widget	DefaultFileDialog_w, DefaultFileShell_w, ConfigureGraphNotebook_w;
	Widget	XYxAutoToggle_w, XYxMinText_w, XYxMaxText_w, XYyAutoToggle_w,
		XYyMinText_w, XYyMaxText_w, lineToOffscreen_w;
	Widget	UserPref_w, ActiveRangeSelectionWidget_w, pufsd_w;
	Widget	configureGraph_w, copyDialog_w, optionsDialog_w, executeCommandDialog_w;
	Widget	configureGraphInside_w;
	struct rng	selection_range;
	int	PuPlotter;
	PuFunction ThisPuFunction;
	char	**rowlabels, **columnlabels;
	short	*columnwidths;
	int	*columnmaxlengths;
	Pixmap  oleo_icon_pm;
	char	*early_msg_text;
	char	input_buf[1024];
	int	input_buf_allocated, chars_buffered;
	char	fileformat[10], pattern[13];
	int	fmt, havea0;
	Boolean	**selectedcells, newcall, needrecalculate;
	Widget	XLogToggle, xticklblmenu, xticklbloption, xtickfmt;
	Widget	YLogToggle, yticklblmenu, yticklbloption, ytickfmt;
	int	xtick, ytick;
	Widget	ticktype_w[2][10];	/* Safe, don't include sciplot.h */
	Widget	ZoomScale;
};

#define	app			Global->MotifGlobal->app_c

#define	UserPref		Global->MotifGlobal->UserPref_w
#define	toplevel		Global->MotifGlobal->toplevel_w
#define	SplashShell		Global->MotifGlobal->SplashShell_w
#define	plot		Global->MotifGlobal->plot_w
#define	mw		Global->MotifGlobal->mw_w
#define	mat		Global->MotifGlobal->mat_w
#define	mb		Global->MotifGlobal->mb_w
#define	filemenu		Global->MotifGlobal->filemenu_w
#define	editmenu		Global->MotifGlobal->editmenu_w
#define	stylemenu		Global->MotifGlobal->stylemenu_w
#define	optionsmenu		Global->MotifGlobal->optionsmenu_w
#define	helpmenu		Global->MotifGlobal->helpmenu_w
#define	graphmenu		Global->MotifGlobal->graphmenu_w
#define	dbmenu		Global->MotifGlobal->dbmenu_w
#define	testmenu		Global->MotifGlobal->testmenu_w
#define	msgtext		Global->MotifGlobal->msgtext_w
#define	statustf		Global->MotifGlobal->statustf_w
#define	formulatf		Global->MotifGlobal->formulatf_w
#define	fsd		Global->MotifGlobal->fsd_w
#define	pfsd		Global->MotifGlobal->pfsd_w
#define	hd		Global->MotifGlobal->hd_w
#define	Html		Global->MotifGlobal->html_w
#define	gs		Global->MotifGlobal->gs_w
#define	FormatD		Global->MotifGlobal->FormatD_w
#define	MySQLDialog		Global->MotifGlobal->MySQLDialog_w
#define	PrintDialog		Global->MotifGlobal->PrintDialog_w
#define	DefaultFileDialog		Global->MotifGlobal->DefaultFileDialog_w
#define	DefaultFileShell		Global->MotifGlobal->DefaultFileShell_w
#define	ConfigureGraphNotebook		Global->MotifGlobal->ConfigureGraphNotebook_w
#define	XYxAutoToggle		Global->MotifGlobal->XYxAutoToggle_w
#define	XYxMinText		Global->MotifGlobal->XYxMinText_w
#define	XYxMaxText		Global->MotifGlobal->XYxMaxText_w
#define	XYyAutoToggle		Global->MotifGlobal->XYyAutoToggle_w
#define	XYyMinText		Global->MotifGlobal->XYyMinText_w
#define	XYyMaxText		Global->MotifGlobal->XYyMaxText_w
#define	lineToOffscreen		Global->MotifGlobal->lineToOffscreen_w
#define	ActiveRangeSelectionWidget		Global->MotifGlobal->ActiveRangeSelectionWidget_w
#define	selection_range		Global->MotifGlobal->selection_range
#define	pufsd			Global->MotifGlobal->pufsd_w
#define	PuPlotter		Global->MotifGlobal->PuPlotter
#define	ThisPuFunction		Global->MotifGlobal->ThisPuFunction
#define	rowlabels		Global->MotifGlobal->rowlabels
#define	columnlabels		Global->MotifGlobal->columnlabels
#define	columnwidths		Global->MotifGlobal->columnwidths
#define	columnmaxlengths		Global->MotifGlobal->columnmaxlengths

#define	configureGraph		Global->MotifGlobal->configureGraph_w
#define	copyDialog		Global->MotifGlobal->copyDialog_w
#define	optionsDialog		Global->MotifGlobal->optionsDialog_w
#define	ConfigureGraphInside	Global->MotifGlobal->configureGraphInside_w

#define	ExecuteCommandDialog		Global->MotifGlobal->executeCommandDialog_w

#endif
