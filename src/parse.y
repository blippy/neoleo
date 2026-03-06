%{
/*
 * $Id: parse.y,v 1.12 2001/04/16 00:16:21 pw Exp $
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
#include <stdlib.h>  /* malloc, free used by bison */
#include "funcdef.h"

#define obstack_chunk_alloc ck_malloc
#define obstack_chunk_free free
#include "obstack.h"
#include "sysdef.h"

#include "global.h"
#include "errors.h"
#include "node.h"
#include "eval.h"

extern int yylex(void);
void yyerror(char *s);

#define YYSTYPE _y_y_s_t_y_p_e
typedef struct node *YYSTYPE;
static YYSTYPE make_list(const YYSTYPE car, const YYSTYPE cdr);

int parse_error = 0;  /* lex.c will put errors here too */

/*
 * Interface to byte-compile.c
 */
extern struct obstack tmp_mem;
YYSTYPE parse_return;

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
yyerror(char *s)
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

