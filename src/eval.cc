/*
 * Copyright (c) 1990, 1992, 1993, 2001 Free Software Foundation, Inc.
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

constexpr auto pi = std::acos(-1);

#include "global.h"
#include "convert.h"
#include "cell.h"
#include "eval.h"
#include "errors.h"
#include "io-utils.h"
#include "ref.h"
#include "sheet.h"
#include "xcept.h"



void throw_valerr(int n, struct value* vp)
{
	vp->sErr(n);
	throw ValErr(n);
}


extern int n_usr_funs;

RETSIGTYPE math_sig ( int sig);


#define Float	x.c_n
#define Int	x.c_l
#define Rng	x.c_r

static struct value *eval_stack;
static int stackmax;
static int curstack;

unsigned short current_cycle;

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


void ERROR1 (int cause) { throw cause;}
void ERROR2 (struct value* v) { ERROR1(v->gErr()); }

void TO_FLT(struct value* val)
{	
	if((val)->type==TYP_FLT) 
		; 
	else if((val)->type==TYP_INT) { 
		(val)->type=TYP_FLT; 
		(val)->Float=(double)(val)->Int; 
	} else if((val)->type==TYP_STR) { 
		bool ok;
		val->sFlt(to_double(val->gString(), ok));
		if(!ok) ERROR1(NON_NUMBER);
	} else if((val)->type==TYP_ERR) {
		ERROR2(val); 
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
		bool ok;
		val->sInt(to_long(val->gString(), ok));
		if(!ok) ERROR1(NON_NUMBER);
	} else if((val)->type==TYP_ERR) {
		ERROR2(val); 
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
		bool ok;
		val->sFlt(to_double(val->gString(), ok));
		if(!ok) ERROR1(NON_NUMBER);
	} else if((val)->type==TYP_ERR) {
		ERROR2(val); 
	} else if((val)->type==0) { 
		(val)->type=TYP_INT; 
		(val)->Int=0; 
	} else 
		ERROR1(NON_NUMBER);
}

void TO_STR(struct value* val)
{
	if((val)->type==TYP_STR)	
		;	
	else if((val)->type==TYP_INT) {	
		val->sString(format("%ld", val->gInt()));
	} else if((val)->type==TYP_FLT) {		
		val->sString(flt_to_str(val->gFlt()));
	} else if((val)->type==TYP_ERR) {		
		ERROR2(val);	
	} else if((val)->type==0) {	
		val->sString("");
	} else 
		ERROR1(NON_STRING);
}

void TO_BOL(struct value* val)	
{
	if((val)->type==TYP_BOL)	
		;	
	else if((val)->type==TYP_ERR) {	
		ERROR2(val);	
	} else	
		ERROR1(NON_BOOL);
}


void TO_RNG(struct value* val) 
{
	if((val)->type==TYP_RNG) 
		; 
	else if((val)->type==TYP_ERR) {
		ERROR2(val); 
	} else 
		ERROR1(NON_RANGE);
}


void TO_ANY(struct value* val) 
{
	if((val)->type==TYP_RNG) 
		ERROR1(BAD_INPUT); 
}


void PUSH_ANY(struct value* value_ptr, cell* cp)
{
	if(!cp || !GET_TYP(cp)) {		
		value_ptr->type=TYP_NUL;			
		value_ptr->Int=0;			
	} else {				
		value_ptr->type=GET_TYP(cp);		
		value_ptr->x=cp->get_c_z();			
	}
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

static void do_math_binop(int op, struct value* p1, struct value* p2)
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
			TO_STR(p);
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
	eval_stack = (struct value *) ck_malloc (stackmax * sizeof (struct value));
	curstack = 0;
	current_cycle = 1;
	(void) signal (SIGFPE, math_sig);
}

static void compare_values(const unsigned byte, struct value *value_ptr)
{
	int tmp;
	char *strptr;

	struct value *v0 = value_ptr;
	struct value *v1 = value_ptr+1;
	auto t0 = v0->type, t1 = v1->type;

	assert(byte == NOTEQUAL || byte == LESS || byte == LESSEQ || byte == EQUAL ||
			byte == GREATEQ || byte == NOTEQUAL || byte == GREATER);
	if (value_ptr->type == TYP_ERR)
		return;
	if ((value_ptr + 1)->type == TYP_ERR)
		throw_valerr((value_ptr + 1)->gInt(), value_ptr);

	if (value_ptr->type == TYP_BOL || (value_ptr + 1)->type == TYP_BOL)
	{
		if (value_ptr->type != (value_ptr + 1)->type || (byte != EQUAL && byte != NOTEQUAL))
			throw_valerr(BAD_INPUT, value_ptr);
		if (byte == EQUAL)
			value_ptr->sBol(value_ptr->gBol() == (value_ptr + 1)->gBol());
		else
			value_ptr->sBol(value_ptr->gBol() != (value_ptr + 1)->gBol());
		return;
	}
	if (value_ptr->type != (value_ptr + 1)->type)
	{
		if (value_ptr->type == 0)
		{
			if ((value_ptr + 1)->type == TYP_STR)
			{
				value_ptr->sString("");
			}
			else if ((value_ptr + 1)->type == TYP_INT)
			{
				value_ptr->sInt(0);
			}
			else
			{
				value_ptr->sFlt(0.0);
			}
		}
		else if ((value_ptr + 1)->type == 0)
		{
			if (value_ptr->type == TYP_STR)
			{				
				(value_ptr + 1)->sString("");
			}
			else if (value_ptr->type == TYP_INT)
			{
				(value_ptr+1)->sInt(0);
			}
			else
			{
				(value_ptr+1)->sFlt(0.0);
			}
		}
		else if (value_ptr->type == TYP_STR)
		{
			bool ok = false;
			const char* s = value_ptr->gString();
			//strptr = value_ptr->gString();
			if ((value_ptr + 1)->type == TYP_INT)
			{
				//value_ptr->type = TYP_INT;
				//value_ptr->Int = astol (&strptr);
				value_ptr->sInt(to_long(s, ok));
			}
			else
			{
				//value_ptr->type = TYP_FLT;
				//value_ptr->Float = astof (&strptr);
				value_ptr->sFlt(to_double(s, ok));
			}
			//if (*strptr)
			if(!ok)
			{
				value_ptr->sBol((byte == NOTEQUAL));
				return;
			}
		}
		else if ((value_ptr + 1)->type == TYP_STR)
		{
		       	bool ok = false ;
			const char* s = (value_ptr+1)->gString();
			if (value_ptr->type == TYP_INT)
				(value_ptr + 1)->sInt(to_long(s, ok));
			else
				(value_ptr + 1)->sFlt(to_double(s, ok));
			if(!ok)
			{
				value_ptr->sBol(byte == NOTEQUAL);
				return;
			}

			/* If we get here, one is INT, and the other
			   is FLT  Make them both FLT */
		}
		else if (value_ptr->type == TYP_INT)
		{
			value_ptr->type = TYP_FLT;
			value_ptr->Float = (double) value_ptr->Int;
		}
		else
			(value_ptr + 1)->Float = (double) (value_ptr + 1)->Int;
	}
	if (value_ptr->type == TYP_STR)
		tmp = strcmp (value_ptr->gString(), (value_ptr + 1)->gString());
	else if (value_ptr->type == TYP_FLT)
		tmp = (value_ptr->Float < (value_ptr + 1)->Float) ? -1 : ((value_ptr->Float > (value_ptr + 1)->Float) ? 1 : 0);
	else if (value_ptr->type == TYP_INT)
		tmp = (value_ptr->Int < (value_ptr + 1)->Int ? -1 : ((value_ptr->Int > (value_ptr + 1)->Int) ? 1 : 0));
	else if (value_ptr->type == 0)
		tmp = 0;
	else
	{
		tmp = 0;
		panic ("Bad type value %d", value_ptr->type);
	}
	if (tmp < 0)
		value_ptr->sBol(byte == NOTEQUAL || byte == LESS || byte == LESSEQ);
	else if (tmp == 0)
		value_ptr->sBol(byte == EQUAL || byte == GREATEQ || byte == LESSEQ);
	else
		value_ptr->sBol(byte == NOTEQUAL || byte == GREATER || byte == GREATEQ);
}

// Maybe contains too many parameters, but it will do as a first cut
static void switch_by_byte(unsigned char &byte, unsigned &numarg, int &tmp, 
		struct value *value_ptr, unsigned &jumpto, unsigned char *&expr,
		function_t *f)
{
	cell* cell_ptr;
	char *strptr;
	switch (byte) {
		case IF_L:
		case F_IF_L:
		case IF:
		case F_IF:
			if (value_ptr->type != TYP_BOL)
			{
				if (value_ptr->type != TYP_ERR)
				{
					value_ptr->sErr(NON_BOOL);
				}
				expr += jumpto;
				if (expr[-2] != SKIP)
					jumpto = expr[-1] + (((unsigned) expr[-2]) << 8);
				else
					jumpto = expr[-1];
				expr += jumpto;	/* Skip both branches of the if */

			}
			else if (value_ptr->gBol() == 0)
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
			if (value_ptr->type == TYP_ERR)
				expr += jumpto;
			else if (value_ptr->type != TYP_BOL)
			{
				value_ptr->sErr(NON_BOOL);
				expr += jumpto;
			}
			else if (value_ptr->gBol() == 0)
				expr += jumpto;
			else
				--curstack;
			break;

		case OR_L:
		case OR:
			if (value_ptr->type == TYP_ERR)
				expr += jumpto;
			else if (value_ptr->type != TYP_BOL)
			{
				value_ptr->sErr(NON_BOOL);
				expr += jumpto;
			}
			else if (value_ptr->gBol())
				expr += jumpto;
			else
				--curstack;
			break;

		case CONST_FLT:
			value_ptr->type = TYP_FLT;
			bcopy ((VOIDSTAR) expr, (VOIDSTAR) (&(value_ptr->Float)), 
					sizeof (double));
			expr += sizeof (double);
			break;

		case CONST_INT:
			value_ptr->type = TYP_INT;
			bcopy ((VOIDSTAR) expr, (VOIDSTAR) (&(value_ptr->Int)), 
					sizeof (long));
			expr += sizeof (long);
			break;

		case CONST_STR:
		case CONST_STR_L:
			value_ptr->sString((char *) expr + jumpto);
			break;

		case CONST_ERR:
			value_ptr->sErr(*expr++);
			/* expr+=sizeof(char *); */
			break;

		case CONST_INF:
		case CONST_NINF:
		case CONST_NAN:
			value_ptr->type = TYP_FLT;
			value_ptr->Float = (byte == CONST_INF) ? __plinf : ((byte == CONST_NINF) ? __neinf : NAN);
			break;

		case VAR:
			{
				struct var *varp;

				bcopy ((VOIDSTAR) expr, (VOIDSTAR) (&varp), sizeof (struct var *));
				expr += sizeof (struct var *);
				switch (varp->var_flags)
				{
					case VAR_UNDEF:
						value_ptr->sErr(BAD_NAME);
						break;

					case VAR_CELL:
						cell_ptr = find_cell (varp->v_rng.lr, varp->v_rng.lc);
						PUSH_ANY(value_ptr, cell_ptr);
						break;

					case VAR_RANGE:
						if (varp->v_rng.lr == varp->v_rng.hr && varp->v_rng.lc == varp->v_rng.hc)
						{
							cell_ptr = find_cell (varp->v_rng.lr, varp->v_rng.lc);
							PUSH_ANY(value_ptr, cell_ptr);
						}
						else
						{
							value_ptr->type = TYP_RNG;
							value_ptr->Rng = varp->v_rng;
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
				PUSH_ANY(value_ptr, cell_ptr);
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
			value_ptr->type = TYP_RNG;
			GET_RNG (expr, &(value_ptr->Rng));
			expr += EXP_ADD_RNG;
			break;

		case F_TRUE:
		case F_FALSE:
			value_ptr->sBol((byte == F_TRUE));
			break;

		case F_PI:
			value_ptr->type = TYP_FLT;
			value_ptr->Float = pi;
			break;

		case F_ROW:
		case F_COL:
			value_ptr->type = TYP_INT;
			value_ptr->Int = ((byte == F_ROW) ? cur_row : cur_col);
			break;

		case F_NOW:
			value_ptr->type = TYP_INT;
			value_ptr->Int = time (nullptr);
			break;

			/* Single operand instrs */			

		case NEGATE:
		case F_NEG:
			if (value_ptr->type == TYP_ERR)
				break;
			if (value_ptr->type == TYP_INT)
				value_ptr->Int = -(value_ptr->Int);
			else if (value_ptr->type == TYP_FLT)
				value_ptr->Float = -(value_ptr->Float);
			else
				throw_valerr(NON_NUMBER, value_ptr);
			break;

		case NOT:
		case F_NOT:
			value_ptr->sBol(!(value_ptr->gBol()));
			break;

		case F_ROWS:
		case F_COLS:
			value_ptr->type = TYP_INT;
			value_ptr->Int = 1 + (byte == F_ROWS ? 
					(value_ptr->Rng.hr - value_ptr->Rng.lr) 
					: (value_ptr->Rng.hc - value_ptr->Rng.lc));
			break;

			/* Two operand cmds */
		case POW:
			{
				double (*funp2) (double, double);
				funp2 = (double (*)(double, double)) (f->fn_fun);

				value_ptr->Float = (*funp2) (value_ptr->Float, 
						(value_ptr + 1)->Float);
				if (value_ptr->Float != value_ptr->Float)
					throw_valerr(OUT_OF_RANGE, value_ptr);
			}
			break;

		case DIFF:
		case DIV:
		case MOD:
		case PROD:
		case SUM:
			do_math_binop(byte, value_ptr, value_ptr+1);
			break;

		case EQUAL:
		case NOTEQUAL:
		case GREATEQ:
		case GREATER:
		case LESS:
		case LESSEQ:
			compare_values(byte, value_ptr);
			break;

		case F_FIXED:
			tmp = (value_ptr + 1)->Int;
			if (tmp < -29 || tmp > 29)
				throw_valerr(OUT_OF_RANGE, value_ptr);
			if (tmp < 0) {
				num_t f1 = (value_ptr->Float) / exp10_arr[-tmp];
				num_t f2 = rintn(f1);
				//num f3 = f2 * exp10_arr[-tmp];
				value_ptr->Float = rintn (f2) * exp10_arr[-tmp];
			} else {
				value_ptr->Float = rintn ((value_ptr->Float) * exp10_arr[tmp]) / exp10_arr[tmp];
			}
			break;

		case F_IFERR:
			if (value_ptr->type == TYP_ERR)
				*value_ptr = *(value_ptr + 1);
			break;



		case CONCAT:
			{
				std::string s{value_ptr->gString()};
				s+= (value_ptr+1)->gString();
				value_ptr->sString(s);
			}
			break;


			/* This is now a fallthrough for all the USRmumble codes */
		case USR1:
		default:
			if ((f->fn_argn & X_ARGS) == X_AN)
			{
				void (*funp) (int, struct value *);
				funp = (void (*)(int, struct value *)) f->fn_fun;
				(*funp) (numarg, value_ptr);
			}
			else
			{
				void (*funp) (struct value *);
				funp = (void (*)(struct value *)) f->fn_fun;
				(*funp) (value_ptr);
			}
			break;
	}
}
/* This huge function takes a byte-compiled expression and executes it. */
static struct value *
eval_expression (unsigned char *expr)
{
	if (!expr) return 0;

	unsigned char byte;
	unsigned numarg = 0;
	unsigned jumpto = 0;
	function_t *f;
	struct value *value_ptr = 0;
	int tmp;

	CELLREF lrow, hrow, crow;
	CELLREF lcol, hcol, ccol;

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
					eval_stack = (struct value *) ck_realloc (eval_stack, sizeof (struct value) * stackmax);
				}
				value_ptr = &eval_stack[curstack];
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
				value_ptr = 0;
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
			value_ptr = &eval_stack[curstack - numarg];
			curstack -= (numarg - 1);
			for (xt = 0; xt < numarg; xt++)
			{
				char arg_type =f->fn_argt[xt <= 3 ? xt : 3];
				try {
					fill_argument(arg_type, value_ptr+xt);
				} catch (int e) {
					value_ptr->sErr(e);
					goto next_byte;
				}

			}
		}

		try {
			switch_by_byte(byte, numarg, tmp, value_ptr, jumpto, expr, f);
		} catch (ValErr& e) {
			goto next_byte;
		}
next_byte:
		;
	}
#ifdef TEST
	if (curstack != 1)
		io_error_msg ("%d values on stack", curstack);
#endif
	return eval_stack;
}

static int cnt_flt;
static int cnt_int;

static long int_tmp;
static double flt_tmp;

static long sqr_int_tmp;	/* for AREA_STD */
static double sqr_flt_tmp;

static unsigned char area_cmd;

/* Various methods of dealing with arithmatic overflow.  They don't work well.
   Someone should really convince this thing to properly deal with it.
 */
RETSIGTYPE
math_sig ( int sig)
{
	eval_stack[curstack].sErr(BAD_INPUT);
}


/* Here's the entry point for this module. */
void
update_cell(CELL *cell)
{
	struct value *newv = eval_expression (cell->get_cell_formula());

	if (!newv) {
		push_refs(cell);
		return;
	}

	int new_val = 0; 
	cell->cell_cycle = current_cycle;
	if (newv->type != GET_TYP (cell)) {
		SET_TYP (cell, newv->type);
		new_val = 1;
	} else {
		switch (newv->type) {
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
				new_val = strcmp (newv->gString(), cell->gString());
				break;
			case TYP_BOL:
				new_val = newv->gBol() != cell->gBol();
				break;
			case TYP_ERR:
				new_val = newv->gBol() != cell->gErr();
				break;
			default:
				new_val = 0;
#ifdef TEST
				panic ("Unknown type %d in update_cell", newv->type);
#endif
		}
	}

	if(!new_val) return;
	cell->set_c_z(newv->x);
	push_refs(cell);
	
}


