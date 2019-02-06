/*
 * Copyright © 1990-2000, 2001 Free Software Foundation, Inc.
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

#include <ctype.h>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "byte-compile.h"
#include "convert.h"
#include "format.h"
#include "io-generic.h"
#include "io-abstract.h"
#include "io-utils.h"
#include "io-term.h"
#include "global.h"
#include "sheet.h"
#include "ref.h"
#include "regions.h"
#include "window.h"
#include "spans.h"
#include "utils.h"
#include "xcept.h"

/* These functions read and write OLEO style files. */

/* We're reading in a cell, whose formula is FORM, and whose current value
   is VAL.  Parse both of them. . .  (Parsing of VAL is quite primitive)
 */
char *
read_new_value (CELLREF row, CELLREF col, char *form, char *val)
{
	char* text;
	if(val) text = val;
	if(form) text = form;
	assert(text);
	set_cell_input(row, col, text);
	my_cell = 0;
	return 0;
}

void
oleo_read_file (FILE *fp, int ismerge)
{
	char *ptr;
	CELLREF crow = 0, ccol = 0, czrow = 0, czcol = 0;
	int lineno;
	char cbuf[1024];
	char expbuf[1024];
	char *vname, *vval;
	int vlen = 0;
	int cprot;
	char *cexp, *cval;
	CELL *cp;
	struct rng rng;
	int fmt = 0, prc = 0;
	int jst = 0;
	struct font_memo * fnt = 0;
	struct font_memo ** fnt_map = 0;
	int fnt_map_size = 0;
	int fnt_map_alloc = 0;
	int font_spec_in_format = 1;	/* Reset if we discover this is a v1.1 file. */

	long mx_row = MAX_ROW, mx_col = MAX_COL;
	int old_a0;
	int next_a0;

	Global->return_from_error = 1;

	old_a0 = Global->a0;
	next_a0 = old_a0;
	Global->a0 = 0;
	lineno = 0;
	if (!ismerge)
		clear_spreadsheet ();
	while (fgets (cbuf, sizeof (cbuf), fp))
	{
		std::string input_line = cbuf;
		lineno++;

		if ((ptr = (char *)index (cbuf, '\n')))
			*ptr = '\0';

		ptr = cbuf;
		switch (*ptr)
		{
			case '#':		/* comment line -- ignored */
				break;
			case '%':		/* Font or pixel size data. */
				ptr++;
				switch (*ptr) {
					case 'F':		/* %F font-name */
						if (fnt_map_size == fnt_map_alloc)
						{
							fnt_map_alloc = (fnt_map_alloc + 1) * 2;
							fnt_map =
								((struct font_memo **)
								 ck_remalloc
								 (fnt_map, fnt_map_alloc * sizeof (struct font_memo *)));
						}
						//fnt_map[fnt_map_size++] = parsed_matching_font (ptr + 1);
						break;
					case 'f':		/* %f range font-name */
						{
							struct rng rng;
							/* This field only occurs in files written by 1.1
							 * oleo.  It's presense indicates that when parsing
							 * format fields, we should *not* reset cell fonts to 0.
							 */
							font_spec_in_format = 0;
							++ptr;
							while (isspace (*ptr))
								++ptr;
							if (!parse_cell_or_range (&ptr, &rng))
								goto bad_field;
							while (isspace (*ptr))
								++ptr;
							{
							}
							break;
						}
					default:		/* % with something invalid */
						goto bad_field;
				}
				break;
			case 'F':		/* Format field */
				vlen = 0;
				ptr++;
				fnt = 0;	/* The font must be explicitly overriden for a cell. */
				while (*ptr)
				{
					if (*ptr != ';')
						goto bad_field;
					ptr++;
					switch (*ptr++) {
						int clo, chi, cwid;
						case 'C':	/* Column from rows 1 to 255 */
						czcol = astol (&ptr);
						vlen = 2;
						break;

						case 'D':	/* Default format */
						switch (*ptr++)
						{
							case 'G':
								default_fmt = FMT_GEN;
								break;
							case 'E':
								default_fmt = FMT_EXP;
								break;
							case 'F':
								default_fmt = FMT_FXT;
								break;
							case '$':
								default_fmt = FMT_DOL;
								break;
							case '*':	/* * format implemented as +- format */
								default_fmt = FMT_GPH;
								break;
							case ',':	/* JF */
								default_fmt = FMT_CMA;
								break;
							case 'U':
								default_fmt = FMT_USR;
								break;
							case '%':
								default_fmt = FMT_PCT;
								break;
							case 'H':
								default_fmt = FMT_HID;
								break;
							case 'd':	/* Date */
								default_fmt = FMT_DATE;
								break;
								/* End of JF */
							default:
								io_error_msg ("Line %d: format %c not supported", lineno, ptr[-1]);
								break;
						}
						if (*ptr == 'F')
						{
							prc = default_prc = FLOAT_PRECISION;
							ptr++;
						}
						else
							default_prc = prc = astol (&ptr);

						switch (*ptr++)
						{
							case 'C':
								default_jst = JST_CNT;
								break;
							case 'L':
								default_jst = JST_LFT;
								break;
							case 'R':
								default_jst = JST_RGT;
								break;
							case 'G':	/* General format not supported */
							default:
								io_error_msg ("Line %d: Alignment %c not supported", lineno, ptr[-1]);
								break;
						}
						default_width = astol (&ptr);
						break;

						case 'f': /* Font specification */
						{
							int id;
							id = astol(&ptr);
							if (id < 0 || id >= fnt_map_size)
							{
								io_error_msg ("Line %d: Undefined font (%d)\n",
										lineno, id);
								break;
							}
							fnt = fnt_map[id];
							break;
						}

						case 'F':
						switch (*ptr++)
						{
							case 'D':
								fmt = FMT_DEF;
								break;
							case 'G':
								fmt = FMT_GEN;
								break;
							case 'E':
								fmt = FMT_EXP;
								break;
							case 'F':
								fmt = FMT_FXT;
								break;
							case '$':
								fmt = FMT_DOL;
								break;
							case '*':	/* JF implemented as +- format */
								fmt = FMT_GPH;
								break;
							case ',':	/* JF */
								fmt = FMT_CMA;
								break;
							case 'U':
								fmt = FMT_USR;
								break;
							case '%':
								fmt = FMT_PCT;
								break;
							case 'H':
								fmt = FMT_HID;
								break;	/* END of JF */
							case 'd':
								fmt = FMT_DATE;
								break;
							case 'C':
							default:
								io_error_msg ("Line %d: format %c not supported", lineno, ptr[-1]);
								fmt = FMT_DEF;
								break;
						}
						if (*ptr == 'F') {
							prc = FLOAT_PRECISION;
							ptr++;
						} else {
							prc = astol(&ptr);
						}
						switch (*ptr++)
						{
							case 'C':
								jst = JST_CNT;
								break;
							case 'L':
								jst = JST_LFT;
								break;
							case 'R':
								jst = JST_RGT;
								break;
							case 'D':
								jst = JST_DEF;
								break;
							default:
								io_error_msg ("Line %d: Alignment %c not supported", lineno, ptr[-1]);
								jst = JST_DEF;
								break;
						}
						vlen = 1;
						break;
						case 'R':	/* Row from cols 1 to 63 */
						czrow = astol (&ptr);
						vlen = 4;
						break;

						case 'W':	/* Width of clo to chi is cwid */
						clo = astol (&ptr);
						chi = astol (&ptr);
						cwid = astol (&ptr) + 1;
						for (; clo <= chi; clo++) {
							set_width (clo, cwid);
						}
						break;

						case 'H':	/* JF: extension */
						clo = astol (&ptr);
						chi = astol (&ptr);
						cwid = astol (&ptr) + 1;
						for (; clo <= chi; clo++)
							set_height (clo, cwid);
						break;
						case 'c':
						ccol = astol (&ptr);
						break;
						case 'r':
						crow = astol (&ptr);
						break;

						default:
						goto bad_field;
					}
				}
				switch (vlen)
				{
					case 1:
						cp = find_or_make_cell (crow, ccol);
						SET_FORMAT (cp, fmt);
						SET_PRECISION(cp, prc);
						SET_JST (cp, jst);
						//if (font_spec_in_format) cp->cell_font = fnt;
						break;
					case 2:
						rng.lr = MIN_ROW;
						rng.lc = czcol;
						rng.hr = mx_row;
						rng.hc = czcol;
						make_cells_in_range (&rng);
						for(CELL* cp:get_cells_in_range(&rng))
						{
							SET_FORMAT (cp, fmt);
							SET_PRECISION(cp, prc);
							SET_JST (cp, jst);
						}
						break;
					case 4:
						rng.lr = czrow;
						rng.lc = MIN_COL;
						rng.hr = czrow;
						rng.hc = mx_col;
						make_cells_in_range (&rng);
						for(CELL* cp:get_cells_in_range(&rng))
						{
							SET_FORMAT (cp, fmt);
							SET_JST (cp, jst);
						}
						break;
					default:
						break;
				}
				break;

			case 'B':		/* Boundry field, ignored */
				ptr++;
				while (*ptr)
				{
					if (*ptr != ';')
						goto bad_field;
					ptr++;
					switch (*ptr++)
					{
						case 'c':
							mx_col = astol (&ptr);
							if (mx_col > MAX_COL)
							{
								io_error_msg ("Boundry column %lu too large!", mx_col);
								mx_col = MAX_COL;
							}
							break;
						case 'r':
							mx_row = astol (&ptr);
							if (mx_row > MAX_ROW)
							{
								io_error_msg ("Boundry row %lu too large!", mx_row);
								mx_row = MAX_ROW;
							}
							break;
						default:
							goto bad_field;
					}
				}
				break;

			case 'N':		/* A Name field */
				if (ptr[1] != 'N')
					goto bad_field;
				ptr += 2;
				vname = 0;
				vval = 0;
				while (*ptr)
				{
					if (*ptr != ';')
						goto bad_field;
					*ptr++ = '\0';
					switch (*ptr++)
					{
						case 'N':	/* Name is */
							vname = ptr;
							while (*ptr && *ptr != ';')
								ptr++;
							vlen = ptr - vname;
							break;
						case 'E':	/* Expression is */
							vval = ptr;
							while (*ptr && *ptr != ';')
								ptr++;
							break;
						default:
							--ptr;
							goto bad_field;
					}
				}
				if (!vname || !vval)
					goto bad_field;
				*ptr = '\0';
				ptr = old_new_var_value (vname, vlen, vval);
				if (ptr)
					io_error_msg ("Line %d: Couldn't set %.*s to %s: %s", lineno, vlen, vname, vval, ptr);
				break;

			case 'C':		/* A Cell entry */
				cprot = 0;
				cval = 0;
				cexp = 0;
				cval = 0;
				ptr++;
				while (*ptr)
				{
					int quotes;

					if (*ptr != ';')
						goto bad_field;
					*ptr++ = '\0';
					switch (*ptr++)
					{
						case 'c':
							ccol = astol (&ptr);
							break;
						case 'r':
							crow = astol (&ptr);
							break;
						case 'R':
							czrow = astol (&ptr);
							break;
						case 'C':
							czcol = astol (&ptr);
							break;
						case 'P':	/* This cell is Protected */
							cprot++;
							break;
						case 'K':	/* This cell's Konstant value */
							cval = ptr;
							quotes = 0;
							while (*ptr && (*ptr != ';' || quotes > 0))
								if (*ptr++ == '"')
									quotes = !quotes;
							break;
						case 'E':	/* This cell's Expression */
							cexp = ptr;
							quotes = 0;
							while (*ptr && (*ptr != ';' || quotes > 0))
								if (*ptr++ == '"')
									quotes = !quotes;

							break;
						case 'G':
							strcpy (expbuf, cval);
							break;
						case 'D':
							strcpy (expbuf, cexp);
							break;
						case 'S':
							cexp = expbuf;
							break;
						default:
							--ptr;
							goto bad_field;
					}
				}
				*ptr = '\0';
				if (cexp && cval && strcmp (cexp, cval))
				{
					ptr = read_new_value (crow, ccol, cexp, cval);
					if (ptr)
					{
						io_error_msg ("Line %d: %d,%d: Read '%s' %s", lineno, crow, ccol, cexp, ptr);
						break;
					}
				}
				else if (cval)
				{
					ptr = read_new_value (crow, ccol, 0, cval);
					if (ptr)
					{
						io_error_msg ("Line %d: %d,%d: Val '%s' %s", lineno, crow, ccol, cexp, ptr);
						break;
					}
				}
				else if (cexp)
				{
					ptr = read_new_value (crow, ccol, cexp, 0);
					if (ptr)
					{
						io_error_msg ("Line %d: %d,%d: Exp '%s' %s", lineno, crow, ccol, cexp, ptr);
						break;
					}
				}
				if (cprot)
					SET_LCK (find_or_make_cell (crow, ccol), LCK_LCK);
				if (ismerge)
					push_cell (crow, ccol);
				/* ... */
				break;
			case 'E':	/* End of input ?? */
				break;
			case 'W':
				io_read_window_config (ptr + 2);
				break;
			case 'U':
				/* JF extension:  read user-defined formats */
				read_mp_usr_fmt (ptr + 1);
				break;
				/* JF extension: read uset-settable options */
			case 'O':
				Global->a0 = next_a0;
				read_mp_options (ptr + 2);
				next_a0 = Global->a0;
				Global->a0 = 0;
				break;
			case 'G':	/* Graph data */
				break;
			case 'D':	/* Database Access */
				break;
			default:
bad_field:
				{
					Global->a0 = old_a0;
					if (!ismerge)
						clear_spreadsheet ();
					io_recenter_all_win ();
					std::string fmt{"Line %d: Unknown OLEO line \"%s\""};
					std::string msg{string_format(fmt, lineno, input_line.c_str())};
					msg = trim(msg);
					throw SyntaxError(msg);
					Global->return_from_error = 0;
					return;
					}
		}	/* End of switch */
	}
	if (!feof (fp)) {
		if (!ismerge)
			clear_spreadsheet ();
		io_recenter_all_win ();
		io_error_msg ("read-file: read-error near line %d.", lineno);
		Global->return_from_error = 0;
		return;
	}
	Global->a0 = next_a0;
	io_recenter_all_win ();

	Global->return_from_error = 0;
}

static char * oleo_fmt_to_str (int f1, int p1)
{
	static char p_buf[40];

	p_buf[1] = '\0';
	switch (f1)
	{
		case FMT_DEF:
			p_buf[0] = 'D';
			break;
		case FMT_HID:
			p_buf[0] = 'H';
			break;
		case FMT_GPH:
			p_buf[0] = '*';
			break;
		default:
			if (p1 == FLOAT_PRECISION)
			{
				p_buf[1] = 'F';
				p_buf[2] = '\0';
			}
			else
				sprintf (&p_buf[1], "%d", p1);

			switch (f1)
			{
				case FMT_USR:
					p_buf[0] = 'U';
					break;
				case FMT_GEN:
					p_buf[0] = 'G';
					break;
				case FMT_DOL:
					p_buf[0] = '$';
					break;
				case FMT_PCT:
					p_buf[0] = '%';
					break;
				case FMT_FXT:
					p_buf[0] = 'F';
					break;
				case FMT_CMA:
					p_buf[0] = ',';
					break;
				case FMT_EXP:
					p_buf[0] = 'E';
					break;
				case FMT_DATE:
					p_buf[0] = 'd';
					break;
				default:
					p_buf[0] = '?';
					break;
			}
			break;
	}
	return p_buf;
}

static char jst_to_chr ( int just)
{
	switch (just)
	{
		case JST_DEF:
			return 'D';
		case JST_LFT:
			return 'L';
		case JST_RGT:
			return 'R';
		case JST_CNT:
			return 'C';
		default:
			return '?';
	}
}

static FILE *oleo_fp;
static struct rng *oleo_rng;

static void oleo_write_var ( char *name, struct var *var)
{
	if (var->var_flags == VAR_UNDEF
			&& (!var->var_ref_fm || var->var_ref_fm->refs_used == 0))
		return;

	switch (var->var_flags) {
		case VAR_UNDEF:
			break;
		case VAR_CELL:
			if (var->v_rng.lr >= oleo_rng->lr && var->v_rng.lr <= oleo_rng->hr
					&& var->v_rng.lc >= oleo_rng->lc && var->v_rng.lc <= oleo_rng->hc)
				(void) fprintf(oleo_fp, "NN;N%s;E%s\n",
						var->var_name.c_str(), cell_name(var->v_rng.lr, var->v_rng.lc));
			break;
		case VAR_RANGE:
			if (var->v_rng.lr < oleo_rng->lr || var->v_rng.hr > oleo_rng->hr
					|| var->v_rng.lc < oleo_rng->lc || var->v_rng.hc > oleo_rng->hc)
				break;

			(void) fprintf(oleo_fp, "NN;N%s;E%s\n", var->var_name.c_str(), 
					range_name (&(var->v_rng)));
			break;
	}
}

static void write_mp_windows ( FILE *fp)
{
	//struct line line;

	//line.alloc = 0;
	//line.buf = 0;
	std::string str = io_write_window_config();
	fputs(str.c_str(), fp);
	//free (line.buf);
}

void write_spans(FILE* fp, span_find_t& s_find, char typechar)
{
	CELLREF c;
	unsigned short w = next_span(s_find, c);
	while (w)
	{
		CELLREF cc, ccc;
		unsigned short ww;
		cc = c;
		do
			ww = next_span(s_find, ccc);
		while (ccc == ++cc && ww == w);
		(void) fprintf (fp, "F;%c%u %u %u\n", typechar, c, cc - 1, w - 1);
		c = ccc;
		w = ww;
	}
}



void write_cells(FILE* fp)
{
	CELLREF crow = 0, ccol = 0;
	for(CELL* cp: get_cells_in_range(&all_rng))
	{
		coord_t coord = cp->coord;
		CELLREF r = get_row(coord);
		assert(r>0);
		CELLREF c = get_col(coord);
		assert(c>0);

		char *ptr;
		int f1, j1;
		char p_buf[40];

		f1 = GET_FORMAT (cp);
		j1 = GET_JST (cp);
		if (f1 != FMT_DEF || j1 != JST_DEF )
		{
			(void) fprintf (fp, "F;");
			if (c != ccol) {
				(void) fprintf (fp, "c%u;", c);
				ccol = c;
			}
			if (r != crow) {
				(void) fprintf (fp, "r%u;", r);
				crow = r;
			}
			(void) fprintf (fp, "F%s%c\n",
					oleo_fmt_to_str (f1, GET_PRECISION(cp)), jst_to_chr (j1));
		}

		if (!GET_TYP (cp) && !cp->get_cell_formula())
			continue;

		(void) fprintf (fp, "C;");
		if (c != ccol) {
			(void) fprintf (fp, "c%u;", c);
			ccol = c;
		}
		if (r != crow) {
			(void) fprintf (fp, "r%u;", r);
			crow = r;
		}

		const unsigned char* formula_1 = cp->get_cell_formula();
		if (formula_1 && !is_constant(formula_1)) {
			//std::string formula = decomp_str(r, c);
			std::string formula = formula_text(r, c);
			(void) fprintf (fp, "E%s;", formula.c_str());
		}

		value val = cp->get_value();
		std::string strval = stringify_value_file_style(&val);
		if(GET_TYP(cp) != TYP_NUL)
			fprintf(fp, "K%s", strval.c_str());

		if(cp->locked()) fprintf (fp, ";P");

		putc ('\n', fp);
	}
}

void
oleo_write_file(FILE *fp, struct rng *rng)
{
	assert(rng == nullptr); // mcarter 06-May-2018: insist on writing whole spreadsheet
	unsigned short w;
	/* struct var *var; */
	int old_a0, i, fnt_map_size = 0;
	char	*s;

	(void) fprintf (fp, "# This file was created by Neoleo\n");

	/* All versions of the oleo file format should have a 
	 * version cookie on the second line.
	 */
	(void) fprintf (fp, "# format 3.0 (requires Neoleo 8.0 or higher)\n");

	int n;
	int fmts;
	char *data[9];

	rng = &all_rng;

	(void) fprintf (fp, "F;D%s%c%u\n",
			oleo_fmt_to_str (default_fmt, default_prc),
			jst_to_chr (default_jst),
			default_width);

	fmts = usr_set_fmts ();
	for (n = 0; n < 16; n++)
	{
		if (fmts & (1 << n))
		{
			get_usr_stats (n, data);
			fprintf (fp, "U;N%u;P%s;S%s", n + 1, data[7], data[8]);
			if (data[0][0])
				fprintf (fp, ";HP%s", data[0]);
			if (data[1][0])
				fprintf (fp, ";HN%s", data[1]);
			if (data[2][0])
				fprintf (fp, ";TP%s", data[2]);
			if (data[3][0])
				fprintf (fp, ";TN%s", data[3]);
			if (data[4][0])
				fprintf (fp, ";Z%s", data[4]);
			if (data[5][0])
				fprintf (fp, ";C%s", data[5]);
			if (data[6])
				fprintf (fp, ";D%s", data[6]);
			putc ('\n', fp);
		}
	}
	write_mp_options (fp);


	old_a0 = Global->a0;
	Global->a0 = 0;

	span_find_t w_find = find_span(the_wids, rng->lc, rng->hc);
	write_spans(fp, w_find, 'W');

	span_find_t h_find = find_span(the_hgts, rng->lr, rng->hr);
	write_spans(fp, h_find, 'H');

	oleo_fp = fp;
	oleo_rng = rng;
	for_all_vars (oleo_write_var);

	write_cells(fp);

	write_mp_windows (fp);
	(void) fprintf (fp, "E\n");
	Global->a0 = old_a0;
}

int oleo_set_options( int set_opt, char *option)
{
	return -1;
}



