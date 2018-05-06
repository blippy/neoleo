/*
 * $Header: /cvs/oleo/src/eval.c,v 1.12 2001/02/13 23:38:05 danny Exp $
 *
 * Copyright © 1990, 1992, 1993, 2001 Free Software Foundation, Inc.
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

#include <ctype.h>
#include <cmath>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "funcdef.h"

constexpr auto pi = std::acos(-1);
#define obstack_chunk_alloc ck_malloc
#define obstack_chunk_free free
#include "obstack.h"

#include "global.h"
#include "cell.h"
#include "eval.h"
#include "errors.h"
#include "io-utils.h"
#include "sheet.h"

#if defined(HAVE_RINT)
extern double rint (double);
extern long random (void);
#else
#define rint(x) (((x)<0) ? ceil((x)-.5) : floor((x)+.5))
#endif

extern int n_usr_funs;

//double to_int ();
//static int deal_area ( unsigned char cmd, unsigned char num_args, struct value *p);
//static void add_int (long value);
//static void add_flt (double value);
RETSIGTYPE math_sig ( int sig);

int fls (long);

#define Float	x.c_n
#define String	x.c_s
#define Int	x.c_l
#define Value	x.c_i
#define Rng	x.c_r

static struct value *stack;
static int stackmax;
static int curstack;

unsigned short current_cycle;

CELLREF cur_row;
CELLREF cur_col;

num_t rintn(num_t n)
{
	double d1 = n;
	double d2 = rint(d1);
	num_t res = d2;
	return res;
}

static num_t exp10_arr[] =
{
  1E0, 1E1, 1E2, 1E3, 1E4,
  1E5, 1E6, 1E7, 1E8, 1E9,
  1E10, 1E11, 1E12, 1E13, 1E14,
  1E15, 1E16, 1E17, 1E18, 1E19,
  1E20, 1E21, 1E22, 1E23, 1E24,
  1E25, 1E26, 1E27, 1E28, 1E29
};

double ftmp;
long itmp;
int overflow;

/* You may ask:  Why not jsut put the value in stack[0] and goto break_out
   The answer is that ERROR is a valid input type for several operators, so
   we want to work if we're feeding an error into one of these operators. . .
 */
#define ERROR(cause)		\
	{			\
		p->type=TYP_ERR;\
		p->Value=cause; \
		goto next_byte; \
	}

#define ERROR1(cause) { throw cause;}
void TO_FLT(struct value* val)
{	
	if((val)->type==TYP_FLT) 
		; 
	else if((val)->type==TYP_INT) { 
		(val)->type=TYP_FLT; 
		(val)->Float=(double)(val)->Int; 
	} else if((val)->type==TYP_STR) { 
		(val)->type=TYP_FLT; 
		char* strptr=(val)->String; 
		(val)->Float=astof(&strptr); 
		if(*strptr) 
			ERROR1(NON_NUMBER); 
	} else if((val)->type==TYP_ERR) {
		ERROR1((val)->Value); 
	} else if((val)->type==0) { 
		(val)->type=TYP_FLT; 
		(val)->Float=0.0; 
	} else 
		ERROR1(NON_NUMBER);
}

void  TO_INT(struct value* val)	
{	
	if((val)->type==TYP_INT) 
		; 
	else if((val)->type==TYP_FLT) { 
		(val)->type=TYP_INT; 
		(val)->Int=(long)(val)->Float; 
	} else if((val)->type==TYP_STR) { 
		(val)->type=TYP_INT; 
		char* strptr=(val)->String; 
		(val)->Int=astol(&strptr); 
		if(*strptr) 
			ERROR1(NON_NUMBER); 
	} else if((val)->type==TYP_ERR) {
		ERROR1((val)->Value); 
	} else if((val)->type==0) { 
		(val)->type=TYP_INT; 
		(val)->Int=0; 
	} else 
		ERROR1(NON_NUMBER);
}

void TO_NUM(struct value* val)
{
	if((val)->type==TYP_INT || (val)->type==TYP_FLT) 
		; 
	else if((val)->type==TYP_STR) { 
		(val)->type=TYP_FLT; 
		char* strptr=(val)->String; 
		(val)->Float=astof(&strptr); 
		if(*strptr) 
			ERROR1(NON_NUMBER); 
	} else if((val)->type==TYP_ERR) {
		ERROR1((val)->Value); 
	} else if((val)->type==0) { 
		(val)->type=TYP_INT; 
		(val)->Int=0; 
	} else 
		ERROR1(NON_NUMBER);
}

#define TO_STR(val)	\
	if((val)->type==TYP_STR)	\
		;	\
	else if((val)->type==TYP_INT) {	\
		char *s;	\
		(val)->type=TYP_STR;	\
		s=(char*) obstack_alloc(&tmp_mem,30); \
		sprintf(s,"%ld",(val)->Int); \
		(val)->String=s;	\
	} else if((val)->type==TYP_FLT) {		\
		char *s;				\
		s=flt_to_str((val)->Float);		\
		(void)obstack_grow(&tmp_mem,s,strlen(s)+1); \
		(val)->String=(char*) obstack_finish(&tmp_mem);	\
		(val)->type=TYP_STR;			\
	} else if((val)->type==TYP_ERR) {		\
		ERROR1((val)->Value);	\
	} else if((val)->type==0) {	\
		(val)->type=TYP_STR;	\
		(val)->String=(char*) obstack_alloc(&tmp_mem,1); \
		(val)->String[0]='\0'; \
	} else \
		ERROR1(NON_STRING);

#define TO_BOL(val)	\
	if((val)->type==TYP_BOL)	\
		;	\
	else if((val)->type==TYP_ERR) {	\
		ERROR1((val)->Value);	\
	} else	\
		ERROR1(NON_BOOL);


#define TO_RNG(val) \
	if((val)->type==TYP_RNG) \
		; \
	else if((val)->type==TYP_ERR) {\
		ERROR1((val)->Value); \
	} else \
		ERROR1(NON_RANGE);


#define TO_ANY(val) \
	if((val)->type==TYP_RNG) \
		ERROR1(BAD_INPUT); 

#define PUSH_ANY(cp)				\
	if(!cp || !GET_TYP(cp)) {		\
		p->type=TYP_NUL;			\
		p->Int=0;			\
	} else {				\
		p->type=GET_TYP(cp);		\
		p->x=cp->get_c_z();			\
	}

// should probably use one of TO_NUM(), or something like that.
num_t as_flt(struct value* v)
{
	switch(v->get_type()) {
		case TYP_INT:
			return v->gInt();
		case TYP_FLT:
			return v->gFlt();
		default:
			assert(false); // TODO improve
	}
}

void do_math_binop(int op, struct value* p1, struct value* p2)
{
	num_t v1 = as_flt(p1);
	num_t v2 = as_flt(p2);
	num_t v3;

	switch(op) {
		case DIFF:
			v3 = v1 -v2;
			break;
		case DIV:
			v3 = v1 / v2;
			break;
		case MOD:
			v3 = (long) v1 % (long) v2;
			break;
		case PROD:
			v3 = v1 * v2;
			break;
		case SUM:
			v3 = v1 + v2;
			break;
		default:
			assert(false);
	}

       //	= std::plus<double>(v1, v2);

	ValType t = TYP_INT;
	if(std::nearbyint(v3) != v3) t = TYP_FLT;
	if(p1->get_type() == TYP_FLT || p2->get_type() == TYP_FLT) t = TYP_FLT;

	if(t == TYP_FLT) 
		p1->sFlt(v3);
	else
		p1->sInt(v3);

}

void fill_argument(char arg_type, struct value* p)
{
	char* strptr;
	switch (arg_type) {
		/* A is for anything */
		/* Any non-range value */
		case 'A':
			TO_ANY (p);
			break;
			/* B is for boolean */
		case 'B':
			TO_BOL (p);
			break;
			/* D is for Don't check */
		case 'D':
			break;
			/* E is for Everything */
		case 'E':
			break;
			/* F is for Float */
		case 'F':
			TO_FLT (p);
			break;
			/* I is for Int */
		case 'I':
			TO_INT (p);
			break;
			/* N is for Number (int or float) */
		case 'N':
			TO_NUM (p);
			break;
			/* R is for Range */
		case 'R':
			TO_RNG (p);
			break;
			/* S is for String */
		case 'S':
			TO_STR (p);
			break;
#ifdef TEST
		default:
			io_error_msg ("YIKE!  Unknown argtype for Fun %u  arg #%u", byte, xt);
			break;
#endif
	}
}
void
init_eval ()
{
	stackmax = 20;
	stack = (struct value *) ck_malloc (stackmax * sizeof (struct value));
	curstack = 0;
	current_cycle = 1;
	(void) signal (SIGFPE, math_sig);
}

/* This huge function takes a byte-compiled expression and executes it. */
struct value *
eval_expression ( unsigned char *expr)
{
	unsigned char byte;
	unsigned numarg;
	unsigned jumpto;
	struct function *f;
	struct value *p;
	char *strptr;
	int tmp;

	CELLREF lrow, hrow, crow;
	CELLREF lcol, hcol, ccol;

	cell* cell_ptr;

	if (!expr)
		return 0;
	jumpto = 0;
	numarg = 0;
	p = 0;
	curstack = 0;
	while ((byte = *expr++) != ENDCOMP)
	{
		if (byte < USR1)
			f = &the_funs[byte];
		else if (byte < SKIP)
		{
#ifdef TEST
			if (byte - USR1 >= n_usr_funs)
				panic ("Only have %d usr-function slots, but found byte for slot %d", n_usr_funs, 1 + byte - USR1);
#endif
			tmp = *expr++;
			f = &usr_funs[byte - USR1][tmp];
		}
		else
			f = &skip_funs[byte - SKIP];

		if (f->fn_argn & X_J)
			jumpto = *expr++;
		else if (f->fn_argn & X_JL)
		{
			jumpto = expr[0] + ((unsigned) (expr[1]) << 8);
			expr += 2;
		}

		switch (f->fn_argn & X_ARGS)
		{
			/* A0 is special, since it makes the stack grow, while
			   all the others make the stack the same size or
			   less. . . */
			case X_A0:
				numarg = 0;
				if (curstack == stackmax)
				{
					stackmax *= 2;
					stack = (struct value *) ck_realloc (stack, sizeof (struct value) * stackmax);
				}
				p = &stack[curstack];
				curstack++;
				break;

			case X_A1:
				numarg = 1;
				break;

			case X_A2:
				numarg = 2;
				break;
			case X_A3:
				numarg = 3;
				break;
			case X_A4:
				numarg = 4;
				break;
			case X_AN:
				numarg = *expr++;
				break;
			default:
				numarg = 0;
				p = 0;
#ifdef TEST
				panic ("Unknown arg_num %d", f->fn_argn);
#endif
		}
		if (numarg > 0)
		{
			int xt;

#ifdef TEST
			if (curstack < numarg)
				panic ("Only %u values on stack, not %u", curstack, numarg);
#endif
			p = &stack[curstack - numarg];
			curstack -= (numarg - 1);
			for (xt = 0; xt < numarg; xt++)
			{
				char arg_type =f->fn_argt[xt <= 3 ? xt : 3];
				try {
					fill_argument(arg_type, p+xt);
				} catch (int e) {
					//assert(false);
					//p->type = TYP_ERR;
					//p->Value = e;
					p->sErr(e);
					goto next_byte;
				}

			}
		}

		switch (byte)
		{
			case IF_L:
			case F_IF_L:
			case IF:
			case F_IF:
				if (p->type != TYP_BOL)
				{
					if (p->type != TYP_ERR)
					{
						p->type = TYP_ERR;
						p->Value = NON_BOOL;
					}
					expr += jumpto;
					if (expr[-2] != SKIP)
						jumpto = expr[-1] + (((unsigned) expr[-2]) << 8);
					else
						jumpto = expr[-1];
					expr += jumpto;	/* Skip both branches of the if */

				}
				else if (p->Value == 0)
				{
					expr += jumpto;
					--curstack;
				}
				else
					--curstack;
				break;

			case SKIP_L:
			case SKIP:
				--curstack;
				expr += jumpto;
				break;

			case AND_L:
			case AND:
				if (p->type == TYP_ERR)
					expr += jumpto;
				else if (p->type != TYP_BOL)
				{
					p->type = TYP_ERR;
					p->Value = NON_BOOL;
					expr += jumpto;
				}
				else if (p->Value == 0)
					expr += jumpto;
				else
					--curstack;
				break;

			case OR_L:
			case OR:
				if (p->type == TYP_ERR)
					expr += jumpto;
				else if (p->type != TYP_BOL)
				{
					p->type = TYP_ERR;
					p->Value = NON_BOOL;
					expr += jumpto;
				}
				else if (p->Value)
					expr += jumpto;
				else
					--curstack;
				break;

			case CONST_FLT:
				p->type = TYP_FLT;
				bcopy ((VOIDSTAR) expr, (VOIDSTAR) (&(p->Float)), sizeof (double));
				expr += sizeof (double);
				break;

			case CONST_INT:
				p->type = TYP_INT;
				bcopy ((VOIDSTAR) expr, (VOIDSTAR) (&(p->Int)), sizeof (long));
				expr += sizeof (long);
				break;

			case CONST_STR:
			case CONST_STR_L:
				p->type = TYP_STR;
				p->String = (char *) expr + jumpto;
				break;

			case CONST_ERR:
				p->type = TYP_ERR;
				p->Value = *expr++;
				/* expr+=sizeof(char *); */
				break;

			case CONST_INF:
			case CONST_NINF:
			case CONST_NAN:
				p->type = TYP_FLT;
				p->Float = (byte == CONST_INF) ? __plinf : ((byte == CONST_NINF) ? __neinf : NAN);
				break;

			case VAR:
				{
					struct var *varp;

					bcopy ((VOIDSTAR) expr, (VOIDSTAR) (&varp), sizeof (struct var *));
					expr += sizeof (struct var *);
					switch (varp->var_flags)
					{
						case VAR_UNDEF:
							p->type = TYP_ERR;
							p->Value = BAD_NAME;
							break;

						case VAR_CELL:
							cell_ptr = find_cell (varp->v_rng.lr, varp->v_rng.lc);
							PUSH_ANY (cell_ptr);
							break;

						case VAR_RANGE:
							if (varp->v_rng.lr == varp->v_rng.hr && varp->v_rng.lc == varp->v_rng.hc)
							{
								cell_ptr = find_cell (varp->v_rng.lr, varp->v_rng.lc);
								PUSH_ANY (cell_ptr);
							}
							else
							{
								p->type = TYP_RNG;
								p->Rng = varp->v_rng;
							}
							break;
#ifdef TEST
						default:
							panic ("Unknown var type %d", varp->var_flags);
#endif
					}
				}
				break;

				/* Cell refs */
			case R_CELL:
			case R_CELL | COLREL:
			case R_CELL | ROWREL:
			case R_CELL | ROWREL | COLREL:
				{
					CELLREF torow, tocol;

					torow = GET_ROW (expr);
					tocol = GET_COL (expr);
					expr += EXP_ADD;
					cell_ptr = find_cell ((CELLREF) torow, (CELLREF) tocol);
					PUSH_ANY (cell_ptr);
				}
				break;

			case RANGE:
			case RANGE | LRREL:
			case RANGE | LRREL | LCREL:
			case RANGE | LRREL | LCREL | HCREL:
			case RANGE | LRREL | HCREL:
			case RANGE | LRREL | HRREL:
			case RANGE | LRREL | HRREL | LCREL:
			case RANGE | LRREL | HRREL | LCREL | HCREL:
			case RANGE | LRREL | HRREL | HCREL:
			case RANGE | HRREL:
			case RANGE | HRREL | LCREL:
			case RANGE | HRREL | LCREL | HCREL:
			case RANGE | HRREL | HCREL:
			case RANGE | LCREL:
			case RANGE | LCREL | HCREL:
			case RANGE | HCREL:
				p->type = TYP_RNG;
				GET_RNG (expr, &(p->Rng));
				expr += EXP_ADD_RNG;
				break;

			case F_TRUE:
			case F_FALSE:
				p->type = TYP_BOL;
				p->Value = (byte == F_TRUE);
				break;

			case F_PI:
				p->type = TYP_FLT;
				p->Float = pi;
				break;

			case F_ROW:
			case F_COL:
				p->type = TYP_INT;
				p->Int = ((byte == F_ROW) ? cur_row : cur_col);
				break;

			case F_NOW:
				p->type = TYP_INT;
				p->Int = time (nullptr);
				break;

				/* Single operand instrs */
			case F_CEIL:
			case F_FLOOR:
				{
					double (*funp1) (double);
					funp1 = (double (*)(double)) (f->fn_fun);

					p->Float = (*funp1) (p->Float);
					if (p->Float != p->Float)
						ERROR (OUT_OF_RANGE);
				}
				break;

			case F_CTIME:
				p->type = TYP_STR;
				strptr = ctime ((time_t*) &p->Int);
				p->String = (char*) obstack_alloc (&tmp_mem, 25);
				strncpy (p->String, strptr, 24);
				p->String[24] = '\0';
				break;

			case NEGATE:
			case F_NEG:
				if (p->type == TYP_ERR)
					break;
				if (p->type == TYP_INT)
					p->Int = -(p->Int);
				else if (p->type == TYP_FLT)
					p->Float = -(p->Float);
				else
					ERROR (NON_NUMBER);
				break;

			case F_RND:
				p->Int = (random () % (p->Int)) + 1;
				break;

			case NOT:
			case F_NOT:
				p->Value = !(p->Value);
				break;

			case F_ISERR:
				p->Value = (p->type == TYP_ERR);
				p->type = TYP_BOL;
				break;

			case F_ISNUM:
				if (p->type == TYP_FLT || p->type == TYP_INT)
					p->Value = 1;
				else if (p->type == TYP_STR)
				{
					strptr = p->String;
					(void) astof (&strptr);
					p->Value = (*strptr == '\0');
				}
				else
					p->Value = 0;
				p->type = TYP_BOL;
				break;

			case F_ROWS:
			case F_COLS:
				p->type = TYP_INT;
				p->Int = 1 + (byte == F_ROWS ? (p->Rng.hr - p->Rng.lr) : (p->Rng.hc - p->Rng.lc));
				break;

				/* Two operand cmds */
			case F_ATAN2:
			case F_HYPOT:
			case POW:
				{
					double (*funp2) (double, double);
					funp2 = (double (*)(double, double)) (f->fn_fun);

					p->Float = (*funp2) (p->Float, (p + 1)->Float);
					if (p->Float != p->Float)
						ERROR (OUT_OF_RANGE);
				}
				break;

			case DIFF:
			case DIV:
			case MOD:
			case PROD:
			case SUM:
				do_math_binop(byte, p, p+1);
				break;
			case EQUAL:
			case NOTEQUAL:

			case GREATEQ:
			case GREATER:
			case LESS:
			case LESSEQ:
				if (p->type == TYP_ERR)
					break;
				if ((p + 1)->type == TYP_ERR)
					ERROR ((p + 1)->Value);

				if (p->type == TYP_BOL || (p + 1)->type == TYP_BOL)
				{
					if (p->type != (p + 1)->type || (byte != EQUAL && byte != NOTEQUAL))
						ERROR (BAD_INPUT);
					if (byte == EQUAL)
						p->Value = p->Value == (p + 1)->Value;
					else
						p->Value = p->Value != (p + 1)->Value;
					break;
				}
				if (p->type != (p + 1)->type)
				{
					if (p->type == 0)
					{
						if ((p + 1)->type == TYP_STR)
						{
							p->type = TYP_STR;
							p->String = "";
						}
						else if ((p + 1)->type == TYP_INT)
						{
							p->type = TYP_INT;
							p->Int = 0;
						}
						else
						{
							p->type = TYP_FLT;
							p->Float = 0.0;
						}
					}
					else if ((p + 1)->type == 0)
					{
						if (p->type == TYP_STR)
						{
							(p + 1)->type = TYP_STR;
							(p + 1)->String = "";
						}
						else if (p->type == TYP_INT)
						{
							(p + 1)->type = TYP_INT;
							(p + 1)->Int = 0;
						}
						else
						{
							(p + 1)->type = TYP_FLT;
							(p + 1)->Float = 0.0;
						}
					}
					else if (p->type == TYP_STR)
					{
						strptr = p->String;
						if ((p + 1)->type == TYP_INT)
						{
							p->type = TYP_INT;
							p->Int = astol (&strptr);
						}
						else
						{
							p->type = TYP_FLT;
							p->Float = astof (&strptr);
						}
						if (*strptr)
						{
							p->type = TYP_BOL;
							p->Value = (byte == NOTEQUAL);
							break;
						}
					}
					else if ((p + 1)->type == TYP_STR)
					{
						strptr = (p + 1)->String;
						if (p->type == TYP_INT)
							(p + 1)->Int = astol (&strptr);
						else
							(p + 1)->Float = astof (&strptr);
						if (*strptr)
						{
							p->type = TYP_BOL;
							p->Value = (byte == NOTEQUAL);
							break;
						}

						/* If we get here, one is INT, and the other
						   is FLT  Make them both FLT */
					}
					else if (p->type == TYP_INT)
					{
						p->type = TYP_FLT;
						p->Float = (double) p->Int;
					}
					else
						(p + 1)->Float = (double) (p + 1)->Int;
				}
				if (p->type == TYP_STR)
					tmp = strcmp (p->String, (p + 1)->String);
				else if (p->type == TYP_FLT)
					tmp = (p->Float < (p + 1)->Float) ? -1 : ((p->Float > (p + 1)->Float) ? 1 : 0);
				else if (p->type == TYP_INT)
					tmp = (p->Int < (p + 1)->Int ? -1 : ((p->Int > (p + 1)->Int) ? 1 : 0));
				else if (p->type == 0)
					tmp = 0;
				else
				{
					tmp = 0;
					panic ("Bad type value %d", p->type);
				}
				p->type = TYP_BOL;
				if (tmp < 0)
					p->Value = (byte == NOTEQUAL || byte == LESS || byte == LESSEQ);
				else if (tmp == 0)
					p->Value = (byte == EQUAL || byte == GREATEQ || byte == LESSEQ);
				else
					p->Value = (byte == NOTEQUAL || byte == GREATER || byte == GREATEQ);
				break;

			case F_FIXED:
				tmp = (p + 1)->Int;
				if (tmp < -29 || tmp > 29)
					ERROR (OUT_OF_RANGE);
				if (tmp < 0) {
					num_t f1 = (p->Float) / exp10_arr[-tmp];
					num_t f2 = rintn(f1);
					//num f3 = f2 * exp10_arr[-tmp];
					p->Float = rintn (f2) * exp10_arr[-tmp];
				} else {
					p->Float = rintn ((p->Float) * exp10_arr[tmp]) / exp10_arr[tmp];
				}
				break;

			case F_IFERR:
				if (p->type == TYP_ERR)
					*p = *(p + 1);
				break;

			case F_INDEX:
				tmp = (p + 1)->Int - 1;
				if (tmp < 0)
					ERROR (OUT_OF_RANGE);
				lrow = p->Rng.lr;
				lcol = p->Rng.lc;
				hrow = p->Rng.hr;
				hcol = p->Rng.hc;
				if (lrow != hrow && lcol != hcol)
				{
					int dex;

					dex = 1 + hrow - lrow;
					if (tmp >= dex * (1 + hcol - lcol))
						ERROR (OUT_OF_RANGE);
					crow = tmp % dex;
					ccol = tmp / dex;
					lrow += crow;
					lcol += ccol;
				}
				else if (lrow != hrow)
				{
					if (tmp > (hrow - lrow))
						ERROR (OUT_OF_RANGE);
					lrow += tmp;
				}
				else
				{
					if (tmp > (hcol - lcol))
						ERROR (OUT_OF_RANGE);
					lcol += tmp;
				}
				cell_ptr = find_cell (lrow, lcol);
				PUSH_ANY (cell_ptr);
				break;

			case F_INDEX2:
				crow = (p + 1)->Int - 1;
				ccol = (p + 2)->Int - 1;
				lrow = p->Rng.lr;
				lcol = p->Rng.lc;
				hrow = p->Rng.hr;
				hcol = p->Rng.hc;
				if (crow > (hrow - lrow) || ccol > (hcol - lcol)) 
					ERROR (OUT_OF_RANGE);
				cell_ptr = find_cell (lrow + crow, lcol + ccol);
				PUSH_ANY (cell_ptr);
				break;

				/* case F_PRINTF:
				   panic("no printf yet");
				   break; */

			case CONCAT:
				strptr = (char *) obstack_alloc (&tmp_mem, strlen (p->String) + strlen ((p + 1)->String) + 1);
				strcpy (strptr, p->String);
				strcat (strptr, (p + 1)->String);
				p->String = strptr;
				break;

			case F_ONEOF:
				if (numarg < 2)
					ERROR (NO_VALUES);
				--numarg;
				tmp = p->Int;
				if (tmp < 1 || tmp > numarg)
					ERROR (OUT_OF_RANGE);
				/* Can never happen? */
				TO_ANY (p + tmp);
				p[0] = p[tmp];
				break;

				/* This is now a fallthrough for all the USRmumble codes */
			case USR1:
			default:
				if ((f->fn_argn & X_ARGS) == X_AN)
				{
					void (*funp) (int, struct value *);
					funp = (void (*)(int, struct value *)) f->fn_fun;
					(*funp) (numarg, p);
				}
				else
				{
					void (*funp) (struct value *);
					funp = (void (*)(struct value *)) f->fn_fun;
					(*funp) (p);
				}
				break;

				/* #ifdef TEST
				   default:
				   panic("Unknown byte-value %d",byte);
				   break;
#endif */
		}
		/* Goto next-byte is the equiv of a multi-level break, which
		   C doesn't allow. */
next_byte:
		;
	}
#ifdef TEST
	if (curstack != 1)
		io_error_msg ("%d values on stack", curstack);
#endif
	return stack;
}

/* These helper functions were split out so that eval_expression would compile
   under Turbo C 2.0 on my PC.
 */


static int cnt_flt;
static int cnt_int;

static long int_tmp;
static double flt_tmp;

static long sqr_int_tmp;	/* for AREA_STD */
static double sqr_flt_tmp;

static unsigned char area_cmd;



double
dtr (double x)
{
	return x * (M_PI / (double) 180.0);
}

double
rtd (double x)
{
	return x * (180.0 / (double) M_PI);
}

double
to_int (double x)
{
	return (x < 0 ? ceil (x) : floor (x));
}

/* Various methods of dealing with arithmatic overflow.  They don't work well.
   Someone should really convince this thing to properly deal with it.
 */
RETSIGTYPE
math_sig ( int sig)
{
	stack[curstack].type = TYP_ERR;
	stack[curstack].Value = BAD_INPUT;
}


/* Here's the entry point for this module. */
void
update_cell(CELL *cell)
{
	struct value *newv;
	int new_val;

	newv = eval_expression (cell->get_cell_formula());
	if (!newv)
	{
		push_refs (cell->cell_refs_from);
		return;
	}

	cell->cell_cycle = current_cycle;
	//cell->set_omnival(newv); // The rest of this function should be redundant after this functionality has been set up properly

	if (newv->type != GET_TYP (cell))
	{
		if (GET_TYP (cell) == TYP_STR) free (cell->gString());
		SET_TYP (cell, newv->type);
		new_val = 1;
		if (newv->type == TYP_STR) newv->String = strdup (newv->String);
	}
	else
		switch (newv->type)
		{
			case 0:
				new_val = 0;
				break;
			case TYP_FLT:
				new_val = newv->Float != cell->gFlt();
				break;
			case TYP_INT:
				new_val = newv->Int != cell->gInt();
				break;
			case TYP_STR:
				new_val = strcmp (newv->String, cell->gString());
				if (new_val)
				{
					free (cell->gString());
					newv->String = strdup (newv->String);
				}
				break;
			case TYP_BOL:
				new_val = newv->Value != cell->gBol();
				break;
			case TYP_ERR:
				new_val = newv->Value != cell->gErr();
				break;
			default:
				new_val = 0;
#ifdef TEST
				panic ("Unknown type %d in update_cell", newv->type);
#endif
		}
	if (new_val)
	{
		cell->set_c_z(newv->x);
		push_refs (cell->cell_refs_from);
	}
	(void) obstack_free (&tmp_mem, tmp_mem_start);
}

int
fls (long a_num)
{
	int ret = 1;

	if (!a_num)
		return 0;
	if (a_num < 0)
		a_num = -a_num;
	if (a_num & 0xffff0000)
	{
		ret += 16;
		a_num = (a_num >> 16) & 0xffff;
	}
	if (a_num & 0xff00)
	{
		ret += 8;
		a_num >>= 8;
	}
	if (a_num & 0xf0)
	{
		ret += 4;
		a_num >>= 4;
	}
	if (a_num & 0x0c)
	{
		ret += 2;
		a_num >>= 2;
	}
	if (a_num & 2)
		ret++;
	return ret;
}

