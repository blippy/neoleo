/*
 * lex.c - lexical analyzer to feed tokens to yyparse, and related
 * routines for parsing cell ranges.
 *
 * $Id: lex.c,v 1.1 2001/04/16 00:16:21 pw Exp $
 *
 * Copyright (C) 2001 Free Software Foundation, Inc.
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
#include <stdlib.h>
#include <ctype.h>
#include "global.h"
#define obstack_chunk_alloc ck_malloc
#define obstack_chunk_free free
#include "obstack.h"
#include "node.h"
#include "eval.h"
#include "errors.h"
#include "ref.h"
/* generated from parse.y, declare what type we'll use it as */
#define YYSTYPE struct node *
#include "parse.h"

#define ERROR -1

/* from parse.y */
extern int parse_error;

/* declare the hash tree, and the external function we call */
VOIDSTAR parse_hash;
extern VOIDSTAR hash_find(VOIDSTAR handle, char *string);

/*
 * Callers of yyparse() will hang the input string here.
 */
char *instr;

/* This table contains a list of the infix single-char functions */
static unsigned char fnin[] = {
    SUM, DIFF, DIV, PROD, MOD, /* AND, OR, */ POW, EQUAL, IF, CONCAT, 0
};

int
yylex(void)
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
		fp=(struct function *)hash_find(parse_hash,begin);
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
 * a0 helper
 */
static int
a0_str_to_col(char **str)
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
	tmpc=a0_str_to_col(&p);
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
		tmpc1=a0_str_to_col(&p);
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
 * Parse a cell or range, returning which one, or invalid (-1).
 * Handles mixed relative/absolute ranges, separated by a : or .
 * The last argument is to catch separators where they shouldn't
 * be.  If possible_trailing_sep, we're parsing the C part of a
 * (so-far) cell.  The cell may turn out to be a range in which case
 * no trailing separator should be there, or if it is a cell, return
 * with the : intact.
 *
 * This requires lookahead:
 *    r3c5: could be r3c5:7 or r3c5:r4c6
 */
static int noa0_cell_or_range(char **ptr, int *lo, int *hi,
  int *lorel, int *hirel, int curpos, int possible_trailing_sep)
{
    char *p = *ptr, *endp;
    int in_relative = 0;
    int need_sep = 0;
    int num_parsed = 0;
    long val;

    for (;;) switch (*p) {
	case '+': case '-': case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7': case '8': case '9':
	    if (need_sep)
		return ERROR;
	    val = strtol(p, &endp, 10);
	    p = endp;
	    if (in_relative)
		val += curpos;
	    if (num_parsed == 0) {
		*lo = val;
		*lorel = in_relative;
		need_sep = 1;
	    } else if (num_parsed == 1) {
		*hi = val;
		*hirel = in_relative;
	    } else
		return ERROR;
	    ++num_parsed;
	    break;
	case '[':
	    ++p;
	    if (need_sep)
		return ERROR;
	    if (in_relative)
		return ERROR;
	    in_relative = 1;
	    break;
	case ']':
	    ++p;
	    if (!in_relative)
		return ERROR;
	    /* need_sep remains */
	    in_relative = 0;
	    break;
	case ':': case '.':
	    if (!need_sep) {
		/* case r1c:r3c5 */
		/* case r1c5:7:r2c2 err caught elsewhere */
		if (possible_trailing_sep)
		    goto out;
		return ERROR;
	    }
	    /* case r1c5:r4c8 */
	    if (possible_trailing_sep && tolower(*(p+1)) == 'r')
		goto out;
	    ++p;
	    need_sep = 0;
	    break;
	default:
	    goto out;
    }
  out:
    if (in_relative)
	return ERROR;
    *ptr = p;
    if (num_parsed == 0) {  /* empty -> [0] */
	*lo = curpos;
	*lorel = 1;
	return 0;
    } else if (num_parsed == 1)
	return 0;
    else
	return 1;  /* a range */
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
 * Note that . can be used in place of : anywhere.
 *
 * Examples :
 *	(Oleo)		RC[-2:-3]
 *	(SYLK)		RC[-2]:RC[-3]
 *
 *  Oleo parsing:
 *  cells:
 *	r1c2
 *	r[1]c2
 *	r1c[2]
 *	r[1]c[2]
 *	(shorthand)
 *	rc* -> r[0]c*
 *	r*c -> r*c[0]
 *  ranges:
 *	r1:3c2
 *	r[1]:3c2
 *	r1:[3]c2
 *	r[1]:[3]c2
 *	r[1:3]c* -> r[1]:[3]c*
 *	(similar for c side)
 *
 * Sylk parsing is easier:
 *   <cell> from above
 *   <cell>:<cell>
 *
 * Note ambiguity:
 *   r2c3:r5c is sylk
 *   r2c3:4:r5c is not sylk
 */
#define SWAP(a,b) do { int _x = a; a = b; b = _x; } while (0)

static unsigned char
noa0_parse_cell_or_range(char **ptr, struct rng *retp)
{
    int rrange, crange;
    int rlo, rhi, clo, chi;
    int rlorel, clorel, rhirel, chirel;
    char *p;
    int ret, junk;

    p = *ptr;
    if (tolower(*p) != 'r')
	return 0;
    p++;
    rrange = noa0_cell_or_range(&p, &rlo, &rhi, &rlorel, &rhirel, cur_row, 0);
    if (rrange == ERROR || tolower(*p) != 'c')
	return 0;
    p++;
    crange = noa0_cell_or_range(&p, &clo, &chi, &clorel, &chirel, cur_col,
      !rrange);
    if (crange == ERROR)
	return 0;

    if (!rrange && !crange && (*p == ':' || *p == '.')) {
	/* sylk-style */
	p++;
	if (tolower(*p) != 'r')
	    return 0;
	p++;
	if (noa0_cell_or_range(&p, &rhi, &junk, &rhirel, &junk, cur_row, 0))
	    return 0;
	if (tolower(*p) != 'c')
	    return 0;
	p++;
	if (noa0_cell_or_range(&p, &chi, &junk, &chirel, &junk, cur_col, 0))
	    return 0;
	rrange = crange = 1;
    }
    *ptr = p;

    /*
     * Convert results to internal format.
     */
    if (!rrange && !crange) {
	ret = R_CELL;
	retp->lr = retp->hr = rlo;
	retp->lc = retp->hc = clo;
	if (rlorel) ret |= ROWREL;
	if (clorel) ret |= COLREL;
    } else {
	ret = RANGE;
	if (!rrange) {  /* in case one wasn't a range, fill hi vals */
	    rhi = rlo;
	    rhirel = rlorel;
	} else if (rlo > rhi) {  /* maybe swap to order them */
	    SWAP(rlo,rhi);
	    SWAP(rlorel,rhirel);
	}
	if (!crange) {
	    chi = clo;
	    chirel = clorel;
	} else if (clo > chi) {
	    SWAP(clo,chi);
	    SWAP(clorel,chirel);
	}
	retp->lr = rlo;
	retp->hr = rhi;
	retp->lc = clo;
	retp->hc = chi;
	if (rlorel) ret |= LRREL;
	if (rhirel) ret |= HRREL;
	if (clorel) ret |= LCREL;
	if (chirel) ret |= HCREL;

    }
    return ret;
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

