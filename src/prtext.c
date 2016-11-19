/*
 *  $Id: prtext.c,v 1.10 2000/08/10 21:02:51 danny Exp $
 *
 *  This file is part of Oleo, the GNU spreadsheet.
 *
 *  Copyright © 1999 by the Free Software Foundation, Inc.
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

static char rcsid[] = "$Id: prtext.c,v 1.10 2000/08/10 21:02:51 danny Exp $";

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include <stdio.h>

#include "global.h"

#include "print.h"

void TextJobHeader(char *str, int npages, FILE *fp)
{
}

void TextJobTrailer(int npages, FILE *fp)
{
}

void TextPageHeader(char *str, FILE *fp)
{
}

void TextPageFooter(char *str, FILE *fp)
{
}

void TextField(char *str, int wid, int rightborder,
			int xpoints, int xchars, FILE *fp)
{
}

void TextBorders(FILE *fp)
{
}

void TextFont(char *family, char *slant, int size, FILE *fp)
{
}

void TextNewLine(int ht, FILE *fp)
{
	fprintf(fp, "\n");
}

void TextPrinterPaperSize(int wid, int ht, FILE *fp)
{
}

void TextSetEncoding(char *encoding)
{
}

struct PrintDriver TextPrintDriver = {
	"Text",
	&TextJobHeader,
	&TextJobTrailer,
	&TextPageHeader,
	&TextPageFooter,
	&TextField,
	&TextBorders,
	&TextFont,
	&TextNewLine,
	&TextPrinterPaperSize,
	&TextSetEncoding
};
