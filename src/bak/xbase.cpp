/*
 *  $Id: xbase.cpp,v 1.8 2000/11/22 19:33:01 danny Exp $
 *
 *  This file is part of Oleo, the GNU spreadsheet.
 *
 *  Copyright (C) 1998-1999 by the Free Software Foundation, Inc.
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

extern "C" {

static char rcsid[] = "$Id: xbase.cpp,v 1.8 2000/11/22 19:33:01 danny Exp $";

#include "config.h"

#include <stdio.h>
#ifdef	HAVE_LOCALE_H
#include <locale.h>
#endif

#include <libintl.h>

#include "cmd.h"
#include "basic.h"
#include "io-utils.h"
#include "io-term.h"

}

#if defined(HAVE_LIBXBASE) || defined(HAVE_LIBXDB)

/*
 * Avoid Xbase madness
 */
#ifdef	PACKAGE
#undef	PACKAGE
#endif
#ifdef	VERSION
#undef	VERSION
#endif

#ifdef	HAVE_LIBXBASE
#include <xbase/xbase.h>
#endif
#ifdef	HAVE_LIBXDB
#include <xdb/xbase.h>
#endif

void
CppReadXbaseFile(char *fn, int ismerge)
{
	xbXBase	x;
	xbDbf	db(&x);

	int	i, fc, rc, j;
	char	*r;

//	fn = cell_value_string(curow, cucol, True);
//	fn = "/home/danny/LAND.DBF";

	db.OpenDatabase(fn);
	fc = db.FieldCount();

	/* Put DBF file headers on current row */
	for (i=0; i<fc; i++) {
		char	*s = db.GetFieldName(i),
			*ss = (char *)malloc(strlen(s) + 3);

		ss[0] = '"'; ss[1] = '\0';
		strcat(ss, s);
		strcat(ss, "\"");

		r = new_value(curow, i + cucol, ss);
	}

	/* Put DBF file data on subsequent rows */
	rc = db.GetFirstRecord();
	j = 1;
	while (rc == XB_NO_ERROR) {
		for (i=0; i<fc; i++) {
			char	t = db.GetFieldType(i);
			long	l;
			float	f;
			char	buf[128];

			switch (t) {
			case 'C':	/* character */
				buf[0] = '"';
				rc = db.GetField(i, buf+1, 0);
				strcat(buf, "\"");

				r = new_value(curow + j, cucol + i, buf);
				break;
			case 'D':	/* date */
				buf[0] = '"';
				rc = db.GetField(i, buf+1, 0);
				strcat(buf, "\"");

				/* FIX ME probably CCYYMMDD format, need to convert */
				r = new_value(curow + j, cucol + i, buf);
				break;
			case 'L':	/* logical */
				break;
			case 'M':	/* memo */
				break;
			case 'N':	/* numeric */
				l = db.GetLongField(i);
				sprintf(buf, "%ld", l);
				r = new_value(curow + j, cucol + i, buf);
				break;
			case 'F':	/* float */
				f = db.GetFloatField(i);
				sprintf(buf, "%f", f);
				r = new_value(curow + j, cucol + i, buf);
				break;
			default:
				break;
			}
		}

		rc = db.GetNextRecord();
		j++;
	}

#ifdef	HAVE_LIBXBASE
	db.CloseDatabase();
#else
	db.CloseDatabase(false);
#endif

	Global->modified = 1;
	recalculate(1);
}

extern "C" {

/*
 * The C interface
 */
void ReadXbaseFile(char *name, int ismerge)
{
	CppReadXbaseFile(name, ismerge);
}

char *oleoXdbVersion(void)
{
#ifdef	XDB_VERSION
	return XDB_VERSION;
#else
#ifdef	XBASE_VERSION
	return XBASE_VERSION;
#else
	return "unknown";
#endif
#endif
}

}
#endif	/* HAVE_LIBXBASE */
