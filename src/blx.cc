/*
 * blx.cc
 *
 *  blang "extras". Separated from blang2 because blang2 is a generic language
 *  This file contains neoleo extensions
 *  Created on: 11 Oct 2025
 *      Author: pi
 */


#include "blang2.h"
#include "blx.h"
#include "neotypes.h"
#include "parser-2019.h"

using namespace std;

blang_expr_t eval_bind(blang_exprs_t args)
{
	log("eval_bind  called");
	char trigger = to_string(eval(args[0]))[0];
	log("trigger is ", string{trigger});
	string code = to_string(eval(args[1]));
	log("code is ", code);
	bind_char(trigger, code);
	return std::monostate{};

}

blang_expr_t eval_set_cell (blang_exprs_t args)
{
	int row = to_num(eval(args[0]));
	if(row == 0) row = curow;
	int col = to_num(eval(args[1]));
	if(col == 0) col = cucol;
	auto val  = to_string(eval(args[2]));
	set_and_eval(row, col, val, false);

	return std::monostate{};
}


void blx_init()
{
	blang_funcmap["bind"] = &eval_bind;
	blang_funcmap["set_cell"] = &eval_set_cell;
}


