/*
 * Copyright © 1990, 1992, 1993, 1999 Free Software Foundation, Inc.
 * 
 * This file is part of Oleo, the GNU Spreadsheet.
 * 
 * Oleo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * Oleo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Oleo; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <cstring>
#include <stdarg.h>
#include <ctype.h>
#include "global.h"
#include "decompile.h"
#include "eval.h"
#include "cell.h"
#include "io-utils.h"
#include "cmd.h"
#include "mem.h"
#include "sheet.h"
#include "logging.h"
#include "utils.h"

using namespace std::string_literals;

//std::string decomp(const CELLREF r, const CELLREF c, CELL *cell);

using CPTR = char*;

typedef struct pr_node
{
	int tightness;
	std::string string;
} pr_node_t;


static pr_node_t save_decomp;
static CELLREF decomp_row;
static CELLREF decomp_col;

/* These #defines are so that we don't have to duplicate code below */
/* JF: now obsolete, and should be trashed. */

#define F0	Global->a0?(CPTR)"@%s()":(CPTR)"%s()"
#define F1	Global->a0?(CPTR)"@%s(%s)":(CPTR)"%s(%s)"
#define F2	Global->a0?(CPTR)"@%s(%s, %s)":(CPTR)"%s(%s, %s)"
#define F3	Global->a0?(CPTR)"@%s(%s, %s, %s)":(CPTR)"%s(%s, %s, %s)"
#define F4	Global->a0?(CPTR)"@%s(%s, %s, %s, %s)":(CPTR)"%s(%s, %s, %s, %s)"
#define FN1	Global->a0?(CPTR)"@%s(%s":(CPTR)"%s(%s"


static void log_debug_1(std::string s)
{
       	if constexpr(true) log_debug(s); 
}



/* We decompile things with these wierd node-things.  It's ugly, but it works.
 */

static struct pr_node *
n_alloc (int tightness, const char *fmt, ...)
{
	char buffer[1000];
	va_list args;
	va_start (args, fmt);
	vsprintf (buffer, fmt, args);
	va_end (args);

	pr_node_t* ret = new pr_node_t;
	ret->tightness = tightness;
	ret->string = buffer;
	//log_debug_1("n_alloc:"s + ret->string);
	return ret;
}

//#define n_free(x)	ck_free(x)

void n_free(pr_node_t* p) { delete p; }

static pr_node_t* byte_decompile(unsigned char *expr);

/* This function is only called by byte_decompile, but I refactored it out
 * Everything is still monstrous, mind.
 * Note that this function call byte_decompile() - so there is recursion
 */
void decompile_comp(function_t*& f, struct pr_node*& newn,  
		unsigned char* &expr,   
		unsigned jumpto, unsigned char &byte, 
		struct pr_node **&c_node)
{
	num_t tmp_flt;
	long tmp_lng;
	const std::string tmp_str;
	struct var *v;
	unsigned char save_val;

	int pri;
	int aso;
	const char *chr;
	switch (GET_COMP (f->fn_comptype)) {
		case C_IF:
			{
				unsigned long_skp = 0;
				if (expr[jumpto - 2] != SKIP)
				{
					long_skp = 1;
					save_val = expr[jumpto - 3];
					expr[jumpto - 3] = 0;
				}
				else
				{
					long_skp = 0;
					save_val = expr[jumpto - 2];
					expr[jumpto - 2] = 0;
				}
				c_node[0] = byte_decompile (expr);
				c_node++;

				if (long_skp)
				{
					expr[jumpto - 3] = save_val;
					expr += jumpto;
					jumpto = expr[-2] + ((unsigned) (expr[-1]) << 8);
				}
				else
				{
					expr[jumpto - 2] = save_val;
					expr += jumpto;
					jumpto = expr[-1];
				}
				save_val = expr[jumpto];
				expr[jumpto] = 0;
				c_node[0] = byte_decompile (expr);
				c_node -= 2;
				expr[jumpto] = save_val;
				expr += jumpto;
				if (byte == IF || byte == IF_L)
				{
					//log_debug_1("decomp:IF(_L)");
					const char* fmt;
					if (c_node[0]->tightness <= 1)
						fmt = "(%s) ? %s : %s";
					else
						fmt = "%s ? %s : %s";
					newn = n_alloc(1, fmt, c_node[0]->string.c_str(), 
							c_node[1]->string.c_str(), c_node[2]->string.c_str());
				}
				else
					newn = n_alloc(1000, F3, f->fn_str, 
							c_node[0]->string.c_str(), c_node[1]->string.c_str(), 
							c_node[2]->string.c_str());
				n_free (c_node[0]);
				n_free (c_node[1]);
				n_free (c_node[2]);
			}
			break;

		case C_ANDOR:
			save_val = expr[jumpto];
			expr[jumpto] = 0;
			c_node[0] = byte_decompile (expr);
			expr[jumpto] = save_val;
			expr += jumpto;
			c_node++;
			if (ISINFIX (f->fn_comptype))
			{
				pri = GET_IN (f->fn_comptype);
				aso = GET_ASO (f->fn_comptype);
				chr = f->fn_str;
				goto do_infix;
			}
			else
				goto do_fn2;

		case C_STR:
			{
				std::string str = std::string(backslash_a_string ((char *) expr + jumpto, 2));
				newn = n_alloc(1000, "%s", str.c_str());
				//log_debug_1("decomp:C_STR:"s + newn->string);
			}
			break;

		case C_CELL:
			{
				int num1, num2;
				const char *str;
				CELLREF row, col;

				row = GET_ROW (expr);
				col = GET_COL (expr);
				expr += EXP_ADD;

				if (Global->a0)
				{
					newn = n_alloc(1000, f->fn_str, col_to_str (col), row);
				}
				else
				{
					if (byte & ROWREL)
					{
						num1 = row - decomp_row;
						if (byte & COLREL)
						{
							num2 = col - decomp_col;
							if (row == decomp_row && col == decomp_col)
								str = "rc";
							else if (row == decomp_row)
							{
								str = "rc[%+d]";
								num1 = num2;
							}
							else if (col == decomp_col)
								str = "r[%+d]c";
							else
								str = "r[%+d]c[%+d]";
						}
						else if (row == decomp_row)
						{
							str = "rc%u";
							num1 = num2 = col;
						}
						else
						{
							str = "r[%+d]c%u";
							num2 = col;
						}
					}
					else if (byte & COLREL)
					{
						num1 = row;
						num2 = col - decomp_col;
						if (col == decomp_col)
							str = "r%uc";
						else
							str = "r%uc[%+d]";
					}
					else
					{
						str = "r%uc%u";
						num1 = row;
						num2 = col;
					}
					newn = n_alloc(1000, str, num1, num2);
				}
			}
			//log_debug_1("decomp:C_CELL:"s + newn->string);
			break;

		case C_RANGE:
			{
				char tmprbuf[40];
				char tmpcbuf[40];
				struct rng rng;

				GET_RNG (expr, &rng);
				expr += EXP_ADD_RNG;

				if (Global->a0)
					newn = n_alloc(1000, f->fn_str, col_to_str (rng.lc), rng.lr, col_to_str (rng.hc), rng.hr);
				else
				{
					/* Check for special cases */
					if (rng.lr == rng.hr && ((byte & LRREL) ? 1 : 0) == ((byte & HRREL) ? 1 : 0))
					{
						if (byte & LRREL)
						{
							if (rng.lr == decomp_row)
							{
								tmprbuf[0] = 'r';
								tmprbuf[1] = '\0';
							}
							else
								(void) sprintf (tmprbuf, "r[%+d]", rng.lr - decomp_row);
						}
						else
							sprintf (tmprbuf, "r%u", rng.lr);
					}
					else if ((byte & LRREL) && (byte & HRREL))
					{
						int r1, r2, rtmp;

						r1 = rng.lr - decomp_row;
						r2 = rng.hr - decomp_row;
						if (r1 < r2)
							rtmp = r1, r1 = r2, r2 = rtmp;
						(void) sprintf (tmprbuf, "r[%+d:%+d]", r1, r2);
					}
					else if ((byte & LRREL))
						(void) sprintf (tmprbuf, "r[%+d]:%u", rng.lr - decomp_row, rng.hr);
					else if (byte & HRREL)
						(void) sprintf (tmprbuf, "r%u:[%+d]", rng.lr, rng.hr - decomp_row);
					else if (rng.lr < rng.hr)
						(void) sprintf (tmprbuf, "r%u:%u", rng.lr, rng.hr);
					else
						(void) sprintf (tmprbuf, "r%u:%u", rng.hr, rng.lr);

					if (rng.lc == rng.hc && ((byte & LCREL) ? 1 : 0) == ((byte & HCREL) ? 1 : 0))
					{
						if (byte & LCREL)
						{
							if (rng.lc == decomp_col)
							{
								tmpcbuf[0] = 'c';
								tmpcbuf[1] = '\0';
							}
							else
								sprintf (tmpcbuf, "c[%+d]", rng.lc - decomp_col);
						}
						else
							sprintf (tmpcbuf, "c%u", rng.lc);
					}
					else if ((byte & LCREL) && (byte & HCREL))
					{
						int c1, c2, ctmp;

						c1 = rng.lc - decomp_col;
						c2 = rng.hc - decomp_col;
						if (c1 < c2)
							ctmp = c1, c1 = c2, c2 = ctmp;
						(void) sprintf (tmpcbuf, "c[%+d:%+d]", c1, c2);
					}
					else if ((byte & LCREL))
						(void) sprintf (tmpcbuf, "c[%+d]:%u", rng.lc - decomp_col, rng.hc);
					else if (byte & HCREL)
						(void) sprintf (tmpcbuf, "c%u:[%+d]", rng.lc, rng.hc - decomp_col);
					else if (rng.lc < rng.hc)
						(void) sprintf (tmpcbuf, "c%u:%u", rng.lc, rng.hc);
					else
						(void) sprintf (tmpcbuf, "c%u:%u", rng.hc, rng.lc);

					newn = n_alloc(1000, "%s%s", tmprbuf, tmpcbuf);
				}
			}
			break;

		case C_CONST:
			newn = n_alloc(1000, f->fn_str);
			break;

		case C_FN0:
		case C_FN0X:
		case C_FN0 | C_T:
			newn = n_alloc(1000, F0, f->fn_str);
			//log_debug_1("decomp:C_FN?:"s + newn->string);
			break;

		case C_FN1:
			--c_node;
			newn = n_alloc(1000, F1, f->fn_str, c_node[0]->string.c_str());
			n_free (*c_node);
			break;

		case C_UNA:
			--c_node;
			if (c_node[0]->tightness < 9)
			{
				newn = n_alloc(9, "%s(%s)", f->fn_str, c_node[0]->string.c_str());
			}
			else
			{
				newn = n_alloc(9, "%s%s", f->fn_str, c_node[0]->string.c_str());
			}
			n_free (*c_node);
			break;

		case C_INF:
			pri = GET_IN (f->fn_comptype);
			aso = GET_ASO (f->fn_comptype);
			chr = f->fn_str;

do_infix:
			c_node -= 2;
			if (c_node[0]->tightness < pri || (c_node[0]->tightness == pri && aso != 1))
			{
				if (c_node[1]->tightness < pri || (c_node[1]->tightness == pri && aso != -1))
					newn = n_alloc(pri, "(%s) %s (%s)", c_node[0]->string.c_str(), chr, 
							c_node[1]->string.c_str());
				else
					newn = n_alloc( pri, "(%s) %s %s", c_node[0]->string.c_str(), 
							chr, c_node[1]->string.c_str());
			}
			else if (c_node[1]->tightness < pri || (c_node[1]->tightness == pri && aso != -1))
				newn = n_alloc(pri, "%s %s (%s)", c_node[0]->string.c_str(), chr, c_node[1]->string.c_str());
			else
				newn = n_alloc(pri, "%s %s %s", c_node[0]->string.c_str(), chr, c_node[1]->string.c_str());

			n_free (c_node[0]);
			n_free (c_node[1]);
			//log_debug_1("decomp:C_INF:"s + newn->string);
			break;

		case C_FN2:
do_fn2:
			c_node -= 2;
			newn = n_alloc(1000, F2, f->fn_str, c_node[0]->string.c_str(), c_node[1]->string.c_str());
			n_free (c_node[0]);
			n_free (c_node[1]);
			break;

		case C_FN3:
			c_node -= 3;
			newn = n_alloc( 1000, F3, f->fn_str, c_node[0]->string.c_str(), 
					c_node[1]->string.c_str(), c_node[2]->string.c_str());
			n_free (c_node[0]);
			n_free (c_node[1]);
			n_free (c_node[2]);
			//log_debug_1("decomp:C_FN3:"s + newn->string);
			break;

		case C_FNN:
			aso = *expr++;
			c_node -= aso;

			if (aso == 1)
				newn = n_alloc(1000, F1, f->fn_str, c_node[0]->string.c_str());
			else
			{
				newn = n_alloc(1000, FN1, f->fn_str, c_node[0]->string);
				--aso;
				for (pri = 1; pri < aso; pri++)
				{
					n_free (c_node[0]);
					c_node[0] = newn;
					newn = n_alloc(1000, "%s, %s", newn->string.c_str(), c_node[pri]->string.c_str());
				}
				n_free (c_node[0]);
				c_node[0] = newn;
				newn = n_alloc(1000, "%s, %s)", newn->string.c_str(), c_node[aso]->string.c_str());
			}
			n_free (c_node[0]);
			break;

		case C_FN4:
			c_node -= 4;
			newn = n_alloc(1000, F4, f->fn_str, c_node[0]->string.c_str(), 
					c_node[1]->string.c_str(), c_node[2]->string.c_str(),
					c_node[3]->string.c_str());
			n_free (c_node[0]);
			n_free (c_node[1]);
			n_free (c_node[2]);
			n_free (c_node[3]);
			break;

		case C_ERR:
			{
				std::string str = (char *) expr + jumpto;
				expr++;
				newn = n_alloc(1000, "%s", str.c_str());
			}
			break;

		case C_FLT:
			bcopy ((VOIDSTAR) expr, (VOIDSTAR) & tmp_flt, sizeof (num_t));
			expr += sizeof (num_t);
			newn = n_alloc(1000, f->fn_str, (double) tmp_flt); 
			//log_debug_1("decomp:C_FLT:"s + newn->string);
			break;

		case C_INT:
			bcopy ((VOIDSTAR) expr, (VOIDSTAR) & tmp_lng, sizeof (long));
			expr += sizeof (long);
			newn = n_alloc(1000, f->fn_str, tmp_lng);
			//log_debug_1("decomp:C_INT:"s + newn->string);
			break;

		case C_VAR:
			bcopy ((VOIDSTAR) expr, (VOIDSTAR) & v, sizeof (struct var *));
			expr += sizeof (struct var *);
			newn = n_alloc(1000, f->fn_str, v->var_name.c_str());
			break;


		default:
			panic ("Bad decompile %d", f->fn_comptype);
	}
}

static pr_node_t*
byte_decompile (formula_t expr)
{
	static struct pr_node **c_node;
	static int line_alloc;
	static pr_node_t** the_line;
	if (!the_line)
	{
		the_line = new pr_node_t*[20];
		line_alloc = 20;
		c_node = the_line;
	}

	struct pr_node *newn = 0;
	while(*expr) {
		unsigned char byte = *expr++;
		function_t *f;
		long tmp_lng;
		if (byte < USR1)
			f = &the_funs[byte];
		else if (byte < SKIP)
		{
			tmp_lng = *expr++;
			f = &usr_funs[byte - USR1][tmp_lng];
		}
		else
			f = &skip_funs[byte - SKIP];

		{
			unsigned jumpto = 0;
			if (f->fn_argn & X_J)
				jumpto = *expr++;
			else if (f->fn_argn & X_JL)
			{
				jumpto = expr[0] + ((unsigned) (expr[1]) << 8);
				expr += 2;
			}
			else
				jumpto = 0;


			decompile_comp(f, newn, expr, jumpto, byte, c_node);
		}


		*c_node++ = newn;
		if (c_node == &the_line[line_alloc])
		{
			assert(false);
			line_alloc *= 2;
			the_line = (pr_node**) ck_realloc (the_line, line_alloc * sizeof (struct pr_node *));
			c_node = &the_line[line_alloc / 2];
		}

	}


	newn = *--c_node;
	return newn;
}


std::string
decomp(const CELLREF r, const CELLREF c, CELL *cell)
{
	return decomp_formula (r, c, cell, 0);
}

std::string
decomp_str(const CELLREF r, const CELLREF c)
{
	std::string res;
	CELL *cp = find_cell(r, c);
	if(cp == nullptr)
		return res;
	return decomp(r, c, cp);
}




const std::string
decomp_formula_1(const CELLREF r, const CELLREF c, CELL *cell, int tog)
{
	extern char *bname[];
	switch (GET_TYP (cell)) {
		case 0:
			return "";
		case TYP_FLT:
			if (tog) return flt_to_str_fmt(cell);
			return flt_to_str (cell->gFlt());
		case TYP_INT:
			return std::to_string(cell->gInt());
		case TYP_STR:
			{
				strcpy_c s{cell->gString()};
				return backslash_a_string (s.data(), 1);
			}
		case TYP_BOL:
			return bname[cell->gBol()];
		case TYP_ERR:
			return ename[cell->gBol()];
		default:
			panic ("Unknown type %d in decomp", GET_TYP (cell));
			return "PANIC";
	}
}

std::string
decomp_formula(const CELLREF r, const CELLREF c, CELL *cell, int tog)
{
	if(!cell) return "";
	decomp_row = r;
	decomp_col = c;

	// we don't care about 'tightness', as all the computation has been done
	if (cell->get_cell_formula() == 0) {
		save_decomp.string = decomp_formula_1(r, c, cell, tog);
	} else {
		pr_node_t* n = byte_decompile (cell->get_cell_formula());
		save_decomp.string = n->string;
		delete n;
	}
	//log_debug_1("decomp_formula:return:"s + save_decomp.string);
	return save_decomp.string.c_str();
}


/*
 * This takes a string and returns a backslashed form suitable for printing.
 * Iff add_quote is true, it'll add "s at the beginning and end.
 * Note that this returns a pointer to a static area that is overwritten with
 * each call. . .
 *
 * If add_quote is 1, escape normal quotes, if it is 2, also escape %.
 */
const char *
backslash_a_string (char *string, int add_quote)
{
// TODO causes crashing at 19-Jan-2019
#if 0	
	return string;
#else
	char		*pf, *pt;
	unsigned char	ch;
	int		size, len;
	static char	*cbuf;
	static int	s_cbuf;

#define ALLOC_PT()				\
	len=strlen(pf);				\
	size=pf-string;				\
	if(s_cbuf<3+size+4*len) {		\
		s_cbuf=3+size+4*len;		\
		cbuf= (cbuf) ? (CPTR) ck_realloc(cbuf,s_cbuf) : (CPTR) ck_malloc(s_cbuf); \
	}					\
	if(size)				\
	bcopy(string,cbuf,size);	\
	pt=cbuf+size;				\


	pt = 0;
	pf = string;
	if (add_quote) {
		ALLOC_PT ()
			*pt++ = '"';
	}
	for (; *pf; pf++) {
		ch = *pf;
		if (isprint(ch) && ch != '\\'
				&& (ch != '%' || !(add_quote == 2))
				&& (ch != '"' || !add_quote)) {
			if (pt)
				*pt++ = ch;
			continue;
		}

		if (!pt) {
			ALLOC_PT ()
		}
		if (ch == '\\') {
			*pt++ = '\\';
			*pt++ = '\\';
		} else if (ch == '"') {
			*pt++ = '\\';
			*pt++ = ch;
		} else if (ch == '%') {
			*pt++ = '%';
			*pt++ = '%';
		} else {
			*pt++ = '\\';
			*pt++ = ((ch >> 6) & 0x3) + '0';
			*pt++ = ((ch >> 3) & 0x7) + '0';
			*pt++ = (ch & 0x7) + '0';
		}
	}
	if (add_quote)
		*pt++ = '"';
	if (pt) {
		*pt++ = '\0';
		return cbuf;
	}
	return string;
#endif
}

