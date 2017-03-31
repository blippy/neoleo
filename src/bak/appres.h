/*
 * $Id: appres.h,v 1.9 2000/08/10 21:02:49 danny Exp $
 *
 * Copyright © 1998 Free Software Foundation, Inc.
 * Written by Danny Backx <danny@gnu.org>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

typedef struct GnuSheetAppres {
	int		show_version;
	Boolean		quiet;
	Boolean		ignore_init_file;
	String		init_file;
	Boolean		show_usage;
	int		rows, columns;
	XmString	quit_message;
	String		save_as_title, load_title;
	int		columnWidth;
	String		paper, printer, program, defaultPrintTo;
} GnuSheetAppres;

extern XtResource	resources[];
extern int		num_resources;
extern GnuSheetAppres	AppRes;
