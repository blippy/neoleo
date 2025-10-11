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

blang_expr_t eval_set_cell(blang_exprs_t args)
{
	int row = to_num(eval(args[0]));
	if(row == 0) row = curow;
	int col = to_num(eval(args[1]));
	if(col == 0) col = cucol;
	auto val  = to_string(eval(args[2]));
	set_and_eval(row, col, val, false);

	/*
	for(const auto& a : args) {
		cout << to_string(eval(a));
	}
	cout << endl;
*/
	return std::monostate{};
}


void blx_init()
{
	blang_funcmap["set_cell"] = &eval_set_cell;
}


