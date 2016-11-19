/*
 *  $Id: postscript.c,v 1.20 2001/02/13 23:38:06 danny Exp $
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

/*
 * This file contains the PostScript specific printing support.
 * It is called by src/print.c which contains general printing support.
 *
 * There shouldn't be much spreadsheet functionality here...
 */

static char rcsid[] = "$Id: postscript.c,v 1.20 2001/02/13 23:38:06 danny Exp $";

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <locale.h>

#include "display.h"
#include "font.h"
#include "global.h"
#include "cmd.h"
#include "io-generic.h"
#include "io-abstract.h"
#include "print.h"

/*
 * FIX ME these should be static
 */
extern float default_pswid, default_pshgt;

static int	CurrentFontSize = 0;
static char	*ps_encoding = NULL;

void 
put_ps_string (char *str, FILE *fp)
{
  fputc ('(', fp);
  while (*str)
    {
      if (*str == ')')
	fputs ("\\)", fp);
      else if (*str == '(')
	fputs ("\\(", fp);
      else
	fputc (*str, fp);
      ++str;
    }
  fputc (')', fp);
}

static char	**KnownFonts = NULL;
static int	nfonts = 0;

void FontCacheRestart(void)
{
	free(KnownFonts);
	KnownFonts = 0;
	nfonts = 0;
}

void ReencodeFont(char *fn, FILE *fp)
{
	int		i;

	for (i=0; i<nfonts; i++)
		if (strcmp(fn, KnownFonts[i]) == 0)
			return;

	nfonts++;
	KnownFonts = realloc(KnownFonts, sizeof(char *) * nfonts);

	KnownFonts[nfonts-1] = strdup(fn);

	fprintf(fp, "/%sISO /%s reencodeISO\n", fn, fn);
}

void PostScriptEncoding(char *encoding)
{
	if (ps_encoding)
		free(ps_encoding);
	ps_encoding = strdup(encoding);
}

void PostScriptJobHeader(char *title, int npages, FILE *fp)
{
	struct font_names	*fn;
	struct font_memo	*fm;

	if (! Global->oldLocale)
		Global->oldLocale = setlocale(LC_ALL, NULL);

	FontCacheRestart();

	fprintf(fp, "%%!PS-Adobe-3.0\n");
	fprintf(fp, "%%%%Creator: %s %s\n", PACKAGE, VERSION);
	fprintf(fp, "%%%%Pages: (atend)\n");
	fprintf(fp, "%%%%PageOrder: Ascend\n");

	/*
	 * Print a list of all fonts
	 */
	fprintf(fp, "%%%%DocumentFonts:");
	for (fm = font_list; fm; fm = fm->next) {
		if ((fn = fm->names) == 0)
			continue;
		if (fn->ps_name == 0 || strlen(fn->ps_name) == 0)
			continue;
		fprintf(fp, " %sISO", fn->ps_name);
	}
	fprintf(fp, "\n");

	/* done */

	fprintf(fp, "%%%%Title: %s\n", title);
	fprintf(fp, "%%%%EndComments\n%%%%BeginProlog\n");

	/*
	 * Define a function to reencode all fonts to ISO Latin 1
	 * (or another known encoding).
	 */
	fprintf(fp, "/reencodeISO { %%def\n");
	fprintf(fp, "    findfont dup length dict begin\n");
	fprintf(fp, "    { 1 index /FID ne { def }{ pop pop } ifelse } forall\n");
	fprintf(fp, "    /Encoding %sEncoding def\n",
		ps_encoding ? ps_encoding : "ISOLatin1");
	fprintf(fp, "    currentdict end definefont pop\n");
	fprintf(fp, "} bind def\n");

	/*
	 * Remap all used fonts to ISO, give them a new name.
	 */
	for (fm = font_list; fm; fm = fm->next) {
		if ((fn = fm->names) == 0)
			continue;
		if (fn->ps_name == 0 || strlen(fn->ps_name) == 0)
			continue;
		ReencodeFont(fn->ps_name, fp);
	}

	/* Don't forget Courier */
	ReencodeFont("Courier", fp);

	/*
	 *
	 */
	fprintf(fp, "/FontName where { pop } { /FontName (Courier) def } ifelse\n");
	fprintf(fp, "/FirstSize where { pop } { /FirstSize 10 def } ifelse\n");
	fprintf(fp, "%%%%EndProlog\n");

}

void PostScriptJobTrailer(int npages, FILE *fp)
{
	fprintf(fp, "%%Pages: %d\n", npages);
	fprintf(fp, "%%EOF\n");

	setlocale(LC_ALL, Global->oldLocale);
}

/*
 * These variables hold the position we're printing at
 */
static float	x, y;
static float	paper_width, paper_height;
static float	border_width = 20.0,
		border_height = 20.0;

/*
 * MULTIPLY_WIDTH should provide the mapping between string length (in chars)
 *	and visible length in points.
 */
/*
 * 8 : too wide
 * 6 : no margin between columns
 * 7 : ??
 */
#define	MULTIPLY_WIDTH	12

void PostScriptPageHeader(char *str, FILE *fp)
{
	fprintf(fp, "%%%%Page: %s\n", str);

	x = border_width;
	y = paper_height - border_height;
}

void PostScriptPageFooter(char *str, FILE *fp)
{
	fprintf(fp, "showpage\n");
}

void PostScriptField(char *str, int wid, int rightborder,
			int xpoints, int xchars, FILE *fp)
{
#if 0
	fprintf(stderr, "PostScriptField(%s, wid %d, xpoints %d)\n",
			str, wid, xpoints);
#endif
	if (strlen(str)) {
		fprintf(fp, "%3.1f %3.1f moveto ", border_width + xpoints, y);

		put_ps_string(str, fp);
		fprintf(fp, " show\n");
	}

	x += wid;

	if (rightborder) {
	}
}

void PostScriptBorders(FILE *fp)
{
}

static int changed(char *a, char *b)
{
	if (a == 0 && b == 0)
		return 0;
	if (a == 0 || b == 0)
		return 1;
	return strcmp(a, b);
}

void PostScriptFont(char *family, char *slant, int size, FILE *fp)
{
	static char	*of = NULL, *osl = NULL;
	static int	osz = 0;

	if ((! changed(family, of)) && (! changed(slant, osl)) && size == osz)
		return;

	fprintf(fp, "/%sISO findfont %d scalefont setfont\n", family, size);

	CurrentFontSize = size;

	if (of) free(of);
	if (osl) free(osl);

	osl = of = NULL;

	if (family) of = strdup(family);
	if (slant) osl = strdup(slant);
	osz = size;
}

void PostScriptNewLine(int ht, FILE *fp)
{
	x = border_width;
	y -= ht;
}

void PostScriptPaperSize(int wid, int ht, FILE *fp)
{
	paper_width = wid;
	paper_height = ht;
}

void PostScriptSetBorder(int width, int height, FILE *fp)
{
	border_width = width;
	border_height = height;
}

struct PrintDriver PostScriptPrintDriver = {
	"PostScript",
	&PostScriptJobHeader,
	&PostScriptJobTrailer,
	&PostScriptPageHeader,
	&PostScriptPageFooter,
	&PostScriptField,
	&PostScriptBorders,
	&PostScriptFont,
	&PostScriptNewLine,
	&PostScriptPaperSize,
	&PostScriptEncoding,
	&PostScriptSetBorder
};
