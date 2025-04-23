/*
 * Copyright � 1990-2000, 2001 Free Software Foundation, Inc.
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

#include <format>
#include <string>
#include <ctype.h>
#include <cmath>
#include <map>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <iostream>

//using std::format;
using std::cout;
using std::map;

#include "convert.h"
#include "format.h"
#include "io-utils.h"
#include "global.h"
#include "sheet.h"
#include "ref.h"
#include "regions.h"
#include "window.h"
#include "spans.h"
#include "utils.h"
#include "logging.h"
#include "oleofile.h"

#include "io-curses.h"


const map<char, int>  format_map{{'D', FMT_DEF}, {'G', FMT_GEN}, {'E', FMT_EXP}, {'F', FMT_FXT}, {'$', FMT_DOL},
	{',', FMT_CMA}, {'U', FMT_USR}, {'%', FMT_PCT}, {'H', FMT_HID}, {'d', FMT_DATE}};

const map<char, int>  jst_map{{'C', JST_CNT}, {'D', JST_DEF}, {'L', JST_LFT}, {'R', JST_RGT}};

// 25/4 let's try to abstract away some stuff
// olf_ prefix refers to "oleo file"
void olf_set_optionsXXX (char *opts) // FN
{
	// 25/4 We should probably do soemthing here
}
void olf_do_set_optionXXX (char *str) // FN
{
	// 25/4 We should probably do soemthing here
}

/*
void olf_recenter_window(window* win)
{

}
*/

void olf_io_recenter_all_win ()
{

}

void read_mp_usr_fmt (char *ptr) // FN
{
	int usr_n = -1;
	int n_chrs = 0;
	char *p = ptr;

	while (*p == ';')
	{
		*p++ = '\0';
		switch (*p++)
		{
			case 'N':
				usr_n = astol (&p) - 1;
				break;
			case 'H':
				switch (*p++)
				{
					case 'P':
						//i = 0;
						break;
					case 'N':
						//i = 1;
						break;
					default:
						goto badline;
				}
				goto count_chars;
			case 'T':
				switch (*p++)
				{
					case 'P':
						//i = 2;
						break;
					case 'N':
						//i = 3;
						break;
					default:
						goto badline;
				}
				goto count_chars;

			case 'Z':
				//i = 4;
				goto count_chars;

			case 'C':
				//i = 5;
				goto count_chars;

			case 'D':
				//i = 6;
				goto count_chars;

			case 'P':
				//i = 7;
				goto count_chars;

			case 'S':
				//i = 8;
				goto count_chars;

count_chars:
				n_chrs++;
				while (*p && *p != ';')
				{
					p++;
					n_chrs++;
				}
				break;

			default:
badline:
				raise_error("Unknown OLEO line %s", ptr);
				return;
		}
	}
	if (*p || usr_n < 0 || usr_n > 15)
		goto badline;

	
}


void read_mp_options (char *str) // FN
{
	char *np;

	while ((np = (char *)index (str, ';')))
	{
		*np = '\0';
		//olf_do_set_option (str);
		*np++ = ';';
		str = np;
	}
	if ((np = (char *)rindex (str, '\n')))
		*np = '\0';
	//olf_do_set_option (str);
}




/* These functions read and write OLEO style files. */

/* We're reading in a cell, whose formula is FORM, and whose current value
   is VAL.  Parse both of them. . .  (Parsing of VAL is quite primitive)
 */
void
read_new_value (CELLREF row, CELLREF col, char *form, char *val)
{
	if(form) {
		set_cell_input(row, col, form);
		return;
	}

	if(val) {
                set_cell_input(row, col, val);
		return;
	}
	assert(false);

}

static bool read_fmt_line(char **cptr, CELLREF &crow, CELLREF &ccol, CELLREF &czrow, CELLREF &czcol, int &lineno, int &fnt_map_size, long &mx_row, long &mx_col);

void oleo_read_window_config (char * line)
{
	//int wnum = 0;
	char *text;
	//CELLREF nrow = NON_ROW, ncol = NON_COL;
	char *opts = 0;
	//struct window *win;

	text = line;
	auto eat = [&]() { while (*text && *text != ';') text++; };
	for (;;)
	{
		switch (*text++)
		{
			case 'A': // cursor at
				//log("oleo_read_window_config:A");
				curow = astol (&text);
				cucol = astol (&text);
				break;
				/* JF: Window options */
			case 'O': // options
			case 'S': /* Split into two windows. 25/4 unsupported */
			case 'C': /* Set Colors NOT supported */
			case 'N': // window number
			case 'B': /* Alternate border NOT supported. . . */
				eat();
				break;
			default:
				--text;
				break;
		}
		if (*text == '\0' || *text == '\n')
			break;
		if (*text != ';')
		{
			char *bad;

			bad = text;
			while (*text && *text != ';')
				text++;
			if (*text)
				*text++ = '\0';
			raise_error("Unknown SYLK window cmd: %s", bad);
			if (!*text)
				break;
		}
		else
			*text++ = '\0';
	}
}


void oleo_read_file (FILE *fp, int ismerge)
{
	char *ptr;
	CELLREF crow = 0, ccol = 0, czrow = 0, czcol = 0;
	int lineno;
	char cbuf[1024];
	char expbuf[1024];
	//int vlen = 0;
	int cprot;
	char *cexp, *cval;
	//CELL *cp;
	//struct rng rng;
	//int fmt = 0, prc = 0;
	//int jst = 0;
	//struct font_memo ** fnt_map = 0;
	int fnt_map_size = 0;

	long mx_row = MAX_ROW, mx_col = MAX_COL;
	int old_a0;
	int next_a0;

	//Global->return_from_error = 1;

	//old_a0 = Global->a0;
	//next_a0 = old_a0;
	//Global->a0 = 0;
	lineno = 0;
	if (!ismerge)
		clear_spreadsheet ();
	while (fgets (cbuf, sizeof (cbuf), fp))
	{
		std::string input_line = cbuf;
		lineno++;
		//cout << "oleofile:lineno:" << lineno << "\n";
		//std::flush;

		if ((ptr = (char *)index (cbuf, '\n')))
			*ptr = '\0';

		ptr = cbuf;
		switch (*ptr)
		{
			case '#':		/* comment line -- ignored */
				break;
			case '%':		/* Font or pixel size data. */
				ASSERT_UNCALLED();
				ptr++;
				switch (*ptr) {
					case 'F':		/* %F font-name */
						throw SyntaxError("Font name no longer handled");
						break;
					case 'f':		/* %f range font-name */
						break;
					default:		/* % with something invalid */
						goto bad_field;
				}
				break;
			case 'F':		/* Format field */
				if(!(read_fmt_line(&ptr, crow, ccol, czrow, czcol, lineno, fnt_map_size, mx_row, mx_col))) {
					goto bad_field;
				}
				break;

			case 'B':		/* Boundry field, ignored */
				ASSERT_UNCALLED();
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
								raise_error("Boundry column %lu too large!", mx_col);
								mx_col = MAX_COL;
							}
							break;
						case 'r':
							mx_row = astol (&ptr);
							if (mx_row > MAX_ROW)
							{
								raise_error("Boundry row %lu too large!", mx_row);
								mx_row = MAX_ROW;
							}
							break;
						default:
							goto bad_field;
					}
				}
				break;

			case 'N':		/* A Name field */
				ASSERT_UNCALLED();
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
				if(cexp || cval) {
					read_new_value (crow, ccol, cexp, cval);
					ptr = 0;
				}

				if (cprot)
					SET_LCK (find_or_make_cell (crow, ccol), LCK_LCK);
				if (ismerge) {
					ASSERT_UNCALLED();
					//push_cell (crow, ccol);
				}
				break;
			case 'E':	/* End of input ?? */
				break;
			case 'W':
				oleo_read_window_config (ptr + 2);
				break;
			case 'U':
				/* JF extension:  read user-defined formats */
				read_mp_usr_fmt (ptr + 1);
				break;
				/* JF extension: read uset-settable options */
			case 'O':
				//break;
				//Global->a0 = next_a0;
				read_mp_options (ptr + 2);
				//next_a0 = Global->a0;
				//Global->a0 = 0;
				break;
			case 'G':	/* Graph data */
				break;
			case 'D':	/* Database Access */
				break;
			default:
bad_field:
				{
					//Global->a0 = old_a0;
					if (!ismerge)
						clear_spreadsheet ();
					olf_io_recenter_all_win ();
					std::string fmt{"Line %d: Unknown OLEO line \"%s\""};
					std::string msg{string_format(fmt, lineno, input_line.c_str())};
					msg = trim(msg);
					throw SyntaxError(msg);
					//Global->return_from_error = 0;
					return;
					}
		}	/* End of switch */
	}
	if (!feof (fp)) {
		if (!ismerge)
			clear_spreadsheet ();
		olf_io_recenter_all_win ();
		raise_error("read-file: read-error near line %d.", lineno);
		//Global->return_from_error = 0;
		return;
	}
	//Global->a0 = next_a0;
	olf_io_recenter_all_win ();

	//Global->return_from_error = 0;
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
		default:
			if (p1 == FLOAT_PRECISION)
			{
				p_buf[1] = 'F';
				p_buf[2] = '\0';
			}
			else
				sprintf (&p_buf[1], "%d", p1);

			p_buf[0] = map_reverse(format_map, f1);
			break;
	}
	return p_buf;
}

/*
25/4
A line beginning with F (for format)

*/
static bool read_fmt_line(char **cptr, CELLREF &crow, CELLREF &ccol, CELLREF &czrow, CELLREF &czcol, int &lineno, int &fnt_map_size, long &mx_row, long &mx_col)
{
	#define ptr (*cptr) // do some refactoring fudging
	ptr++; // The 'F' is already read in, so skip it

	CELL *cp;
	struct rng rng;
	int vlen =0;
	int fmt = 0, prc = 0;
	int jst = 0;
	bool is_bold = false;
	bool is_italic = false;
	std::string err_msg;
	


	// fnt = 0;	/* The font must be explicitly overriden for a cell. */
	while (*ptr)
	{
		if (*ptr != ';') return false; // oh-oh spaghettio
		ptr++;
		switch (*ptr++)
		{
			int clo, chi, cwid;
		case 'C': /* Column from rows 1 to 255 */
			czcol = astol(&ptr);
			vlen = 2;
			break;

		case 'D': /* Default format */
 			err_msg = std::format("Line {}: format {} not supported", lineno, ptr[0]);
			default_fmt = map_or_raise(format_map, *ptr++, err_msg);

			if (*ptr == 'F')
			{
				prc = default_prc = FLOAT_PRECISION;
				ptr++;
			}
			else
				default_prc = prc = astol(&ptr);

			default_jst = jst_map.at(*ptr++);

			// 25/4
			//default_width = astol(&ptr);
			(void) astol(&ptr);
			break;

		case 'f': /* Font specification */
			throw SyntaxError("Font specification no longer handled");
			break;

		case 'B': 
			is_bold = true;
			break;
		case 'F':
			fmt = format_map.at(*ptr++);
			
			/*
			switch (*ptr++)
			{
			case 'D': fmt = FMT_DEF; break;
			case 'G': fmt = FMT_GEN; break;
			case 'E': fmt = FMT_EXP; break;
			case 'F': fmt = FMT_FXT; break;
			case '$': fmt = FMT_DOL; break;
			case ',': fmt = FMT_CMA; break;
			case 'U': fmt = FMT_USR; break;
			case '%': fmt = FMT_PCT; break;
			case 'H': fmt = FMT_HID; break;
			case 'd': fmt = FMT_DATE; break;
			case 'C':
			default:
				raise_error("Line %d: format %c not supported", lineno, ptr[-1]);
				fmt = FMT_DEF;
				break;
			}
			*/
			if (*ptr == 'F')
			{
				prc = FLOAT_PRECISION;
				ptr++;
			} else {
				prc = astol(&ptr);
			}
			jst = jst_map.at(*ptr++);
			/*
			switch (*ptr++)
			{
			case 'C': jst = JST_CNT; break;
			case 'L': jst = JST_LFT; break;
			case 'R': jst = JST_RGT; break;
			case 'D': jst = JST_DEF; break;
			default:
				raise_error("Line %d: Alignment %c not supported", lineno, ptr[-1]);
				jst = JST_DEF;
				break;
			}
			*/
			vlen = 1;
			break;
		case 'I':
			is_italic = true;
			break;
		case 'R': /* Row from cols 1 to 63 */
			czrow = astol(&ptr);
			vlen = 4;
			break;

		case 'W': /* Width of clo to chi is cwid */
			clo = astol(&ptr);
			chi = astol(&ptr);
			cwid = astol(&ptr) + 1;
			for (; clo <= chi; clo++)
			{
				set_width(clo, cwid);
			}
			break;

		case 'H': /* JF: extension */
			// 25/4 all heights are assumed to be 1
			#if 0
			clo = astol(&ptr);
			chi = astol(&ptr);
			cwid = astol(&ptr) + 1;
			for (; clo <= chi; clo++) set_height(clo, cwid);
			#endif
			break;
		case 'c': ccol = astol(&ptr); break;
		case 'r': crow = astol(&ptr); break;

		default:
			//goto bad_field;
			return false;
		}
	}


	auto set_cell_flags = [&](CELL *cp) { //25/4

		SET_FORMAT(cp, fmt);
		SET_PRECISION(cp, prc);
		SET_JST(cp, jst);
		cp->cell_flags.bold = is_bold;
		cp->cell_flags.italic = is_italic;
	};

	switch (vlen)
	{
	case 1:
		cp = find_or_make_cell(crow, ccol);
		set_cell_flags(cp);
		//SET_FORMAT(cp, fmt);
		//SET_PRECISION(cp, prc);
		//SET_JST(cp, jst);
		// if (font_spec_in_format) cp->cell_font = fnt;
		break;
	case 2:
		rng.lr = MIN_ROW;
		rng.lc = czcol;
		rng.hr = mx_row;
		rng.hc = czcol;
		make_cells_in_range(&rng);
		for (CELL *cp : get_cells_in_range(&rng))
		{
			set_cell_flags(cp);
			//SET_FORMAT(cp, fmt);
			//SET_PRECISION(cp, prc);
			//SET_JST(cp, jst);
		}
		break;
	case 4:
		rng.lr = czrow;
		rng.lc = MIN_COL;
		rng.hr = czrow;
		rng.hc = mx_col;
		make_cells_in_range(&rng);
		for (CELL *cp : get_cells_in_range(&rng))
		{
			set_cell_flags(cp);
			//SET_FORMAT(cp, fmt);
			//SET_JST(cp, jst);
		}
		break;
	default:
		break;
	}

	return true;
//bad_field:
//	return false;
}


static FILE *oleo_fp;
static struct rng *oleo_rng;


void write_widths(FILE* fp)
{
	//fprintf(fp, "W");
	span_find_t w_find = find_span(the_wids, MIN_COL, MAX_COL);
	CELLREF c{0};
	unsigned short w = next_span(w_find, c);
	while (w)
	{
		CELLREF cc, ccc;
		unsigned short ww;
		cc = c;
		do
			ww = next_span(w_find, ccc);
		while (ccc == ++cc && ww == w);
		(void) fprintf (fp, "F;%c%u %u %u\n", 'W', c, cc - 1, w - 1);
		c = ccc;
		w = ww;
	}

}


void write_cells(FILE* fp)
{
	CELLREF crow = 0, ccol = 0;
	for(CELL* cp: get_cells_in_range(&all_rng))
	{
		coord_t coord = cp->get_coord();
		CELLREF r = get_row(coord);
		assert(r>0);
		CELLREF c = get_col(coord);
		assert(c>0);

		int f1, j1;

		f1 = GET_FORMAT (cp);
		j1 = GET_JST (cp);
		bool is_bold = cp->cell_flags.bold;
		bool is_italic = cp->cell_flags.italic;
		if (f1 != FMT_DEF || j1 != JST_DEF || is_bold || is_italic)
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

			if(is_bold) fprintf(fp, "B;");
			if(is_italic) fprintf(fp, "I;");
			fprintf (fp, "F");
			
			fprintf(fp, "%s", oleo_fmt_to_str (f1, GET_PRECISION(cp)));
			//fprintf(fp, "%c\n", jst_to_chr (j1));
			fprintf(fp, "%c\n", map_reverse(jst_map, j1));
		}

		//if (!GET_TYP (cp) && !cp->get_cell_formula()) continue;
		if (cp->get_type() == TYP_NUL) continue;

		(void) fprintf (fp, "C;");
		if (c != ccol) {
			(void) fprintf (fp, "c%u;", c);
			ccol = c;
		}
		if (r != crow) {
			(void) fprintf (fp, "r%u;", r);
			crow = r;
		}

		
		//const unsigned char* formula_1 = cp->get_bytecode();
		//if (formula_1  && !is_constant(formula_1)  ) {
			std::string formula = formula_text(r, c);
			(void) fprintf (fp, "E%s;", formula.c_str());
		//}
		


		value_t val = cp->get_value_2019();
		std::string strval = stringify_value_file_style(val) ;
		if(cp->get_type() != TYP_NUL)
			fprintf(fp, "K%s", strval.c_str());

		if(cp->locked()) fprintf (fp, ";P");

		putc ('\n', fp);
	}
}

static std::string oleo_write_window_config ()
{
	std::ostringstream oss;

	//int n;
	char buf[90];
	//struct line scratch;
	//scratch.alloc = 0;
	//scratch.buf = 0;

	//cwin->win_curow = curow;
	//cwin->win_cucol = cucol;
	//sprint_line (&out, "O;status %d\n", user_status);
	oss << "O;status " << user_status << "\n";
		buf[0] = '\0';
		if (win_flags & WIN_LCK_HZ)
			strcat (buf, ",lockh");
		if (win_flags & WIN_LCK_VT)
			strcat (buf, ",lockv");
		if (win_flags & WIN_PAG_HZ)
			strcat (buf, ",pageh");
		if (win_flags & WIN_PAG_VT)
			strcat (buf, ",pagev");
		if (win_flags & WIN_EDGE_REV)
			strcat (buf, ",standout");
		if ((win_flags & WIN_EDGES) == 0)
			strcat (buf, ",noedges");

		oss << "W;N" << 1 << ";A" << curow << " " << cucol 
			<< ";C7 0 7;O" << buf+1 << "\n";

	return oss.str();
}




/* Modify this to write out *all* the options */
static void write_mp_options (FILE *fp)
{
	fprintf (fp, "O;%sauto;%sbackground;%sa0\n",
			Global->auto_recalc ? "" : "no",
			Global->bkgrnd_recalc ? "" : "no",
			Global->a0 ? "" : "no");
}

void oleo_write_file(FILE *fp)
{
	oleo_write_file(fp, nullptr);
}

void oleo_write_file(FILE *fp, struct rng *rng)
{
	assert(rng == nullptr); // mcarter 06-May-2018: insist on writing whole spreadsheet
	int old_a0;

	(void) fprintf (fp, "# This file was created by Neoleo\n");

	/* All versions of the oleo file format should have a 
	 * version cookie on the second line.
	 */
	(void) fprintf (fp, "# format 3.1 (requires Neoleo 16.0 or higher if bold is used)\n");

	//rng = &all_rng;

	(void) fprintf (fp, "F;D%s%c%u\n",
			oleo_fmt_to_str (default_fmt, default_prc),
			//jst_to_chr (default_jst),
			map_reverse(jst_map, default_jst),
			default_width);

	write_mp_options (fp);


	//old_a0 = Global->a0;
	//Global->a0 = 0;

	//span_find_t w_find = find_span(the_wids, rng->lc, rng->hc);
	//write_spans(fp, w_find, 'W');
	write_widths(fp);

	// 25/4 We no longer write the heights, because they are always 1
	//span_find_t h_find = find_span(the_hgts, rng->lr, rng->hr);
	//write_spans(fp, h_find, 'H');

	oleo_fp = fp;
	oleo_rng = rng;

	write_cells(fp);

	std::string str = oleo_write_window_config();
	fputs(str.c_str(), fp);
	(void) fprintf (fp, "E\n");
	//Global->a0 = old_a0;
}

int oleo_set_options( int set_opt, char *option)
{
	return -1;
}



