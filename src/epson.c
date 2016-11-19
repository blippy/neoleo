/*
 *  $Id: epson.c,v 1.15 2001/02/13 23:38:05 danny Exp $
 *
 *  This file is part of Oleo, the GNU spreadsheet.
 *
 *  Copyright © 1999, 2000, 2001 by the Free Software Foundation, Inc.
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

static char rcsid[] = "$Id: epson.c,v 1.15 2001/02/13 23:38:05 danny Exp $";

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include <stdio.h>

#include "global.h"

#include "print.h"

static float	border_width = 20.0,
		border_height = 20.0;

#if 0  /* unused */
static char	*Escape = "\033";
static char	*formfeed = "\f";

/* Fonts */
static char	*courier = "\033k2";
static char	*proportional = "\033p1";
static char	*noproportional = "\033p0";
static char	*Defaultfont = "\033k\002\033X\0\025\0";
static char	*defaultfont = "\033k\002\033X\000\012\000";	/* play */
#endif
static char	*italic = "\0334";
static char	*noitalic = "\0335";

void EpsonJobHeader(char *str, int npages, FILE *fp)
{
	/* Reset Printer */
	fputc('\033', fp);
	fputc('@', fp);
	fputc('\033', fp);
	fputc('@', fp);

	/* Set unit to 50/3600 = 1/72 of an inch which is 1 point */
	fputc('\033', fp);
	fputc('(', fp);
	fputc('U', fp);
	fputc('\001', fp);
	fputc('\000', fp);
	fputc('\062', fp);

	/* Set printing area */

	/* Select font */

	/* Set print position */
}

void EpsonJobTrailer(int npages, FILE *fp)
{
	fputc('\033', fp);
	fputc('@', fp);
}

void EpsonPageHeader(char *str, FILE *fp)
{
}

void EpsonPageFooter(char *str, FILE *fp)
{
	fputc('\f', fp);
}

void EpsonField(char *str, int wid, int rightborder,
			int xpoints, int xchars, FILE *fp)
{
	char	format[16];
#if 1
	static int	play = 0;

	if (play) {
		play--;
		fprintf(fp, "%s", italic);
	} else {
		play++;
		fprintf(fp, "%s", noitalic);
	}
#endif
	sprintf(format, "%%%ds", wid);
	fprintf(fp, format, str);
}

void EpsonBorders(FILE *fp)
{
}

void EpsonFont(char *family, char *slant, int size, FILE *fp)
{
#if 0
	fputc('\033', fp);
	fputc('k', fp);
	fputc('\002', fp);
	fputc('\033', fp);
	fputc('X', fp);
	fputc('\000', fp);
	fputc('\012', fp);
	fputc('\000', fp);
#else
	fputc('\033', fp);
	fputc('k', fp);
	fputc('\003', fp);	/* fputc('\000', fp);	*/
	fputc('\033', fp);
	fputc('X', fp);
	fputc('\044', fp);
	fputc('\024', fp);
	fputc('\000', fp);
#endif
}

void EpsonNewLine(int ht, FILE *fp)
{
	fputc('\015', fp);
	fputc('\012', fp);
}

void EpsonPaperSize(int wid, int ht, FILE *fp)
{
	int	hi, lo;

	hi = ht / 256;
	lo = ht % 256;

	/* Page Length */
	fputc('\033', fp);
	fputc('(', fp);
	fputc('C', fp);
	fputc('\002', fp);
	fputc('\000', fp);
	fputc(lo, fp);
	fputc(hi, fp);

	/* Margins */
	fputc('\033', fp);
	fputc('(', fp);
	fputc('c', fp);
	fputc('\004', fp);
	fputc('\000', fp);

	/* Top margin - FIX ME currently half an inch */
	hi = (72 / 2) / 256;
	lo = (72 / 2) % 256;
	fputc(lo, fp);
	fputc(hi, fp);

	/* Bottom margin - FIX ME currently half an inch */
	hi = (ht - 72/2) / 256;
	lo = (ht - 72/2) % 256;
	fputc(lo, fp);
	fputc(hi, fp);
}

void EpsonSetEncoding(char *encoding)
{
}

void EpsonSetBorder(int width, int height, FILE *fp)
{
	border_width = width;
	border_height = height;
}

struct PrintDriver EpsonStylusColorPrintDriver = {
	"Epson Stylus Color",
	&EpsonJobHeader,
	&EpsonJobTrailer,
	&EpsonPageHeader,
	&EpsonPageFooter,
	&EpsonField,
	&EpsonBorders,
	&EpsonFont,
	&EpsonNewLine,
	&EpsonPaperSize,
	&EpsonSetEncoding,
	&EpsonSetBorder
};

#ifdef	TEST
#include "oleo_icon.xpm"

main(int argc, char *argv[])
{
	struct PrintDriver	*pd = &EpsonStylusColorPrintDriver;
	FILE			*fp = fopen("test.out", "w");

	int			i;

	fprintf(stderr, "Testing print driver for '%s'\n", pd->name);
	pd->job_header("This is a title", 1, fp);
	pd->font("times", "italic", 8, fp);
	pd->page_header("Page 1", fp);
	pd->field("Field 1", 10, 0, 1, fp);
	pd->font("times", "bold", 8, fp);
	pd->field("Field 2", 10, 0, 1, fp);
	pd->font("times", "bold-italic", 8, fp);
	pd->field("Field 3", 10, 0, 1, fp);

	pd->newline(8, fp);
	pd->font("cg times", NULL, 8, fp);
	pd->field("Field 4 - this is in CG Times", 40, 0, 1, fp);

	/* Start graphics */

	/* Send XPM */
	for (i=7; oleo_icon[i]; i++) {
		
	}

	/* End graphics */

	/* Get it over with */
	pd->page_footer("End page 1", fp);
	pd->job_trailer(1, fp);

	fclose(fp);

	exit(0);
}
#endif
