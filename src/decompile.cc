/*
 * $Id: decompile.c,v 1.15 2000/08/10 21:02:50 danny Exp $
 *
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
#include "lists.h"
#include "logging.h"
#include "utils.h"

using namespace std::string_literals;

const char * decomp(const CELLREF r, const CELLREF c, CELL *cell);

using CPTR = char*;

typedef struct pr_node
{
	int tightness;
	int len;
	std::string string;
} pr_node_t;


static pr_node_t* save_decomp = nullptr;
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


/* We decompile things with these wierd node-things.  It's ugly, but it works.
 */
/*
static struct pr_node *
n_allocXXX (int size, int tightness, const char *fmt, ...)
{
	struct pr_node *ret;
	va_list args;

	ret = (pr_node*) ck_malloc (sizeof (struct pr_node) + size + 1);
	ret->len = size;
	ret->tightness = tightness;
	va_start (args, fmt);
	vsprintf (ret->string, fmt, args);
	va_end (args);
	return ret;
}
*/
static struct pr_node *
n_alloc (int size, int tightness, const char *fmt, ...)
{
	char buffer[1000];
	va_list args;
	va_start (args, fmt);
	vsprintf (buffer, fmt, args);
	va_end (args);

	pr_node_t* ret = new pr_node_t;
	ret->string = buffer;
	ret->tightness = tightness;
	ret->len = strlen(buffer);
	log_debug("n_alloc:"s + ret->string);
	return ret;
}

//#define n_free(x)	ck_free(x)

void n_free(pr_node_t* p) { delete p; }

static pr_node_t* byte_decompile(unsigned char *expr);

/* This function is only called by byte_decompile, but I refactored it out
 * Everything is still monstrous, mind.
 * Note that this function call byte_decompile() - so there is recursion
 */
void decompile_comp(struct function*& f, struct pr_node*& newn,  
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
					log_debug("decomp:IF(_L)");
					if (c_node[0]->tightness <= 1)
						newn = n_alloc (8 + c_node[0]->len + c_node[1]->len + c_node[2]->len,
								1,
								"(%s) ? %s : %s", c_node[0]->string, c_node[1]->string, c_node[2]->string);
					else
						newn = n_alloc (6 + c_node[0]->len + c_node[1]->len + c_node[2]->len,
								1,
								"%s ? %s : %s", c_node[0]->string, c_node[1]->string, c_node[2]->string);
				}
				else
					newn = n_alloc (6 + c_node[0]->len + c_node[1]->len + c_node[2]->len + strlen (f->fn_str),
							1000,
							F3, f->fn_str, c_node[0]->string, c_node[1]->string, c_node[2]->string);
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
				newn = n_alloc (strlen (tmp_str) + 1, 1000, "%s", str.c_str());
				log_debug("decomp:C_STR:"s + newn->string);
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
					newn = n_alloc (30, 1000, f->fn_str, col_to_str (col), row);
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
					newn = n_alloc (30, 1000, str, num1, num2);
				}
				newn->len = strlen (newn->string);
			}
			log_debug("decomp:C_CELL:"s + newn->string);
			break;

		case C_RANGE:
			{
				char tmprbuf[40];
				char tmpcbuf[40];
				struct rng rng;

				GET_RNG (expr, &rng);
				expr += EXP_ADD_RNG;

				if (Global->a0)
					newn = n_alloc (40, 1000, f->fn_str, col_to_str (rng.lc), rng.lr, col_to_str (rng.hc), rng.hr);
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

					newn = n_alloc (40, 1000, "%s%s", tmprbuf, tmpcbuf);
				}
				newn->len = strlen (newn->string);
			}
			break;

		case C_CONST:
			newn = n_alloc (strlen (f->fn_str) + 1, 1000, f->fn_str);
			break;

		case C_FN0:
		case C_FN0X:
		case C_FN0 | C_T:
			newn = n_alloc (strlen (f->fn_str) + 3, 1000, F0, f->fn_str);
			log_debug("decomp:C_FN?:"s + newn->string);
			break;

		case C_FN1:
			--c_node;
			newn = n_alloc (c_node[0]->len + strlen (f->fn_str) + 3,
					1000,
					F1, f->fn_str, c_node[0]->string.c_str());
			n_free (*c_node);
			break;

		case C_UNA:
			--c_node;
			if (c_node[0]->tightness < 9)
			{
				newn = n_alloc (3 + c_node[0]->len,
						9,
						"%s(%s)", f->fn_str, c_node[0]->string.c_str());
			}
			else
			{
				newn = n_alloc (1 + c_node[0]->len,
						9,
						"%s%s", f->fn_str, c_node[0]->string.c_str());
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
					newn = n_alloc (7 + c_node[0]->len + c_node[1]->len,
							pri,
							"(%s) %s (%s)", c_node[0]->string.c_str(), chr, c_node[1]->string.c_str());
				else
					newn = n_alloc (5 + c_node[0]->len + c_node[1]->len,
							pri,
							"(%s) %s %s", c_node[0]->string.c_str(), chr, c_node[1]->string.c_str());
			}
			else if (c_node[1]->tightness < pri || (c_node[1]->tightness == pri && aso != -1))
				newn = n_alloc (5 + c_node[0]->len + c_node[1]->len,
						pri,
						"%s %s (%s)", c_node[0]->string.c_str(), chr, c_node[1]->string.c_str());
			else
				newn = n_alloc (3 + c_node[0]->len + c_node[1]->len,
						pri,
						"%s %s %s", c_node[0]->string, chr, c_node[1]->string);

			n_free (c_node[0]);
			n_free (c_node[1]);
			break;

		case C_FN2:
do_fn2:
			c_node -= 2;
			newn = n_alloc (c_node[0]->len + c_node[1]->len + strlen (f->fn_str) + 5,
					1000,
					F2, f->fn_str, c_node[0]->string.c_str(), c_node[1]->string.c_str());
			n_free (c_node[0]);
			n_free (c_node[1]);
			break;

		case C_FN3:
			c_node -= 3;
			newn = n_alloc (c_node[0]->len + c_node[1]->len + c_node[2]->len + strlen (f->fn_str) + 7,
					1000,
					F3,
					f->fn_str,
					c_node[0]->string.c_str(),
					c_node[1]->string.c_str(),
					c_node[2]->string.c_str());
			n_free (c_node[0]);
			n_free (c_node[1]);
			n_free (c_node[2]);
			log_debug("decomp:C_FN3:"s + newn->string);
			break;

		case C_FNN:
			aso = *expr++;
			c_node -= aso;

			if (aso == 1)
				newn = n_alloc (3 + c_node[0]->len + strlen (f->fn_str),
						1000,
						F1, f->fn_str, c_node[0]->string.c_str());
			else
			{
				newn = n_alloc (2 + c_node[0]->len + strlen (f->fn_str),
						1000,
						FN1, f->fn_str, c_node[0]->string);
				--aso;
				for (pri = 1; pri < aso; pri++)
				{
					n_free (c_node[0]);
					c_node[0] = newn;
					newn = n_alloc (2 + newn->len + c_node[pri]->len,
							1000,
							"%s, %s", newn->string.c_str(), c_node[pri]->string.c_str());
				}
				n_free (c_node[0]);
				c_node[0] = newn;
				newn = n_alloc (3 + newn->len + c_node[aso]->len,
						1000,
						"%s, %s)", newn->string.c_str(), c_node[aso]->string.c_str());
			}
			n_free (c_node[0]);
			break;

		case C_FN4:
			c_node -= 4;
			newn = n_alloc (c_node[0]->len + c_node[1]->len + c_node[2]->len + c_node[3]->len + strlen (f->fn_str) + 6,
					1000,
					F4,
					f->fn_str,
					c_node[0]->string.c_str(),
					c_node[1]->string.c_str(),
					c_node[2]->string.c_str(),
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
				newn = n_alloc (strlen (tmp_str) + 1, 1000, "%s", str.c_str());
			}
			break;

		case C_FLT:
			bcopy ((VOIDSTAR) expr, (VOIDSTAR) & tmp_flt, sizeof (num_t));
			expr += sizeof (num_t);
			newn = n_alloc (20, 1000, f->fn_str, (double) tmp_flt); 
			newn->len = strlen (newn->string);
			log_debug("decomp:C_FLT:"s + newn->string);
			break;

		case C_INT:
			bcopy ((VOIDSTAR) expr, (VOIDSTAR) & tmp_lng, sizeof (long));
			expr += sizeof (long);
			newn = n_alloc (20, 1000, f->fn_str, tmp_lng);
			newn->len = strlen (newn->string);
			log_debug("decomp:C_INT:"s + newn->string);
			break;

		case C_VAR:
			bcopy ((VOIDSTAR) expr, (VOIDSTAR) & v, sizeof (struct var *));
			expr += sizeof (struct var *);
			newn = n_alloc (strlen (v->var_name) + 1,
					1000,
					f->fn_str, v->var_name);
			break;


		default:
			panic ("Bad decompile %d", f->fn_comptype);
	}
}
static pr_node_t*
byte_decompile ( unsigned char *expr)
{
	static struct pr_node **c_node;
	static int line_alloc;
	static pr_node_t** the_line;
	if (!the_line)
	{
		//the_line = (struct pr_node **) ck_malloc (20 * sizeof (struct pr_node *));
		the_line = new pr_node_t*[20];
		line_alloc = 20;
		c_node = the_line;
	}

	struct pr_node *newn = 0;
	while(*expr) {
		unsigned char byte = *expr++;
		struct function *f;
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

/* Actual entry points to this file */
/* decomp(row, col, cell) returns a string that can be byte_compiled to create
   cell->formula  decomp_free() frees up the allocated string */
/* Have moved decomp(row, col, cell) to decomp_formula(row, col, cell, tog).
 * Alias decomp(row, col, cell) behaves exactly as command described
 * in the comment aboce.  the new int tog argument, if true, can be
 * used to turn on formatted editing.
 */

std::string
decomp_str(const CELLREF r, const CELLREF c)
{

	std::string res;
	save_decomp = nullptr;
	CELL *cp = find_cell(r, c);
	if(cp != nullptr) {
		const char *tmp = decomp(r, c, cp);
		if(tmp) res = std::string(tmp);
	}
	decomp_free();
	return res;
}

const char *
decomp(const CELLREF r, const CELLREF c, CELL *cell)
{
	const char *tmp = decomp_formula (r, c, cell, 0);
	return tmp;
}

	
	
	const char *
decomp_formula (const CELLREF r, const CELLREF c, CELL *cell, int tog)
{
	char *str;
	extern char *bname[];

	if (!cell)
	{
		str = (CPTR) ck_malloc (1);
		str[0] = '\0';
		save_decomp =  new pr_node_t;
		return str;
	}
	decomp_row = r;
	decomp_col = c;
	if (cell->get_cell_formula() == 0)
	{
		switch (GET_TYP (cell))
		{
			case 0:
				str = (CPTR) ck_malloc (1);
				str[0] = '\0';
				break;
			case TYP_FLT:
				if (tog)
				{
					str = strdup (flt_to_str_fmt(cell));
				}
				else
				{
					str = strdup (flt_to_str (cell->cell_flt()));
				}
				log_debug("decomp_formula:TYP_FLT:"s + str);
				break;
			case TYP_INT:
				str = (CPTR) ck_malloc (20);
				sprintf (str, "%ld", cell->cell_int());
				break;
			case TYP_STR:
				str = strdup (backslash_a_string (cell->cell_str(), 1));
				break;
			case TYP_BOL:
				str = strdup (bname[cell->cell_bol()]);
				break;
			case TYP_ERR:
				str = strdup (ename[cell->cell_bol()]);
				break;
			default:
				str = 0;
				panic ("Unknown type %d in decomp", GET_TYP (cell));
		}
		//save_decomp = new pr_node_t;
		//save_decomp->string = str;
		return str;
	} else {
		pr_node_t  *ret = byte_decompile (cell->get_cell_formula());
		//save_decomp = ret;
		//save_decomp = new pr_node_t
		save_decomp = ret; // TODO this recasting seems dubious
		return &(ret->string[0]);
		//return ret->string.c_str();
	}
}

void
decomp_free (void)
{
	if(save_decomp) delete save_decomp;
	save_decomp = nullptr;
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
}

