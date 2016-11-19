/*
 * Copyright © 1999, 2001 Free Software Foundation, Inc.
 *
 * $Id: afm.c,v 1.10 2001/02/13 23:38:05 danny Exp $
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

/*
 * This is Oleo's AFM module.
 *
 * Its purpose is to locate and interpret AFM (Adobe Font Metric) files,
 * and to determine string lengths based on that information.
 *
 * This implementation is based on Adobe's document "5004.AFM_Spec.pdf".
 *
 * The return value of AfmStringWidth is of course a number that represents
 * the string's width in points.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/param.h>

#include "afm.h"
#include "cmd.h"

#define	AFM_PATH	"/share/afm"
#define	USRLOCAL_PATH	"/usr/local"
#define	USR_PATH	"/usr"
#define	GS_PATH		"/share/ghostscript/fonts"

#define	LINE_LEN	256

static char	*CurrentFontName = NULL,
		*CurrentFontSlant = NULL;
static int	CurrentFontSize = 10;
FILE		*fp;

struct charmetrics {
	unsigned int	code;
	char	*name;
	int	width;
};

struct afm {
	int			IsFixedPitch, ItalicAngle,
				FontBBox1, FontBBox2, FontBBox3, FontBBox4,
				UnderlinePosition, UnderlineThickness, CapHeight,
				XHeight, Ascender, Descender,
				StartCharMetrics;
	struct charmetrics *	charMetrics;
	int			charIndex[256];
};

static struct afm *afm = 0;

static int changed(char *a, char *b)
{
	if (a == 0 && b == 0)
		return 0;
	if (a != 0 && b != 0 && strcmp(a, b) == 0)
		return 0;
	return 1;
}

static char *Where, *line;

static int IsCommand(char *literal)
{
	int	len;

	len = strlen(literal);

	Where = line + len + 1;

	return (strncmp(literal, line, len) == 0);
}

static int Value(void)
{
	int	v;

	while (isspace(*Where))
		Where++;
	sscanf(Where, "%d", &v);
	return v;
}

static int ValueBoolean(void)
{
	while (isspace(*Where))
		Where++;
	if (strncmp(Where, "True", 4) == 0)
		return 1;
	if (strncmp(Where, "true", 4) == 0)
		return 1;
	if (strncmp(Where, "TRUE", 4) == 0)
		return 1;
	return 0;
}

static void skip(void)
{
	while (*Where != ';' && *Where != '\n' && *Where != '\0')
		Where++;
}

static int namelen(void)
{
	char	*p;
	int	i;

	for (i=0, p=Where; !isspace(*p); i++, p++) ;
	return i;
}

static void ReadCharMetrics(int n)
{
	int	i, l;

	afm->StartCharMetrics = n;

	if (afm->charMetrics)
		free(afm->charMetrics);
	afm->charMetrics = (struct charmetrics *)calloc(n, sizeof(struct charmetrics));

	for (i=0; i<n; i++) {
		if (fgets(line, LINE_LEN, fp) == 0)
			return;	/* FIX ME */
		Where = line;
		while (*Where != '\n') {
			if (isspace(*Where)) {
				Where++;
				continue;
			}
			if (*Where == 'C' && isspace(*(Where+1))) {
				afm->charMetrics[i].code = atoi(Where+2);
				skip();
			} else if (*Where == 'C' && *(Where+1) == 'X' && isspace(*(Where+2))) {
				sscanf(Where+3, "%x", &afm->charMetrics[i].code);
				skip();
			} else if (*Where == 'N' && isspace(*(Where+1))) {
				Where += 2;
				l = namelen();
				afm->charMetrics[i].name = malloc(l+1);
				afm->charMetrics[i].name[l] = '\0';
				strncpy(afm->charMetrics[i].name, Where, l);
				skip();
			} else if (*Where == 'W' && *(Where+1) == 'X' && isspace(*(Where+2))) {
				afm->charMetrics[i].width = atoi(Where+3);
				skip();
			} else {
				skip();
				/* FIX ME */
			}

			Where++;
		}
	}
}

static void
ReadAfmLine(void)
{
	if (afm == 0) {
		afm = (struct afm *)malloc(sizeof(struct afm));
		memset(afm, 0, sizeof(struct afm));
	}

	if (IsCommand("IsFixedPitch")) afm->IsFixedPitch = ValueBoolean();
	if (IsCommand("ItalicAngle")) afm->ItalicAngle = Value();
	if (IsCommand("FontBBox"))
		sscanf(Where, "%d %d %d %d", &afm->FontBBox1, &afm->FontBBox2,
			&afm->FontBBox3, &afm->FontBBox4);
	if (IsCommand("StartCharMetrics")) {
		int	n = atoi(Where);
		ReadCharMetrics(n);
	}
}

char *
GSName(char *name, char *slant)
{
	if (slant == NULL)
		slant = "";

	if (strcmp(name, "Courier") == 0 && strcmp(slant, "") == 0)
		return "n022003l";	/* NimbusMonL-Regu */
	if (strcmp(name, "CGTimes") == 0 && strcmp(slant, "") == 0)
		return "n021003l";	/* NimbusRomNo9L-Regu */
	return NULL;
}

FILE *
OpenAfmFile(char *name, char *slant)
{
	FILE	*fp;
	char	*fn, *alias;

	fn = malloc(MAXPATHLEN);

	sprintf(fn, "%s/%s%s.afm", BUILD_PREFIX AFM_PATH, name, slant ? slant : "");
	fp = fopen(fn, "r");
	if (fp)
		return fp;
#ifdef	VERBOSE
	else
		MessageAppend(0, "Failed to open %s\n", fn);
#endif

	sprintf(fn, "%s/%s%s.afm", USR_PATH GS_PATH, name, slant ? slant : "");
	fp = fopen(fn, "r");
	if (fp)
		return fp;
#ifdef	VERBOSE
	else
		MessageAppend(0, "Failed to open %s\n", fn);
#endif

	sprintf(fn, "%s/%s%s.afm", USRLOCAL_PATH GS_PATH, name, slant ? slant : "");
	fp = fopen(fn, "r");
	if (fp)
		return fp;
#ifdef	VERBOSE
	else
		MessageAppend(0, "Failed to open %s\n", fn);
#endif

	if ((alias = GSName(name, slant)) != NULL) {
		sprintf(fn, "%s/%s.afm", USR_PATH GS_PATH, alias);
		fp = fopen(fn, "r");
		if (fp)
			return fp;
#ifdef	VERBOSE
	else
		MessageAppend(0, "Failed to open %s\n", fn);
#endif
		sprintf(fn, "%s/%s.afm", USRLOCAL_PATH GS_PATH, alias);
		fp = fopen(fn, "r");
		if (fp)
			return fp;
#ifdef	VERBOSE
	else
		MessageAppend(0, "Failed to open %s\n", fn);
#endif
	}

	if (fp == 0) {
		io_info_msg(PACKAGE ": couldn't open AFM file for %s%s\n", name, slant);
		free(fn);
		return NULL;
	}
	free(fn);

	return fp;
}

void
AfmSetFont(char *name, char *slant, int size)
{
/* Have the font name/slant changed ? */
	CurrentFontSize = size;

	if ( ( ! changed(name, CurrentFontName)) &&
		( ! changed(slant, CurrentFontSlant)))
		return;

/*
 * Something's changed, need to read another file.
 *
 * First however remember current state.
 */
	if (CurrentFontName) {
		free(CurrentFontName);
		CurrentFontName = 0;
	}
	if (CurrentFontSlant) {
		free(CurrentFontSlant);
		CurrentFontSlant = 0;
	}

	if (name)
		CurrentFontName = strdup(name);
	if (slant)
		CurrentFontSlant = strdup(slant);

/*
 * Start reading file.
 */
	fp = OpenAfmFile(name, slant);
	if (fp) {
		line = malloc(LINE_LEN + 3);

		while (fgets(line, LINE_LEN, fp) != NULL) {
			ReadAfmLine();
		}

		fclose(fp);
		free(line);
	}

	AfmSetEncoding("ISOLatin1");
}

int
AfmFontHeight(void)
{
	if (! afm) {
		/* FIX ME */
		io_info_msg("AfmFontHeight: failed\n");
		return 0;
	}
	return afm->FontBBox4 * CurrentFontSize / 1000;
}

int
AfmFontWidth(void)
{
	if (! afm) {
		/* FIX ME */
		io_info_msg("AfmFontWidth: failed\n");
		return 8;
	}
	return afm->FontBBox3 * CurrentFontSize / 1000;
}

int
AfmPitch(void)
{
	float	p;

	if (! afm) {
		io_info_msg("AfmPitch: failed\n");
		return 10;	/* FIX ME this is the fallback value */
	}
	p = 72.0 * 1000 / (afm->FontBBox3 * CurrentFontSize);
	return p;
}

int
AfmStringWidth(char *s)
{
	int	r, i, cw;
	unsigned int	pc;
	char	*p;

	if (s == NULL || afm == 0)
		return 0;

	if (afm->IsFixedPitch)
		r = strlen(s) * afm->FontBBox3 * CurrentFontSize / 1000;
	else {
		if (afm->charMetrics == 0) {
			return 0;	/* FIX ME */
		}
		for (r=0, p=s; *p; p++) {
			pc = 255 & *p;
			cw = 0;
#if 1	/* Use quick access */
			if (afm->charIndex[pc] >= 0)
				cw = afm->charMetrics[afm->charIndex[pc]].width;
#else
			for (i=0; cw == 0 && i<afm->StartCharMetrics; i++)
				if (afm->charMetrics[i].code == pc) {
					cw = afm->charMetrics[i].width;
					break;
				}
#endif
			if (cw == 0) {
				fprintf(stderr, "Width of char(%c) not found\n", *p);
/*				return 0;	*/
			}
			r += cw;
		}
		r = r * CurrentFontSize / 1000;
	}

#if 0
	fprintf(stderr, "AfmStringWidth(%s) -> %d\n", s, r);
#endif

	return r;
}

static char *IsoLatin1Encoding[256] = {
	/* 0x00 */	"non-printable",
	/* 0x01 */	"non-printable",
	/* 0x02 */	"non-printable",
	/* 0x03 */	"non-printable",
	/* 0x04 */	"non-printable",
	/* 0x05 */	"non-printable",
	/* 0x06 */	"non-printable",
	/* 0x07 */	"non-printable",
	/* 0x08 */	"non-printable",
	/* 0x09 */	"non-printable",
	/* 0x0a */	"non-printable",
	/* 0x0b */	"non-printable",
	/* 0x0c */	"non-printable",
	/* 0x0d */	"non-printable",
	/* 0x0e */	"non-printable",
	/* 0x0f */	"non-printable",
	/* 0x10 */	"non-printable",
	/* 0x11 */	"non-printable",
	/* 0x12 */	"non-printable",
	/* 0x13 */	"non-printable",
	/* 0x14 */	"non-printable",
	/* 0x15 */	"non-printable",
	/* 0x16 */	"non-printable",
	/* 0x17 */	"non-printable",
	/* 0x18 */	"non-printable",
	/* 0x19 */	"non-printable",
	/* 0x1a */	"non-printable",
	/* 0x1b */	"non-printable",
	/* 0x1c */	"non-printable",
	/* 0x1d */	"non-printable",
	/* 0x1e */	"non-printable",
	/* 0x1f */	"non-printable",
	/* 0x20 */	"/space",
	/* 0x21 */	"/exclam",
	/* 0x22 */	"/quotedbl",
	/* 0x23 */	"/numbersign",
	/* 0x24 */	"/dollar",
	/* 0x25 */	"/percent",
	/* 0x26 */	"/ampersand",
	/* 0x27 */	"/quoteright",
	/* 0x28 */	"/parenleft",
	/* 0x29 */	"/parenright",
	/* 0x2a */	"/asterisk",
	/* 0x2b */	"/plus",
	/* 0x2c */	"/comma",
	/* 0x2d */	"/hyphen",
	/* 0x2e */	"/period",
	/* 0x2f */	"/slash",
	/* 0x30 */	"/zero",
	/* 0x31 */	"/one",
	/* 0x32 */	"/two",
	/* 0x33 */	"/three",
	/* 0x34 */	"/four",
	/* 0x35 */	"/five",
	/* 0x36 */	"/six",
	/* 0x37 */	"/seven",
	/* 0x38 */	"/eight",
	/* 0x39 */	"/nine",
	/* 0x3a */	"/colon",
	/* 0x3b */	"/semicolon",
	/* 0x3c */	"/less",
	/* 0x3d */	"/equal",
	/* 0x3e */	"/greater",
	/* 0x3f */	"/question",
	/* 0x40 */	"/at",
	/* 0x41 */	"/A",
	/* 0x42 */	"/B",
	/* 0x43 */	"/C",
	/* 0x44 */	"/D",
	/* 0x45 */	"/E",
	/* 0x46 */	"/F",
	/* 0x47 */	"/G",
	/* 0x48 */	"/H",
	/* 0x49 */	"/I",
	/* 0x4a */	"/J",
	/* 0x4b */	"/K",
	/* 0x4c */	"/L",
	/* 0x4d */	"/M",
	/* 0x4e */	"/N",
	/* 0x4f */	"/O",
	/* 0x50 */	"/P",
	/* 0x51 */	"/Q",
	/* 0x52 */	"/R",
	/* 0x53 */	"/S",
	/* 0x54 */	"/T",
	/* 0x55 */	"/U",
	/* 0x56 */	"/V",
	/* 0x57 */	"/W",
	/* 0x58 */	"/X",
	/* 0x59 */	"/Y",
	/* 0x5a */	"/Z",
	/* 0x5b */	"/bracketleft",
	/* 0x5c */	"/backslash",
	/* 0x5d */	"/bracketright",
	/* 0x5e */	"/asciicircum",
	/* 0x5f */	"/underscore",
	/* 0x60 */	"/quoteleft",
	/* 0x61 */	"/a",
	/* 0x62 */	"/b",
	/* 0x63 */	"/c",
	/* 0x64 */	"/d",
	/* 0x65 */	"/e",
	/* 0x66 */	"/f",
	/* 0x67 */	"/g",
	/* 0x68 */	"/h",
	/* 0x69 */	"/i",
	/* 0x6a */	"/j",
	/* 0x6b */	"/k",
	/* 0x6c */	"/l",
	/* 0x6d */	"/m",
	/* 0x6e */	"/n",
	/* 0x6f */	"/o",
	/* 0x70 */	"/p",
	/* 0x71 */	"/q",
	/* 0x72 */	"/r",
	/* 0x73 */	"/s",
	/* 0x74 */	"/t",
	/* 0x75 */	"/u",
	/* 0x76 */	"/v",
	/* 0x77 */	"/w",
	/* 0x78 */	"/x",
	/* 0x79 */	"/y",
	/* 0x7a */	"/z",
	/* 0x7b */	"/braceleft",
	/* 0x7c */	"/bar",
	/* 0x7d */	"/braceright",
	/* 0x7e */	"/tilde",
	/* 0x7f */	"non-printable",
	/* 0x80 */	"non-printable",
	/* 0x81 */	"non-printable",
	/* 0x82 */	"non-printable",
	/* 0x83 */	"non-printable",
	/* 0x84 */	"non-printable",
	/* 0x85 */	"non-printable",
	/* 0x86 */	"non-printable",
	/* 0x87 */	"non-printable",
	/* 0x88 */	"non-printable",
	/* 0x89 */	"non-printable",
	/* 0x8a */	"non-printable",
	/* 0x8b */	"non-printable",
	/* 0x8c */	"non-printable",
	/* 0x8d */	"non-printable",
	/* 0x8e */	"non-printable",
	/* 0x8f */	"non-printable",
	/* 0x90 */	"non-printable",
	/* 0x91 */	"non-printable",
	/* 0x92 */	"non-printable",
	/* 0x93 */	"non-printable",
	/* 0x94 */	"non-printable",
	/* 0x95 */	"non-printable",
	/* 0x96 */	"non-printable",
	/* 0x97 */	"non-printable",
	/* 0x98 */	"non-printable",
	/* 0x99 */	"non-printable",
	/* 0x9a */	"non-printable",
	/* 0x9b */	"non-printable",
	/* 0x9c */	"non-printable",
	/* 0x9d */	"non-printable",
	/* 0x9e */	"non-printable",
	/* 0x9f */	"non-printable",
	/* 0xa0 */	"/space",
	/* 0xa1 */	"/exclamdown",
	/* 0xa2 */	"/cent",
	/* 0xa3 */	"/sterling",
	/* 0xa4 */	"/currency",
	/* 0xa5 */	"/yen",
	/* 0xa6 */	"/brokenbar",
	/* 0xa7 */	"/section",
	/* 0xa8 */	"/dieresis",
	/* 0xa9 */	"/copyright",
	/* 0xaa */	"/ordfeminine",
	/* 0xab */	"/guillemotleft",
	/* 0xac */	"/logicalnot",
	/* 0xad */	"/hyphen",
	/* 0xae */	"/registered",
	/* 0xaf */	"/macron",
	/* 0xb0 */	"/degree",
	/* 0xb1 */	"/plusminus",
	/* 0xb2 */	"/twosuperior",
	/* 0xb3 */	"/threesuperior",
	/* 0xb4 */	"/acute",
	/* 0xb5 */	"/mu",
	/* 0xb6 */	"/paragraph",
	/* 0xb7 */	"/bullet",
	/* 0xb8 */	"/cedilla",
	/* 0xb9 */	"/dotlessi",
	/* 0xba */	"/ordmasculine",
	/* 0xbb */	"/guillemotright",
	/* 0xbc */	"/onequarter",
	/* 0xbd */	"/onehalf",
	/* 0xbe */	"/threequarters",
	/* 0xbf */	"/questiondown",
	/* 0xc0 */	"/Agrave",
	/* 0xc1 */	"/Aacute",
	/* 0xc2 */	"/Acircumflex",
	/* 0xc3 */	"/Atilde",
	/* 0xc4 */	"/Adieresis",
	/* 0xc5 */	"/Aring",
	/* 0xc6 */	"/AE",
	/* 0xc7 */	"/Ccedilla",
	/* 0xc8 */	"/Egrave",
	/* 0xc9 */	"/Eacute",
	/* 0xca */	"/Ecircumflex",
	/* 0xcb */	"/Edieresis",
	/* 0xcc */	"/Igrave",
	/* 0xcd */	"/Iacute",
	/* 0xce */	"/Icircumflex",
	/* 0xcf */	"/Idieresis",
	/* 0xd0 */	"/Eth",
	/* 0xd1 */	"/Ntilde",
	/* 0xd2 */	"/Ograve",
	/* 0xd3 */	"/Oacute",
	/* 0xd4 */	"/Ocircumflex",
	/* 0xd5 */	"/Otilde",
	/* 0xd6 */	"/Odieresis",
	/* 0xd7 */	"/multiply",
	/* 0xd8 */	"/Oslash",
	/* 0xd9 */	"/Ugrave",
	/* 0xda */	"/Uacute",
	/* 0xdb */	"/Ucircumflex",
	/* 0xdc */	"/Udieresis",
	/* 0xdd */	"/Yacute",
	/* 0xde */	"/Thorn",
	/* 0xdf */	"/germandbls",
	/* 0xe0 */	"/agrave",
	/* 0xe1 */	"/aacute",
	/* 0xe2 */	"/acircumflex",
	/* 0xe3 */	"/atilde",
	/* 0xe4 */	"/adieresis",
	/* 0xe5 */	"/aring",
	/* 0xe6 */	"/ae",
	/* 0xe7 */	"/ccedilla",
	/* 0xe8 */	"/egrave",
	/* 0xe9 */	"/eacute",
	/* 0xea */	"/ecircumflex",
	/* 0xeb */	"/edieresis",
	/* 0xec */	"/igrave",
	/* 0xed */	"/iacute",
	/* 0xee */	"/icircumflex",
	/* 0xef */	"/idieresis",
	/* 0xf0 */	"/eth",
	/* 0xf1 */	"/ntilde",
	/* 0xf2 */	"/ograve",
	/* 0xf3 */	"/oacute",
	/* 0xf4 */	"/ocircumflex",
	/* 0xf5 */	"/otilde",
	/* 0xf6 */	"/odieresis",
	/* 0xf7 */	"/divide",
	/* 0xf8 */	"/oslash",
	/* 0xf9 */	"/ugrave",
	/* 0xfa */	"/uacute",
	/* 0xfb */	"/ucircumflex",
	/* 0xfc */	"/udieresis",
	/* 0xfd */	"/yacute",
	/* 0xfe */	"/thorn",
	/* 0xff */	"/ydieresis"
};

/*
 * Process the encoding
 */
static void
DoEncoding(void)
{
	int	i, j;

	if (! afm)
		return;

	for (i=0; i<afm->StartCharMetrics; i++) {
		afm->charMetrics[i].code = -1;

		for (j=0; j<256; j++) {
			if (strcmp(IsoLatin1Encoding[j] + 1, afm->charMetrics[i].name) == 0) {
				afm->charMetrics[i].code = j;
				break;
			}
		}
	}

	/* Build the quick access */
	for (i=0; i<256; i++)
		afm->charIndex[i] = -1;
	for (i=0; i<afm->StartCharMetrics; i++)
		if (afm->charMetrics[i].code > 0 && afm->charMetrics[i].code < 256)
			afm->charIndex[afm->charMetrics[i].code] = i;
}

void
AfmSetEncoding(const char *enc)
{
#if 0
	fprintf(stderr, "AfmSetEncoding(%s)\n", enc);
#endif
	DoEncoding();

	/* FIX ME */
}
