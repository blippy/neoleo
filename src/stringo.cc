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
#include "funcdef.h"
#define obstack_chunk_alloc ck_malloc
#define obstack_chunk_free free
#include "obstack.h"
#include "sysdef.h"

#include "funcs.h"
#include "global.h"
#include "cell.h"
#include "eval.h"
#include "errors.h"
#include "stringo.h"
#include "io-utils.h"
#include "lists.h"


#define Float	x.c_n
#define Value	x.c_i
#define Rng	x.c_r

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



static std::string do_repeat(struct value* p)
{
	char* str = p->gString();
	long num  = (p+1)->gLong();
	if(num<0) throw OUT_OF_RANGE;
	std::string res;
	while(num--) res += str;
	return res;
}
static void do_repeat_1(struct value* p) { wrapfunc(do_repeat)(p); }



static void
do_len(struct value * p)
{
	long ret;
	char *ptr;

	for(ret=0,ptr=p->gString();*ptr;ret++,ptr++)
		;
	p->sLong(ret);
	//p->type=TYP_INT;
}



static void
do_up_str(struct value * p)
{
	char *s1,*s2;
	char *strptr;

	strptr=alloc_value_str(p);
	for(s1=strptr,s2=p->gString();*s2;s2++)
		*s1++ = (islower(*s2) ? toupper(*s2) : *s2);
	*s1=0;
	p->sString(strptr);
}



static void
do_dn_str(struct value * p)
{
	char *s1,*s2;
	char *strptr;

	strptr=alloc_value_str(p);
	for(s1=strptr,s2=p->gString();*s2;s2++)
		*s1++ = (isupper(*s2) ? tolower(*s2) : *s2);
	*s1=0;
	p->sString(strptr);
}



static void
do_cp_str(struct value * p)
{
	char *strptr;
	char *s1,*s2;
	int wstart=1;

	strptr=alloc_value_str(p);
	for(s1=strptr,s2=p->gString();*s2;s2++) {
		if(!isalpha(*s2)) {
			wstart=1;
			*s1++= *s2;
		} else if(wstart) {
			*s1++ = (islower(*s2) ? toupper(*s2) : *s2);
			wstart=0;
		} else
			*s1++ = (isupper(*s2) ? tolower(*s2) : *s2);
	}
	*s1=0;
	p->sString(strptr);
}



static void
do_trim_str(struct value * p)
{
	char *s1,*s2;
	int sstart=0;
	char *strptr;

	strptr=alloc_value_str(p);
	for(s1=strptr,s2=p->gString();*s2;s2++) {
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
	char *str = p->gString();
	long from = (p+1)->gLong()-1;
	long len =  (p+2)->gLong();

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


static void
do_substr(struct value * p)
{
	long off1 = (p  )->gLong();
	long off2 = (p+1)->gLong();
	char *str = (p+2)->gString();

	char	*ptr1,	*ptr2;
	int tmp;
	char *ret;

	tmp=strlen(str);
	if(off1==0 || tmp < ((off1<0) ? -off1 : off1) ||
	   off2==0 || tmp < ((off2<0) ? -off2 : off2))
		ERROR(OUT_OF_RANGE);
	ptr1=str + (off1>0 ? off1-1 : tmp+(off1));
	ptr2=str + (off2>0 ? off2-1 : tmp+(off2));

	if(ptr1>ptr2)
		ERROR(OUT_OF_RANGE);
	tmp=(ptr2-ptr1)+1;
	ret=(char *)obstack_alloc(&tmp_mem,tmp+1);
	strncpy(ret,ptr1,tmp);
	ret[tmp]=0;
	p->sString(ret);
}



static void
do_strstr(struct value * p)
{
	char *strptr	= p->gString();
	char *str1	= (p+1)->gString();
	long off	= (p+2)->gLong();
	//char *ret;

	if(off<1 || strlen(strptr)<=off-1)
		ERROR(OUT_OF_RANGE);
	//ret=(char *)strstr(strptr+off-1,str1);
	char *loc = strstr(strptr+off-1,str1);
	size_t pos = loc ? 1 + loc - strptr : 0;
	p->sLong(pos);
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
	int pos = (p+1)->gLong() -1;
	int len = (p+2)->gLong() - pos;
	return s1.erase(pos, len);
}
static void do_edit_1(struct value*p) { wrapfunc(do_edit)(p);}




struct function string_funs[] = {
{ C_FN1,	X_A1,	"S",    to_vptr(do_len),	"len" }, 
{ C_FN3,	X_A3,	"SSI",  to_vptr(do_strstr),	"find" }, 

{ C_FN1,	X_A1,	"S",    to_vptr(do_up_str),	"strupr" }, 
{ C_FN1,	X_A1,	"S",    to_vptr(do_dn_str),	"strlwr" }, 
{ C_FN1,	X_A1,	"S",    to_vptr(do_cp_str),	"strcap" }, 
{ C_FN1,	X_A1,	"S",    to_vptr(do_trim_str),	"trim" }, 
{ C_FN3,	X_A3,	"IIS",  to_vptr(do_substr),	"substr" }, 
{ C_FN3,	X_A3,	"SII",  to_vptr(do_mid),	"mid" }, 
{ C_FN2,	X_A2,	"SI",   to_vptr(do_repeat_1),	"repeat" }, 
{ C_FN3,	X_A3,	"SII", to_vptr(do_edit_1),	"edit" }, 
{ C_FN0,        X_A0,   "",    to_vptr(do_version_1),    "version" },
{ C_FN2,        X_A2,   "SS",    to_vptr(do_concata_1),    "concata" },

{ 0,		0,	{0},	0,		0 }
};

int init_string_function_count(void) 
{
        return sizeof(string_funs) / sizeof(struct function) - 1;
}
