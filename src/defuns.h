/*
 * $Id: defuns.h,v 1.3 2000/07/22 06:13:15 danny Exp $
 *
 * Copyright © 1993, 2000 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* This file repeatedly includes the contents of defun.h which contains
 * doc strings and FUNC_ARGS structures for all of the interactive
 * built-ins.
 * 
 * It uses ugly preprocessor magic to build various tables.
 */


/* First, build arrays containing FUNC_ARG strings. */

#define DEFUN_5(Sname, FDname, FAname, DFname, Cname) \
0, \
}; \
\
static char * FAname [] = \
{

#define DEFUN(Sname, FDname, FAname, Cname) \
	DEFUN_5(Sname, FDname, FAname, 0, Cname)

#define DEFUN_3(Sname, FDname, Cname)

#define FUNC_ARG_STRINGS	1

static char * FAfnord[] = 
{
#include "defun.h"
0,
};

/* Docstrings */

#undef DEFUN
#undef DEFUN_3
#undef DEFUN_5
#undef FUNC_ARG_STRINGS
#undef DOC_STRINGS
#undef FUNC_INIT_CODE

#define DEFUN_5(Sname, FDname, FAname, DFname, Cname) \
0, \
}; \
\
static char * FDname [] = \
{

#define DEFUN(Sname, FDname, FAname, Cname) \
	DEFUN_5(Sname, FDname, FAname, 0, Cname)
#define DEFUN_3(Sname, FDname, Cname) DEFUN(Sname, FDname, 0, Cname)

#define DOC_STRINGS	1

static char * FDfnord[] = 
{
  "Well, you know.",
#include "defun.h"
0,
};


/* init_code */

#undef DEFUN
#undef DEFUN_3
#undef DEFUN_5
#undef FUNC_ARG_STRINGS
#undef DOC_STRINGS
#undef FUNC_INIT_CODE

#define DEFUN_5(Sname, FDname, FAname, DFname, Cname) \
0, \
}; \
\
static char * DFname [] = \
{

#define DEFUN(Sname, FDname, FAname, Cname)
#define DEFUN_3(Sname, FDname, Cname) 

#define FUNC_INIT_CODE	1

static char * DFfnord[] = 
{
#include "defun.h"
0,
};



/* Building the function table. */

#undef DEFUN
#undef DEFUN_3
#undef DEFUN_5
#undef FUNC_ARG_STRINGS
#undef DOC_STRINGS
#undef FUNC_INIT_CODE

#define DEFUN_5(Sname, FDname, FAname, DFname, Cname) \
	{ Sname, FDname, FAname, DFname, Cname },

#define DEFUN(Sname, FDname, FAname, Cname) \
	DEFUN_5(Sname, FDname, FAname, 0, Cname)
#define DEFUN_3(Sname, FDname, Cname) \
	DEFUN_5(Sname, FDname, 0, 0, Cname)

struct cmd_func cmd_funcs[] =
{
  { "fnord", FDfnord, FAfnord, DFfnord, fnord },
#include "defun.h"
  { 0, 0, 0, 0, 0 }
};

