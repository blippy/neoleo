%{
/*
 * $Id: parse.y,v 1.11 2001/02/04 00:03:48 pw Exp $
 *
 * Copyright (C) 1990, 1992, 1993, 1999 Free Software Foundation, Inc.
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
%}

%right '?' ':'
/* %left '|' */
%left '&'
%nonassoc '=' NE
%nonassoc '<' LE '>' GE
%left '+' '-'
%left '*' '/' '%'
%right '^'
%left NEG '!'

%token	L_CELL L_RANGE
%token	L_VAR

%token	L_CONST
%token	L_FN0	L_FN1	L_FN2	L_FN3	L_FN4	L_FNN
%token	L_FN1R	L_FN2R	L_FN3R	L_FN4R	L_FNNR

%token	L_LE	L_NE	L_GE

%{
#include "funcdef.h"

#include <ctype.h>

#define obstack_chunk_alloc ck_malloc
#define obstack_chunk_free free
#include "obstack.h"
#include "sysdef.h"

#include "global.h"
#include "errors.h"
#include "node.h"
#include "eval.h"
#include "ref.h"

int yylex ();
void yyerror (char *);
VOIDSTAR parse_hash;
extern VOIDSTAR hash_find();

/* This table contains a list of the infix single-char functions */
unsigned char fnin[] = {
	SUM, DIFF, DIV, PROD, MOD, /* AND, OR, */ POW, EQUAL, IF, CONCAT, 0
};

#define YYSTYPE _y_y_s_t_y_p_e
typedef struct node *YYSTYPE;
YYSTYPE parse_return;
YYSTYPE make_list (YYSTYPE, YYSTYPE);

char *instr;
int parse_error = 0;
extern struct obstack tmp_mem;

%}
%%
line:	exp
		{ parse_return=$1; }
	| error {
		if(!parse_error)
			parse_error=PARSE_ERR;
		parse_return=0; }
	;

exp:	  L_CONST
	| cell
	| L_FN0 '(' ')' {
		$$=$1; }
	| L_FN1 '(' exp ')' {
		($1)->n_x.v_subs[0]=$3;
		($1)->n_x.v_subs[1]=(struct node *)0;
		$$=$1; }
	| L_FN2 '(' exp ',' exp ')' {
		($1)->n_x.v_subs[0]=$3;
		($1)->n_x.v_subs[1]=$5;
		$$=$1; }
	| L_FN3 '(' exp ',' exp ',' exp ')' {
		($1)->n_x.v_subs[0]=make_list($3,$5);
 		($1)->n_x.v_subs[1]=$7;
 		$$=$1;}
	| L_FN4 '(' exp ',' exp ',' exp ',' exp ')' {
		($1)->n_x.v_subs[0]=make_list($3,$5);
 		($1)->n_x.v_subs[1]=make_list($7,$9);
 		$$=$1;}
	| L_FNN '(' exp_list ')' {
		($1)->n_x.v_subs[0]=(struct node *)0;
		($1)->n_x.v_subs[1]=$3;
		$$=$1; }
	| L_FN1R '(' L_RANGE ')' {
		$1->n_x.v_subs[0]=$3;
		$$=$1; }
	| L_FN1R '(' L_VAR ')' {
		$1->n_x.v_subs[0]=$3;
		$$=$1; }

	| L_FN2R '(' L_RANGE ',' exp ')' {
		$1->n_x.v_subs[0]=$3;
		$1->n_x.v_subs[1]=$5;
		$$=$1; }
	| L_FN2R '(' L_VAR ',' exp ')' {
		$1->n_x.v_subs[0]=$3;
		$1->n_x.v_subs[1]=$5;
		$$=$1; }

	/* JF:  These should be FN2R, but I'm hacking this for SYLNK */
	| L_FN2R '(' L_RANGE ',' exp ',' exp ')' {
		if($1->comp_value!=F_INDEX)
			parse_error=PARSE_ERR;
		$1->comp_value=F_INDEX2;
		$1->n_x.v_subs[0]=make_list($3,$5);
		$1->n_x.v_subs[1]=$7;
		$$=$1; }
	| L_FN2R '(' L_VAR ',' exp ',' exp ')' {
		if($1->comp_value!=F_INDEX)
			parse_error=PARSE_ERR;
		$1->comp_value=F_INDEX2;
		$1->n_x.v_subs[0]=make_list($3,$5);
		$1->n_x.v_subs[1]=$7;
		$$=$1; }

	| L_FN3R '(' L_RANGE ',' exp ',' exp ')' {
		($1)->n_x.v_subs[0]=make_list($3,$5);
 		($1)->n_x.v_subs[1]=$7;
 		$$=$1;}
	| L_FN3R '(' L_VAR ',' exp ',' exp ')' {
		($1)->n_x.v_subs[0]=make_list($3,$5);
 		($1)->n_x.v_subs[1]=$7;
 		$$=$1;}

	| L_FN4R '(' L_RANGE ',' exp ',' exp ',' exp ')' {
		($1)->n_x.v_subs[0]=make_list($3,$5);
 		($1)->n_x.v_subs[1]=make_list($7,$9);
 		$$=$1;}
	| L_FN4R '(' L_VAR ',' exp ',' exp ',' exp ')' {
		($1)->n_x.v_subs[0]=make_list($3,$5);
 		($1)->n_x.v_subs[1]=make_list($7,$9);
 		$$=$1;}

	| L_FNNR '(' range_exp_list ')' {
		($1)->n_x.v_subs[0]=(struct node *)0;
		($1)->n_x.v_subs[1]=$3;
		$$=$1; }
	| exp '?' exp ':' exp {
		$2->comp_value=IF;
		$2->n_x.v_subs[0]=$4;
		$2->n_x.v_subs[1]=$5;
		$4->n_x.v_subs[0]=$1;
		$4->n_x.v_subs[1]=$3;
		$$=$2; }
	/* | exp '|' exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; } */
	| exp '&' exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; }
	| exp '<' exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; }
	| exp LE exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; }
	| exp '=' exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; }
	| exp NE exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; }
	| exp '>' exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; }
	| exp GE exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; }
	| exp '+' exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; }
	| exp '-' exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; }
	| exp '*' exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; }
	| exp '/' exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; }
	| exp '%' exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; }
	| exp '^' exp {
		$2->n_x.v_subs[0]=$1;
		$2->n_x.v_subs[1]=$3;
		$$ = $2; }
	| '+' exp {
		if($2->comp_value==CONST_FLT) {
			$2->n_x.v_float= ($2->n_x.v_float);
			$$=$2;
		} else if($2->comp_value==CONST_INT) {
			$2->n_x.v_int= ($2->n_x.v_int);
			$$=$2;
		} else {
			$$ = $2;
		} }
	| '-' exp %prec NEG {
		if($2->comp_value==CONST_FLT) {
			$2->n_x.v_float= -($2->n_x.v_float);
			/* free($1); */
			$$=$2;
		} else if($2->comp_value==CONST_INT) {
			$2->n_x.v_int= -($2->n_x.v_int);
			/* free($1); */
			$$=$2;
		} else {
			$1->comp_value = NEGATE;
			$1->n_x.v_subs[0]=$2;
			$1->n_x.v_subs[1]=(struct node *)0;
			$$ = $1;
		} }
	| '!' exp {
		$1->n_x.v_subs[0]=$2;
		$1->n_x.v_subs[1]=(struct node *)0;
		$$ = $1; }
	| '(' exp ')'
		{ $$ = $2; }
	| '(' exp error {
		if(!parse_error)
			parse_error=NO_CLOSE;
		}
	/* | exp ')' error {
		if(!parse_error)
			parse_error=NO_OPEN;
		} */
	| '(' error {
		if(!parse_error)
			parse_error=NO_CLOSE;
		}
	;


exp_list: exp
 		{ $$ = make_list($1, 0); }
	| exp_list ',' exp
		{ $$ = make_list($3, $1); }
	;

range_exp: L_RANGE
	| exp
	;

range_exp_list: range_exp
		{ $$=make_list($1, 0); }
	|   range_exp_list ',' range_exp
		{ $$=make_list($3,$1); }
	;

cell:	L_CELL
		{ $$=$1; }
	| L_VAR
	;
%%

void
yyerror (char * s)
{
	if(!parse_error)
		parse_error=PARSE_ERR;
}

YYSTYPE
make_list (YYSTYPE car, YYSTYPE cdr)
{
	YYSTYPE ret;

	ret=(YYSTYPE)obstack_alloc(&tmp_mem,sizeof(*ret));
	ret->comp_value = 0;
	ret->n_x.v_subs[0]=car;
	ret->n_x.v_subs[1]=cdr;
	return ret;
}

#define ERROR -1

extern struct node *yylval;

unsigned char parse_cell_or_range (char **,struct rng *);
int str_to_col (char ** str);

int
yylex ()
{
	int ch;
	struct node *new;
	int isflt;
	char *begin;
	char *tmp_str;
	unsigned char byte_value;
	int n;

	/* unsigned char *ptr; */
	int nn;
	struct function *fp;
	int tmp_ch;

#ifdef TEST
	if(!instr)
		return ERROR;
#endif
	while(isspace(*instr))
		instr++;
	ch = *instr++;
	if(ch=='(' || ch==',' || ch==')')
		return ch;

	new=(struct node *)obstack_alloc(&tmp_mem,sizeof(struct node));
	new->add_byte=0;
	new->sub_value=0;
	switch(ch) {
	case 0:
		return 0;

	case '0': case '1': case '2': case '3': case '4': case '5': case '6':
	case '7': case '8': case '9': case '.':
		isflt = (ch=='.');

		begin=instr-1;
		tmp_str=instr;

		while(isdigit(*tmp_str) || (!isflt && *tmp_str=='.' && ++isflt))
			tmp_str++;
		if(*tmp_str=='e' || *tmp_str=='E') {
			isflt=1;
			tmp_str++;
			if(*tmp_str=='-' || *tmp_str=='+')
				tmp_str++;
			while(isdigit(*tmp_str))
				tmp_str++;
		}
		if(isflt) {
			new->n_x.v_float=astof((char **)(&begin));
			byte_value=CONST_FLT;
		} else {
			new->n_x.v_int=astol((char **)(&begin));
			if(begin!=tmp_str) {
				begin=instr-1;
				new->n_x.v_float=astof((char **)(&begin));
				byte_value=CONST_FLT;
			} else
				byte_value=CONST_INT;
		}
		ch=L_CONST;
		instr=begin;
		break;

	case '"':
		begin=instr;
		while(*instr && *instr!='"') {
			if(*instr=='\\' && instr[1])
				instr++;
			instr++;
		}
		if(!*instr) {
			parse_error=NO_QUOTE;
			return ERROR;
		}
		tmp_str=new->n_x.v_string=(char *)ck_malloc(1+instr-begin);
		while(begin!=instr) {
			unsigned char n;

			if(*begin=='\\') {
				begin++;
				if(begin[0]>='0' && begin[0]<='7') {
					if(begin[1]>='0' && begin[1]<='7') {
						if(begin[2]>='0' && begin[2]<='7') {
							n=(begin[2]-'0') + (010 * (begin[1]-'0')) + ( 0100 * (begin[0]-'0'));
							begin+=3;
						} else {
							n=(begin[1]-'0') + (010 * (begin[0]-'0'));
							begin+=2;
						}
					} else {
						n=begin[0]-'0';
						begin++;
					}
				} else
					n= *begin++;
				*tmp_str++= n;
			} else
				*tmp_str++= *begin++;
		}
		*tmp_str='\0';
		instr++;
		byte_value=CONST_STR;
		ch=L_CONST;
		break;

	case '+':	case '-':

	case '*':	case '/':	case '%':	case '&':
	/* case '|': */	case '^':	case '=':

	case '?':
	{
		unsigned char *ptr;

		for(ptr= fnin;*ptr;ptr++)
			if(the_funs[*ptr].fn_str[0]==ch)
				break;
#ifdef TEST
		if(!*ptr)
			panic("Can't find fnin[] entry for '%c'",ch);
#endif
		byte_value= *ptr;
	}
		break;

	case ':':
		byte_value=IF;
		break;

	case '!':
	case '<':
	case '>':
		if(*instr!='=') {
			byte_value = (ch=='<') ? LESS : (ch=='>') ? GREATER : NOT;
			break;
		}
		instr++;
		byte_value = (ch=='<') ? LESSEQ : (ch=='>') ? GREATEQ : NOTEQUAL;
		ch = (ch=='<') ? LE : (ch=='>') ? GE : NE;
		break;

	case '\'':
	case ';':
	case '[':
	case ']':
	case '\\':
	case '`':
	case '{':
	case '}':
	case '~':
	bad_chr:
		parse_error=BAD_CHAR;
		return ERROR;

	case '#':
		begin=instr-1;
		while(*instr && (isalnum(*instr) || *instr=='_'))
			instr++;
		ch= *instr;
		*instr=0;
		if(!stricmp(begin,tname))
			byte_value=F_TRUE;
		else if(!stricmp(begin,fname))
			byte_value=F_FALSE;
		else if(!stricmp(begin,iname) && (begin[4]==0 || !stricmp(begin+4,"inity")))
			byte_value=CONST_INF;
		else if(!stricmp(begin,mname) ||
			!stricmp(begin,"#ninf"))
			byte_value=CONST_NINF;
		else if(!stricmp(begin,nname) ||
			!stricmp(begin,"#nan"))
			byte_value=CONST_NAN;
		else {
			for(n=1;n<=ERR_MAX;n++)
				if(!stricmp(begin,ename[n]))
					break;
			if(n>ERR_MAX)
				n=BAD_CHAR;
			new->n_x.v_int=n;
			byte_value=CONST_ERR;
		}
		*instr=ch;
		ch=L_CONST;
		break;

	default:
		if(!Global->a0 && (ch=='@' || ch=='$'))
		   goto bad_chr;

		if(Global->a0 && ch=='@') {
			begin=instr;
			while(*instr && (isalpha(*instr) || isdigit(*instr) || *instr=='_'))
				instr++;
			n=instr-begin;
		} else {
			begin=instr-1;
			byte_value=parse_cell_or_range(&begin,&(new->n_x.v_rng));
			if(byte_value) {
				if((byte_value& ~0x3)==R_CELL)
					ch=L_CELL;
				else
					ch=L_RANGE;
				instr=begin;
				break;
			}

			while(*instr && (isalpha(*instr) || isdigit(*instr) || *instr=='_'))
				instr++;

			n=instr-begin;
			while(isspace(*instr))
				instr++;

			if(*instr!='(') {
				ch=L_VAR;
				byte_value=VAR;
				new->n_x.v_var=find_or_make_var(begin,n);
				break;
			}
		}
		tmp_ch=begin[n];
		begin[n]='\0';
		fp=hash_find(parse_hash,begin);
		begin[n]=tmp_ch;
		byte_value= ERROR;
		if(!fp) {
			parse_error=BAD_FUNC;
			return ERROR;
		}

		if(fp>=the_funs && fp<=&the_funs[USR1])
			byte_value=fp-the_funs;
		else {
			for(nn=0;nn<n_usr_funs;nn++) {
				if(fp>=&usr_funs[nn][0] && fp<=&usr_funs[nn][usr_n_funs[nn]]) {
					byte_value=USR1+nn;
					new->sub_value=fp-&usr_funs[nn][0];
					break;
				}
			}
#ifdef TEST
			if(nn==n_usr_funs) {
				io_error_msg("Couln't turn fp into a ##");
				parse_error=BAD_FUNC;
				return ERROR;
			}
#endif
		}

		if(fp->fn_argn&X_J)
			ch= byte_value==F_IF ? L_FN3 : L_FN2;
		else if(fp->fn_argt[0]=='R' || fp->fn_argt[0]=='E')
			ch=L_FN1R-1+fp->fn_argn-X_A0;
		else
			ch=L_FN0 + fp->fn_argn-X_A0;

		break;
	}
	/* new->node_type=ch; */
	new->comp_value=byte_value;
	yylval=new;
	return ch;
}

/*
 * The return value of parse_cell_or_range() is actually
 *      0 if it doesn't look like a cell or a range,
 *      RANGE | some stuff (bitwise or) if it's a range,
 *      other bits if it's a cell
 */
/*
 * A cell is described by a string such as A0 (letter sequence followed by a number).
 * A cell range is described by two cells separated by a dot or a colon,
 *	such as A0.C3 or B5:C10.
 *
 * In absolute addressing, each part of the description is prefixed by a $ sign.
 *	Example : $A0.$C3, $A$0, or B$5:C$10.
 */
static unsigned char
a0_parse_cell_or_range (char **ptr, struct rng *retp)
{
	unsigned tmpc,tmpr;
	char *p;
	int abz = ROWREL|COLREL;

	p= *ptr;
	tmpc=0;
	if(*p=='$') {
		abz-=COLREL;
		p++;
	}
	if(!isalpha(*p))
		return 0;
	tmpc=str_to_col(&p);
	if(tmpc<MIN_COL || tmpc>MAX_COL)
		return 0;
	if(*p=='$') {
		abz-=ROWREL;
		p++;
	}
	if(!isdigit(*p))
		return 0;
	for(tmpr=0;isdigit(*p);p++)
		tmpr=tmpr*10 + *p - '0';

	if(tmpr<MIN_ROW || tmpr>MAX_ROW)
		return 0;

	if(*p==':' || *p=='.') {
		unsigned tmpc1,tmpr1;

		abz = ((abz&COLREL) ? LCREL : 0)|((abz&ROWREL) ? LRREL : 0)|HRREL|HCREL;
		p++;
		if(*p=='$') {
			abz-=HCREL;
			p++;
		}
		if(!isalpha(*p))
			return 0;
		tmpc1=str_to_col(&p);
		if(tmpc1<MIN_COL || tmpc1>MAX_COL)
			return 0;
		if(*p=='$') {
			abz-=HRREL;
			p++;
		}
		if(!isdigit(*p))
			return 0;
		for(tmpr1=0;isdigit(*p);p++)
			tmpr1=tmpr1*10 + *p - '0';
		if(tmpr1<MIN_ROW || tmpr1>MAX_ROW)
			return 0;

		if(tmpr<tmpr1) {
			retp->lr=tmpr;
			retp->hr=tmpr1;
		} else {
			retp->lr=tmpr1;
			retp->hr=tmpr;
		}
		if(tmpc<tmpc1) {
			retp->lc=tmpc;
			retp->hc=tmpc1;
		} else {
			retp->lc=tmpc1;
			retp->hc=tmpc;
		}
		*ptr= p;
		return RANGE | abz;
	}
	retp->lr = retp->hr = tmpr;
	retp->lc = retp->hc = tmpc;
	*ptr=p;
	return R_CELL | abz;
}

/*
 * Parse a numeric range such as (without the []) :
 *	[-2]
 *	[-2:-3]
 * Return 0 if it's not a range, 1 if it is.
 * "current" should be the current row or column for relative positions.
 */
static int
noa0_numeric_range(char **ptr, int *r1, int *r2, int current)
{
	int	negative = 0, num = 0;
	char	*p;

	for (p=*ptr; *p && (isdigit(*p) || *p == '-' || *p == '+'); p++)
		if (*p == '-')
			negative = 1;
		else if (*p != '+')	/* A digit */
			num = (num * 10) + *p - '0';	/* FIX ME relies on ASCII */

	if (negative)
		*r1 = current - num;
	else
		*r1 = current + num;

	num = 0; negative = 0;
	if (*p == ':' || *p == '.') {	/* it's a range */
		p++;			/* skip colon or dot */
		for (; *p && (isdigit(*p) || *p == '-' || *p == '+'); p++)
			if (*p == '-')
				negative = 1;
			else if (*p != '+')	/* A digit */
				num = (num * 10) + *p - '0';	/* FIX ME relies on ASCII */

		if (negative)
			*r2 = current - num;
		else
			*r2 = current + num;
		*ptr = p;	/* Advance pointer */
		return 1;	/* range */
	}

	*r2 = *r1;	/* both results are the same */
	*ptr = p;	/* Advance pointer */
	return 0;	/* no range */
}

static void
noa0_number(char **ptr, int *r, int current)
{
	int	negative = 0, num = 0;
	char	*p;

	for (p=*ptr; *p && (isdigit(*p) || *p == '-' || *p == '+'); p++)
		if (*p == '-')
			negative = 1;
		else if (*p != '+')	/* A digit */
			num = (num * 10) + *p - '0';	/* FIX ME relies on ASCII */

	if (negative)
		*r = current - num;
	else
		*r = current + num;

	*ptr = p;
}

static char *
noa0_find_end(char *p)
{
	while (*p && (isdigit(*p) || *p == '+' || *p == '-'))
		p++;
	return p;
}

/*
 * Taking relative as well as absolute coordinates into account,
 * potentially swap the order of the range.
 */
static int
SwapEm(int bits, struct rng *retp)
{
	signed short	retbits, r1, r2, c1, c2, x;

	r1 = retp->lr;
	r2 = retp->hr;
	c1 = retp->lc;
	c2 = retp->hc;

	retbits = bits & ~(LRREL|HRREL|LCREL|HCREL);
	if (r1 > r2) {
		x = retp->lr;
		retp->lr = retp->hr;
		retp->hr = x;
		if (LRREL) retbits |= HRREL;
		if (HRREL) retbits |= LRREL;
	} else {
		retbits |= (bits & (LRREL|HRREL));
	}
	if (c1 > c2) {
		x = retp->lc;
		retp->lc = retp->hc;
		retp->hc = x;
		if (LCREL) retbits |= HCREL;
		if (HCREL) retbits |= LCREL;
	} else {
		retbits |= (bits & (LCREL|HCREL));
	}
	return retbits;
}

/*
 * The new implementation of parse_cell_or_range can read non-A0
 *	in SYLK format as well.
 *
 * A single cell is described by R<number>C<number>.
 * A range is described either by :
 *	(Oleo native)	R<number range>C<number range>
 * or
 *	(SYLK)		<cell>:<cell>
 *
 * In absolute addressing, a number is just that.
 * In relative addressing, it is "["<number>"]".
 *	If the relative offset is 0 then it and the brackets can be omitted,
 *	so "rc3" really is a row-relative address.
 *
 * Examples :
 *	(Oleo)		RC[-2:-3]
 *	(SYLK)		RC[-2]:RC[-3]
 */
static unsigned char
noa0_parse_cell_or_range(char **ptr, struct rng *retp)
{
	int		rrange = 0, crange = 0;		/* Are R or C ranges ? */
							/* Are R or C relative ? */
	int		r1rel = 0, c1rel = 0, r2rel = 0, c2rel = 0;
	char		*p, *next;
	int		r1, r2, c1, c2, ret, forget;

	p = *ptr;
	r1 = r2 = cur_row;
	c1 = c2 = cur_col;

	if (*p != 'r' && *p != 'R')
		return 0;

	/* Grab whatever's after the first R */
	p++;
	switch (*p) {
	case 'c':	/* Relative with no row offset */
	case 'C':
		r1 = r2 = cur_row;
		r1rel = r2rel = 1;
		rrange = 0;
		break;
	case '+': case '-':
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	case '8': case '9':
		rrange = noa0_numeric_range(&p, &r1, &r2, 0);
		break;
	case '[':
		p++;
		r1rel = r2rel = 1;
		rrange = noa0_numeric_range(&p, &r1, &r2, cur_row);
		if (*p != ']')
			return 0;	/* Invalid string */
		p++;
		break;
	default:
		return 0;	/* Invalid string */
	}

	/* If this is a bracket then we have a mixed absolute/relative range */
	if (*p == '[') {
		p++;	/* Skip [ */
		r2rel = 1;
		noa0_number(&p, &r2, 0);
		r2 += cur_row;
		p++;	/* Skip ] */
	}

	/* Need to find a C now */
	if (*p != 'c' && *p != 'C')
		return 0;	/* Invalid string */

	/* Read whatever's behind the C */
	p++;
	switch (*p) {
	case '.':	/* SYLK style range */
	case ':':
		c1 = c2 = cur_col;
		c1rel = c2rel = 1;
		break;
	case '+': case '-':
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	case '8': case '9':
		next = noa0_find_end(p);
		if (*next == '.' || *next == ':') {
			if (*(next+1) == 'r' || *(next+1) == 'R') {
				/* SYLK range; only read one number here */
				noa0_number(&p, &c1, 0);
				break;
			} else if (*(next+1) == '[') {
				noa0_number(&p, &c1, 0);
				p = next+2;
				noa0_number(&p, &c2, cur_col);
				p++;	/* Skip ] */
				c2rel = 1;

				*ptr = p;
				ret = 0;

				retp->lr = r1;
				retp->hr = r2;
				retp->lc = c1;
				retp->hc = c2;

				if (rrange || crange)
					ret |= RANGE;
				else
					ret |= R_CELL;
				if (c1rel)
					ret |= LCREL;
				if (r1rel)
					ret |= LRREL;
				if (c2rel)
					ret |= HCREL;
				if (r2rel)
					ret |= HRREL;

				return SwapEm(ret, retp);
			}
		}
		crange = noa0_numeric_range(&p, &c1, &c2, 0);
		break;
	case '[':
		p++;
		c1rel = 1;
		crange = noa0_numeric_range(&p, &c1, &c2, cur_col);
		if (*p != ']')
			return 0;	/* Invalid string */
		p++;
		break;
	default:
		c1 = c2 = cur_col;
		c1rel = c2rel = 1;
#if 0
		*ptr = p;
		if (rrange || crange)
			return RANGE;	/* FIX ME */
		else
			return R_CELL;	/* FIX ME */
#endif
	}

	if (*p != ':' && *p != '.') {	/* Would be SYLK ranges */
		*ptr = p;
		ret = 0;

		retp->lr = r1;
		retp->hr = r2;
		retp->lc = c1;
		retp->hc = c2;

		if (rrange || crange)
			ret |= RANGE;
		else
			ret |= R_CELL;
		if (c1rel)
			ret |= LCREL;
		if (r1rel)
			ret |= LRREL;
		if (c2rel)
			ret |= HCREL;
		if (r2rel)
			ret |= HRREL;

		return SwapEm(ret, retp);
	}

	/* So now it's a SYLK range */
	p++;				/* Skip . or : */

	if (*p != 'r' && *p != 'R')	/* Need to find second R */
		return 0;

	/* Grab whatever's after the second R */
	p++;
	switch (*p) {
	case 'c':	/* Relative with no row offset */
	case 'C':
		r2 = cur_row;
		r2rel = 1;
		rrange = (r1 == r2) ? 0 : 1;
		break;
	case '+': case '-':
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	case '8': case '9':
		rrange = noa0_numeric_range(&p, &r2, &forget, 0);
		break;
	case '[':
		p++;
		r1rel = r2rel = 1;
		rrange = noa0_numeric_range(&p, &r2, &forget, cur_row);
		if (*p != ']')
			return 0;	/* Invalid string */
		p++;
		break;
	default:
		return 0;	/* Invalid string */
	}

	/* Need to find second C now */
	if (*p != 'c' && *p != 'C')
		return 0;	/* Invalid string */

	/* Read whatever's behind the C */
	p++;
	switch (*p) {
	case '.':	/* SYLK style range */
	case ':':
		c2 = cur_col;
		c1rel = c2rel = 1;
		crange = (c1 == c2) ? 0 : 1;
		break;
	case '+': case '-':
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	case '8': case '9':
		crange = noa0_numeric_range(&p, &c2, &forget, 0);
		break;
	case '[':
		p++;
		c2rel = 1;
		crange = noa0_numeric_range(&p, &c2, &forget, cur_col);
		if (*p != ']')
			return 0;	/* Invalid string */
		p++;
		break;
	default:
		c2 = cur_col;
		c1rel = c2rel = 1;
	}

	*ptr = p;
	ret = RANGE;

	retp->lr = r1;
	retp->hr = r2;
	retp->lc = c1;
	retp->hc = c2;

	if (c1rel)
		ret |= LCREL;
	if (c2rel)
		ret |= HCREL;
	if (r1rel)
		ret |= LRREL;
	if (r2rel)
		ret |= HRREL;

	return SwapEm(ret, retp);
}

/*
 * The return value of parse_cell_or_range() is
 *      0			if it doesn't look like a cell or a range,
 *      RANGE | bits		if it's a range,
 *      R_CELL | other bits	if it's a cell
 *
 * These macros are defined in eval.h, their meaning is :
 *	R_CELL	(12)		this is a single cell
 *		ROWREL	(1)	the row is relative
 *		COLREL	(2)	the column is relative
 *	RANGE	(16)		this is a range
 *		LRREL	(1)	low row is relative
 *		HRREL	(2)	high row is relative
 *		LCREL	(4)	low column is relative
 *		HCREL	(8)	high column is relative
 */
unsigned char
parse_cell_or_range(char **ptr, struct rng *retp)
{
	unsigned char	r;

	if (Global->a0)
		r = a0_parse_cell_or_range(ptr, retp);
	else
		r = noa0_parse_cell_or_range(ptr, retp);
#if 0
	/*
	 * Use this to print out whatever parse_cell_or_range() does.
	 */
	fprintf(stderr, "parse_cell_or_range(%s) -> [%d..%d, %d..%d], %d\n",
		*ptr, retp->lr, retp->hr, retp->lc, retp->hc, r);
	fprintf(stderr, "parse_cell_or_range -> remaining string is '%s'\n", *ptr);
#endif
	return r;
}

int
str_to_col (char **str)
{
	int ret;
	char c,cc,ccc;
#if MAX_COL>702
	char cccc;
#endif

	ret=0;
	c=str[0][0];
	if(!isalpha((cc=str[0][1]))) {
		(*str)++;
		return MIN_COL + (isupper(c) ? c-'A' : c-'a');
	}
	if(!isalpha((ccc=str[0][2]))) {
		(*str)+=2;
		return MIN_COL+26 + (isupper(c) ? c-'A' : c-'a')*26 + (isupper(cc) ? cc-'A' : cc-'a');
	}
#if MAX_COL>702
	if(!isalpha((cccc=str[0][3]))) {
		(*str)+=3;
		return MIN_COL+702 + (isupper(c) ? c-'A' : c-'a')*26*26 + (isupper(cc) ? cc-'A' : cc-'a')*26 + (isupper(ccc) ? ccc-'A' : ccc-'a');
	}
	if(!isalpha(str[0][4])) {
		(*str)+=4;
		return MIN_COL+18278 + (isupper(c) ? c-'A' : c-'a')*26*26*26 + (isupper(cc) ? cc-'A' : cc-'a')*26*26 + (isupper(ccc) ? ccc-'A' : ccc-'a')*26 + (isupper(cccc) ? cccc-'A' : cccc-'a');
	}
#endif
	return 0;
}
