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


#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

/* This file repeatedly includes the contents of defun.h which contains
 * doc strings and FUNC_ARGS structures for all of the interactive
 * built-ins.
 * 
 * It uses ugly preprocessor magic to build various tables.
 */


/* First, build arrays containing FUNC_ARG strings. */

#include "global.h"
#include "cmd.h"
#include "key.h"
#include "io-term.h"
#include "basic.h"
#include "format.h"
#include "io-curses.h"
#include "io-edit.h"
#include "regions.h"
#include "spans.h"
#include "window.h"
#include "sheet.h"
#include "tbl.h"


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

/*
 * mcarter 22-Apr-2018
 * Note that FDname is never used as it relates to help documents which have
 * been purged from the system recently.
 */
#define DEFUN_5(Sname, FDname, FAname, DFname, Cname) \
	{ Sname, FAname, DFname, to_vptr(Cname), "\"" #Cname "\"" },

#define DEFUN(Sname, FDname, FAname, Cname) \
	DEFUN_5(Sname, FDname, FAname, 0, Cname)
#define DEFUN_3(Sname, FDname, Cname) \
	DEFUN_5(Sname, FDname, 0, 0, Cname)

struct cmd_func cmd_funcs[] =
{
  { "fnord", FAfnord, DFfnord, to_vptr(fnord), "fnord" },
#include "defun.h"
  { 0, 0, 0, to_vptr(0), 0 }
};



struct cmd_func** get_cmd_funcs()
{
	return (struct cmd_func**) &cmd_funcs[0];
}


////////////////////////////////////////////////////////////////////////////////////////////
// This should be a "one-time" operation to bail us out of the mess that is
// defun.h and defuns.cc
// Call it from headless mode using rewrite-defuns

/* struct cmd_func defined in funcs.h 
 * To create the file and compile it you would do:
  	neoleo -H <<<rewrite-defuns ; g++ -std=c++17 -c defuns-1.cc
 */

void rewrite_defuns()
{
	cout << "writing file defuns-1.cc\n";

	std::ofstream sout;
	sout.open("defuns-1.cc");

	std::string hdr = R"(
#include "defuns-1.h"
#include "alt-parse.h"
#include "basic.h"
#include "cell.h"
#include "cmd.h"
#include "help.h"
#include "io-curses.h"
#include "io-edit.h"
#include "io-term.h"
#include "lists.h"
#include "regions.h"
#include "tbl.h"

using namespace std::literals;

cmd_func_map cmd_funcs_1;

void make_defun_funcs()
{

)";
	sout << hdr;

	int num_funcs = 0;
	for(int i = 1; cmd_funcs[i].func_name != 0 ; ++i) num_funcs++;
	num_funcs++;
	cout << "Number of funcs detected:" << num_funcs << "\n";
	//num_funcs = num_funcs; // for overwriting

	for(int i = 1 ; i< num_funcs ; ++i) { // skipping fnord
		//if(i>1) sout << ",\n";
		sout << "// function number " << i << "\n";
		struct cmd_func cmd;
		cmd = cmd_funcs[i];
		sout << "cmd_funcs_1[\"" << cmd.func_name << "\"sv] = {  \n";
		std::string func_name(cmd.cpp_func_name);
		sout << "\tto_vptr(" << func_name.substr(1, func_name.size() -2) << "),\n";
		//sout << "\t" << cmd.func_name << ",";
		sout << "\t" << cmd.cpp_func_name << "sv\n";
		sout << "};\n\n";

	}
	sout << "}\n";
	sout.close();


}
