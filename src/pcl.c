/*
 *  $Id: pcl.c,v 1.17 2001/02/13 23:38:06 danny Exp $
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

static char rcsid[] = "$Id: pcl.c,v 1.17 2001/02/13 23:38:06 danny Exp $";

#include <stdio.h>

#ifndef	TEST
#ifdef	HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include "afm.h"
#endif

#include "global.h"
#include "cell.h"

#ifdef	TEST
struct OleoGlobal	*Global;

#define JST_DEF		0
#define JST_LFT		1
#define JST_RGT		2
#define JST_CNT		3
#endif

#include "print.h"

static int	y;	/* absolute position on page */
static float	border_width = 20.0,
		border_height = 20.0;

struct {
	char	*e;
	char	*code;
} encodings[] = {
	{ "ISOLatin1",	"\033(0N" },
	{ "ISOLatin2",	"\033(2N" },
	{ "ISO5",	"\033(5N" },
	{ "ISOLatin9",	"\033(5N" },
	{ "Swedish",	"\033(0S" },
	{ NULL,		NULL}
};

static char	*pcl_encoding = NULL;

void PCLJobHeader(char *str, int npages, FILE *fp)
{
	int	i, found = 0;

	/* Printer reset */
	fputc('\033', fp);
	fputc('E', fp);
#if 0
	/* Landscape */
	fputc('\033', fp);
	fputc('&', fp);
	fputc('l', fp);
	fputc('1', fp);
	fputc('O', fp);
#endif

	for (i=0; pcl_encoding && encodings[i].e; i++)
		if (stricmp(pcl_encoding, encodings[i].e) == 0) {
			found++;
			fprintf(fp, "%s", encodings[i].code);
			break;
		}

	if (! found) {
		/* Iso Latin 1 */
		fputc('\033', fp);
		fputc('(', fp);
		fputc('0', fp);
		fputc('N', fp);
	}
}

void PCLJobTrailer(int npages, FILE *fp)
{
}

void PCLPageHeader(char *str, FILE *fp)
{
	if (Global->need_formfeed) {
		/* Form feed */
		fputc('\f', fp);

		Global->need_formfeed = 0;
	}

	y = border_height;
}

void PCLPageFooter(char *str, FILE *fp)
{
	/*
	 * Only set a flag here, to avoid FormFeed at the
	 * end of the document. This would just print a blank page.
	 */
	Global->need_formfeed = 1;
}

void PCLField(char *str, int wid, int rightborder,
			int xpoints, int xchars, FILE *fp)
{
#if 0
	fprintf(stderr, "PCLField(%s,%d,%d)\n",
		str, wid, rightborder);
#endif

#if 0
	/* Absolute cursor positioning in dots - not what we want */
	fprintf(fp, "\033*p%dx%dY", xpoints, y);
#else
	/* Absolute cursor positioning in decipoints */
	fprintf(fp, "\033&a%dh%dV", 10 * xpoints, 10 * y);
#endif

#if 0
	sprintf(format, "%%-%ds", wid);
#endif
	fprintf(fp, "%s", str);
}

void PCLBorders(FILE *fp)
{
}

struct typefaces { char *typeface, *code; } TypeFaces[] = {
	{ "cg times",		"\033(s4101T" },	/* The default one */
	{ "univers",		"\033(s4148T" },
	{ "lineprinter",	"\033(s0T" },
	{ "courier",		"\033(s4099T" },
	{ "albertus",		"\033(s4362T" },
	{ "antique olive",	"\033(s4168T" },
	{ "clarendon",		"\033(s4140T" },
	{ "coronet",		"\033(s4116T" },
	{ "garamond antiqua",	"\033(s4197T" },
	{ "letter gothic",	"\033(s4102T" },
	{ "marigold",		"\033(s4297T" },
	{ "cg omega",		"\033(s4113T" },
	{ "arial",		"\033(s16602T" },
	{ "times new roman",	"\033(s16901T" },
	{ "symbol",		"\033(s16686T" },
	{ "wingdings",		"\033(s31402T" },
	{ NULL, NULL }
};

void PCLFont(char *family, char *slant, int size, FILE *fp)
{
	int	i, found;

	/* Typeface */
	found = 0;
	for (i=0; TypeFaces[i].typeface; i++) {
		if (strstr(family, TypeFaces[i].typeface)) {
			fprintf(fp, "%s", TypeFaces[i].code);
			found = 1;
			break;
		}
	}

	if (found == 0)
		fprintf(fp, "%s", TypeFaces[0].code);

#if 0
	/* Don't send pitch */
	/* Pitch (number of characters per inch) */
#ifndef	TEST
	fprintf(fp, "\033(s%dH", AfmPitch());
#else
{
	/* TEST ! */
	float	p = 72.0 / 0.628 / size;
	int	i = p;

	fprintf(fp, "\033(s%dH", i);
}
#endif
#endif

	/* Primary size */
	fprintf(fp, "\033(s%dV", size);

	/* Slant */
	if (slant == NULL || stricmp(slant, "normal") == 0
			|| stricmp(slant, "medium") == 0
			|| strlen(slant) ==0) {
		fprintf(fp, "\033(s0S\033(s0B");
	} else if (strstr(slant, "bold") != NULL) {	/* Bold */
		if (strstr(slant, "italic") != NULL) {	/* Bold-Italic */
			fprintf(fp, "\033(s1S");
		} else {
			fprintf(fp, "\033(s0S");
		}
			fprintf(fp, "\033(s3B");
	} else {	/* Not bold */
		if (strstr(slant, "italic") != NULL) {	/* Italic */
			fprintf(fp, "\033(s1S");
		} else {
			fprintf(fp, "\033(s0S");
		}
			fprintf(fp, "\033(s0B");
	}
}

void PCLNewLine(int ht, FILE *fp)
{
	fprintf(fp, "\n\r");

	y += ht;
}

void PCLPaperSize(int wid, int ht, FILE *fp)
{
	/* A4 */
	fprintf(fp, "\033&l26A");
}

void PCLSetEncoding(char *encoding)
{
	if (pcl_encoding)
		free(pcl_encoding);

	pcl_encoding = strdup(encoding);
}

void PCLSetBorder(int width, int height, FILE *fp)
{
	border_width = width;
	border_height = height;
}

struct PrintDriver PCLPrintDriver = {
	"PCL",
	&PCLJobHeader,
	&PCLJobTrailer,
	&PCLPageHeader,
	&PCLPageFooter,
	&PCLField,
	&PCLBorders,
	&PCLFont,
	&PCLNewLine,
	&PCLPaperSize,
	&PCLSetEncoding,
	&PCLSetBorder
};

#ifdef	TEST
int 
stricmp (const char * s1, const char * s2)
{
  register const char *scan1;
  register const char *scan2;
  register char chr1, chr2;

  scan1 = s1;
  scan2 = s2;
  do
    {
      chr1 = isupper (*scan1) ? tolower (*scan1) : *scan1;
      chr2 = isupper (*scan2) ? tolower (*scan2) : *scan2;
      scan1++;
      scan2++; 
    }
  while (chr1 && chr1 == chr2);

  /*
         * The following case analysis is necessary so that characters
         * which look negative collate low against normal characters but
         * high against the end-of-string NUL.
         */
  if (chr1 == '\0' && chr2 == '\0')
    return 0;
  else if (chr1 == '\0')
    return -1;
  else if (chr2 == '\0')
    return 1;
  else
    return chr1 - chr2;
}

int main(int argc, char *argv[])
{
	struct PrintDriver	*pd = &PCLPrintDriver;
	FILE			*fp = fopen("test.out", "w");

	Global = (struct OleoGlobal *)malloc(sizeof(struct OleoGlobal));

	fprintf(stderr, "Testing print driver for '%s'\n", pd->name);
	pd->job_header("This is a title", 1, fp);
	pd->font("times", "italic", 8, fp);
	pd->page_header("Page 1", fp);
	pd->field("Field 1", 10, 1, 10, 1, fp);
	pd->font("times", "bold", 8, fp);
	pd->field("Field 2", 10, 1, 110, 1, fp);
	pd->font("times", "bold-italic", 8, fp);
	pd->field("Field 3", 10, 1, 210, 1, fp);

	pd->newline(10, fp);
	pd->font("cg times", NULL, 8, fp);
	pd->field("Field 4 - this is in CG Times", 40, 1, 10, 1, fp);

	pd->newline(10, fp);
	pd->font("marigold", NULL, 8, fp);
	pd->field("Field 5 - this is in Marigold", 40, 1, 10, 1, fp);

	pd->newline(10, fp);
	pd->font("clarendon", NULL, 8, fp);
	pd->field("Field 6 - this is in Clarendon", 40, 1, 10, 1, fp);

	pd->newline(10, fp);
	pd->font("letter gothic", NULL, 8, fp);
	pd->field("Field 7 - this is in Letter Gothic", 60, 1, 10, 1, fp);

	pd->newline(10, fp);
	pd->font("letter gothic", NULL, 8, fp);
	pd->field("Field 8 - centered in Letter Gothic", 60, 1, 10, 1, fp);

	pd->newline(10, fp);
	pd->font("letter gothic", NULL, 8, fp);
	pd->field("Field 9 - right in Letter Gothic", 60, 1, 10, 1, fp);

	pd->newline(10, fp);
	pd->font("letter gothic", NULL, 8, fp);
	pd->field("Field 10 - left in Letter Gothic", 60, 1, 10, 1, fp);

	pd->newline(10, fp);
	pd->font("cg times", NULL, 8, fp);
	pd->field("CG Times, 8 points", 60, 1, 10, 1, fp);
	pd->newline(10, fp);
	pd->font("cg times", NULL, 10, fp);
	pd->field("CG Times, 10 points", 60, 1, 10, 1, fp);
	pd->newline(10, fp);
	pd->font("cg times", NULL, 12, fp);
	pd->field("CG Times, 12 points", 60, 1, 10, 1, fp);
	pd->newline(10, fp);
	pd->font("cg times", NULL, 14, fp);
	pd->field("CG Times, 14 points", 60, 1, 10, 1, fp);
	pd->newline(10, fp);
	pd->font("cg times", NULL, 18, fp);
	pd->field("CG Times, 18 points", 60, 1, 10, 1, fp);
	pd->newline(10, fp);
	pd->font("cg times", NULL, 24, fp);
	pd->field("CG Times, 24 points", 60, 1, 10, 1, fp);
	pd->newline(10, fp);
	pd->font("cg times", "bold", 12, fp);
	pd->field("CG Times, bold, 12 points", 60, 1, 10, 1, fp);

	pd->page_footer("End page 1", fp);
	pd->job_trailer(1, fp);

	fclose(fp);

	exit(0);
}
#endif	/* TEST */
