/*
 * $Id: format.c,v 1.14 2001/02/13 23:38:05 danny Exp $
 *
 * Copyright (c) 1993, 2001 Free Software Foundation, Inc.
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
#include <string.h>

#include "global.h"
#include "format.h"
#include "cell.h"

import errors;

static const char * fmt_to_str (int format, int precision)
{
	const char *ptr;
	static char buf[30];
	char nbuf[10];

	nbuf[0] = '\0';

	auto set_nbuf = [&](int prec) {
		snprintf(nbuf, sizeof(nbuf), "%d", prec);
	};

	switch (format) {
		case FMT_USR:
			ptr = "user-";
			set_nbuf(precision + 1);
			break;
		case FMT_GEN:
			ptr = "general.";
			set_nbuf(precision);
			break;
		case FMT_DOL:
			ptr = "dollar.";
			set_nbuf(precision);
			break;
		case FMT_CMA:
			ptr = "comma.";
			set_nbuf(precision);
			break;
		case FMT_PCT:
			ptr = "percent.";
			set_nbuf(precision);
			break;
		case FMT_FXT:
			if (precision == 0)
				return "integer";
#if 0
			if (format == FMT_FXT)
				return "decimal";				/* What's a decimal ? */
#endif
			ptr = "fixed.";
			set_nbuf(precision);
			break;
		case FMT_EXP:
			ptr = "exponent.";
			set_nbuf(precision);
			break;
		default:
			raise_error("Unknown format %d (%x)", format, format);
			ptr = "UNKNOWN";
			break;
	}
	snprintf(buf, sizeof(buf), "%s%s", ptr, nbuf);
	return buf;
}

/*
 * This is a replacement for fmt_to_str() above which used to have a dirty
 * interface (mixing format and precision).
 * Actually it was called in only two places, preceeded in both cases by
 * GET_FORMAT(cp), so I contracted GET_FORMAT and fmt_to_str.
 *
 * This was one of the calls. Nice huh ?
 * p->String = fmt_to_str ((cell_ptr = find_cell (row, col)) ? GET_FORMAT (cell_ptr) : 0);
 */

 const char *cell_format_string(CELL *cp)
{
	static char no_default[] = "";

	/* FIXME: Doesn't anything! */
	if (cp == NULL)
		return fmt_to_str(0, 0);
	else
		return no_default;


}

struct fmt
{
	int fmt;
	const char **strs;
};




const char *gen_names[] =	{"general.",	"gen.", "G", 0};
const char *dol_names[] =	{"dollar.",	"dol.", "$", 0};
const char *cma_names[] =	{"comma.",	"com.", ",", 0};
const char *pct_names[] =	{"percent.",	"pct.", "%", 0};
const char *fxt_names[] =	{"fixed.",	"fxt.", "F", 0};
const char *exp_names[] =	{"exponent.",	"exp.", "E", 0};



int chr_to_jst (int chr)
{
	if (chr == 'd' || chr == 'D')
		return JST_DEF;
	if (chr == 'l' || chr == 'L')
		return JST_LFT;
	if (chr == 'r' || chr == 'R')
		return JST_RGT;
	if (chr == 'c' || chr == 'C')
		return JST_CNT;
	return -1;
}


std::string string_coord (CELLREF r, CELLREF c)
{
	return string_format("R%dC%d", r, c);
}

std::string string_coord (coord_t coord)
{
	CELLREF r = get_row(coord);
	CELLREF c = get_col(coord);
	return string_coord(r, c);
}
