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
#include "sheet.h"

using namespace std;
using namespace blang;

static blang_expr_t eval_bind(blang_exprs_t args)
{
	log("eval_bind  called");
	char trigger = blang_to_string(eval(args[0]))[0];
	log("trigger is ", string{trigger});
	string code = blang_to_string(eval(args[1]));
	log("code is ", code);
	bind_char(trigger, code);
	return std::monostate{};

}

static blang_expr_t eval_set_cell (blang_exprs_t args)
{
	int row = blang_to_num(eval(args[0]));
	if(row == 0) row = curow;
	int col = blang_to_num(eval(args[1]));
	if(col == 0) col = cucol;
	auto val  = blang_to_string(eval(args[2]));
	set_and_eval(row, col, val, false);

	return std::monostate{};
}


#if 1
// theoretically correct, but no bol type, unfortunately
template <typename ToVariant, typename FromVariant>
ToVariant ConvertVariant(const FromVariant& from) {
    ToVariant to = std::visit([](auto&& arg) -> ToVariant {return arg ; }, from);
    return to;
}
#endif

static blang_expr_t to_blang_expr(const value_t& val)
{
	//return 13;
	if(holds_alternative<monostate>(val)) return monostate{};
	if(auto v = std::get_if<num_t>(&val)) 	return (*v);
	if(auto v = std::get_if<string>(&val)) 	return (*v);
	if(auto v = std::get_if<bool_t>(&val)) 	return (v->v);
	if(auto v = std::get_if<err_t>(&val)) 	return v->what;

	throw std::logic_error("Unhandled variant in to_blang_expr");

}

static blang_expr_t eval_get_cell (blang_exprs_t args)
{
	int row = blang_to_num(eval(args[0]));
	if(row == 0) row = curow;
	int col = blang_to_num(eval(args[1]));
	if(col == 0) col = cucol;
	cell_t* cp = find_cell(row, col);
	if(!cp) return monostate{};

	//blang_expr_t result;
	value_t v = cp->get_value_2019();
	return to_blang_expr(v);
	//return ConvertVariant<decltype(result), decltype(v)>(v);
}


// a definite quit
static blang_expr_t eval_QUIT (blang_exprs_t args)
{
	int exit_code = blang_to_num(eval(args[0]));
	exit(exit_code);
	return monostate{}; // never reach here
}

void blx_init()
{
	blang_funcmap["bind"] = &eval_bind;
	blang_funcmap["set_cell"] = &eval_set_cell;
	blang_funcmap["get_cell"] = &eval_get_cell;
	blang_funcmap["QUIT"] = &eval_QUIT;
}


