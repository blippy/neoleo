/*
 * $Id: string.c,v 1.5 2000/08/10 21:02:51 danny Exp $
 *
 * Copyright © 1990, 1992, 1993 Free Software Foundation, Inc.
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

#include "config.h"

#include <functional>
#include <string.h>
#include <ctype.h>
#define obstack_chunk_alloc ck_malloc
#define obstack_chunk_free free
#include "obstack.h"

#include "funcs.h"
#include "global.h"
#include "cell.h"
#include "eval.h"
#include "errors.h"
#include "mem.h"
#include "stringo.h"
#include "io-utils.h"
#include "sheet.h"


#define Value	x.c_i

#define ERROR(x)	\
 {			\
	p->Value=x;	\
	p->type=TYP_ERR;\
	return;		\
 }
	

extern struct obstack tmp_mem;

char *alloc_tmp_mem(size_t n)
{
	return (char *) obstack_alloc(&tmp_mem, n);
}

char * alloc_value_str(struct value* p)
{
	return alloc_tmp_mem(strlen(p->gString())+1);
}


std::function<void(struct value*)>
wrapfunc(std::function<std::string(struct value*)> func)
{
	auto fn = [=](struct value* p) { 
		try {
			std::string s = func(p);
			char* ret = alloc_tmp_mem(s.size()+1);		
			strcpy(ret,  s.c_str());
			p->sString(ret);
		} catch (int e) {
			p->Value = e;
			p->type = TYP_ERR;
		}
		return; 
	};
	return fn;
}





static void
do_len(struct value * p)
{
	p->sInt(strlen(p->gString()));
}






static void
do_trim_str(struct value * p)
{
	char *s1,*s2;
	int sstart=0;
	char *strptr;

	strcpy_c s2c{p->gString()};
	strptr=alloc_value_str(p);
	for(s1=strptr,s2=s2c.data();*s2;s2++) {
		if(!isascii(*s2) || !isprint(*s2))
			continue;
		if(*s2==' ') {
				if(sstart) {
				*s1++= *s2;
				sstart=0;
			}
		} else {
			sstart=1;
			*s1++= *s2;
		}
	}
	*s1=0;
	p->sString(strptr);
}


static void
do_mid(struct value * p)
{
	const char *str = p->gString();
	long from = (p+1)->gInt()-1;
	long len =  (p+2)->gInt();

	char	*ptr1;
	int tmp;

	tmp=strlen(str);

	if(from<0 || len<0)
		ERROR(OUT_OF_RANGE);
	ptr1=(char *)obstack_alloc(&tmp_mem,len+1);
	if(from>=tmp || len==0)
		ptr1[0]='\0';
	else {
		strncpy(ptr1,str+from,len);
		ptr1[len]='\0';
	}
	p->sString(ptr1);
}








// Mostly for testing purposes for a better way of wrapping functions
static std::string do_version(struct value* p)
{
	return VERSION;
}
static void do_version_1(struct value* p) { wrapfunc(do_version)(p); }



static std::string do_concata(struct value* p)
{
	std::string s1 = (p)->gString();
	std::string s2 = (p+1)->gString();
	return s1+s2;
}
static void do_concata_1(struct value* p) { wrapfunc(do_concata)(p); }



static std::string do_edit(struct value* p)
{
	std::string s1 = p->gString();
	int pos = (p+1)->gInt() -1;
	int len = (p+2)->gInt() - pos;
	return s1.erase(pos, len);
}
static void do_edit_1(struct value*p) { wrapfunc(do_edit)(p);}




function_t string_funs[] = {
{ C_FN1,	X_A1,	"S",    to_vptr(do_len),	"len" }, 
{ C_FN1,	X_A1,	"S",    to_vptr(do_trim_str),	"trim" }, 
{ C_FN3,	X_A3,	"SII",  to_vptr(do_mid),	"mid" }, 
{ C_FN3,	X_A3,	"SII", to_vptr(do_edit_1),	"edit" }, 
{ C_FN0,        X_A0,   "",    to_vptr(do_version_1),    "version" },
{ C_FN2,        X_A2,   "SS",    to_vptr(do_concata_1),    "concata" },

{ 0,		0,	{0},	0,		0 }
};

int init_string_function_count(void) 
{
        return sizeof(string_funs) / sizeof(function_t) - 1;
}
