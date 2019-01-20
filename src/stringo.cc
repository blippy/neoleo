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

std::function<void(struct value*)>
wrapfunc(std::function<std::string(struct value*)> func)
{
	auto fn = [=](struct value* p) { 
		try {
			std::string s = func(p);
			p->sString(s.c_str());
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
{ C_FN3,	X_A3,	"SII", to_vptr(do_edit_1),	"edit" }, 
{ C_FN0,        X_A0,   "",    to_vptr(do_version_1),    "version" },
{ C_FN2,        X_A2,   "SS",    to_vptr(do_concata_1),    "concata" },

{ 0,		0,	{0},	0,		0 }
};

int init_string_function_count(void) 
{
        return sizeof(string_funs) / sizeof(function_t) - 1;
}
