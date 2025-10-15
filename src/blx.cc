/*
 * blx.cc
 *
 *  blang "extras". Separated from blang2 because blang2 is a generic language
 *  This file contains neoleo extensions
 *  Created on: 11 Oct 2025
 *      Author: pi
 */

#include <tuple>
#include <type_traits>

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

// convenience function to deduce row and column from arguments passed in
static auto get_rc(blang_exprs_t args)
{
	int row = blang_to_num(eval(args[0]));
	if(row == 0) row = curow;
	int col = blang_to_num(eval(args[1]));
	if(col == 0) col = cucol;
	return std::make_tuple(row, col);
}

static blang_expr_t eval_set_cell (blang_exprs_t args)
{
	auto [row, col] = get_rc(args);
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
	auto [row, col] = get_rc(args);
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

static blang_expr_t eval_isnull(blang_exprs_t args)
{
	if(std::holds_alternative<std::monostate>(eval(args[0])))
		return 1;
	else
		return 0;
}

template<class Variant, class T>
struct compare_variant;

template<class T, class ... Ts>
struct compare_variant<std::variant<Ts...>, T>
{
    static bool apply(std::variant<Ts...> obj1, T obj2)
    {
        return ((std::is_same<Ts, T>::value
                && std::holds_alternative<T>(obj1)
                && obj1 == obj2)||...);
    }
};

static blang_expr_t eval_eq (blang_exprs_t args)
{
	blang_expr_t r1 = eval(args[0]);
	blang_expr_t r2 = eval(args[1]);

	//auto cmp = compare_variant<blang_expr_t, blang_expr_t>{};
	//cmp.apply(r1,r2);
	//cout << "eval_eq called "  << endl;

	if(r1.index() != r2.index()) return 0;

	int ok = 0;
	if(holds_alternative<num_t>(r1)) {
		ok = get<num_t>(r1) == get<num_t>(r2);
	} else if(holds_alternative<int>(r1)) {
		ok = get<int>(r1) == get<int>(r2);
	} else if(holds_alternative<string>(r1)) {
		ok = get<string>(r1) == get<string>(r2);
	} else if(holds_alternative<monostate>(r1)) {
		ok = 1; // both types are monostates, and so must be equal
	}

	//cout << "eval_eq result: " << ok << endl;

	return ok;
	//if(r1.index() != r2.index()) return 0;
	//if(r1 == r2)
	//	return 1;
	//else
	//	return 0;
}


static blang_expr_t eval_or (blang_exprs_t args)
{
	for(const auto& a : args) {
		blang_expr_t r = eval(a);
		if(auto v = std::get_if<num_t>(&r)) 	return (*v) == 0 ? 0 : 1;
		if(auto v = std::get_if<int>(&r)) 	return (*v) == 0 ? 0 : 1;
	}

	return 0;
}

// print type of cell. User call gst function
static blang_expr_t eval_get_cell_type (blang_exprs_t args)
{
	auto [row, col] = get_rc(args);
	cell_t* cp = find_cell(row, col);
	if(!cp) return "Z";

	//blang_expr_t result;
	value_t v = cp->get_value_2019();
	if(holds_alternative<monostate>(v))
		return "M";
	else if(holds_alternative<num_t>(v))
		return "N";
	else if(holds_alternative<string>(v))
		return "S";
	else if(holds_alternative<err_t>(v))
		return "E";
	else if(holds_alternative<rng_t>(v))
		return "R";
	else if(holds_alternative<bool_t>(v))
		return "B";

	return "?";
	//return ConvertVariant<decltype(result), decltype(v)>(v);
}

void blx_init()
{
	blang_funcmap["bind"] = &eval_bind;
	blang_funcmap["eq"] = &eval_eq;
	blang_funcmap["isnull"] = &eval_isnull;
	blang_funcmap["or"] = &eval_or;
	blang_funcmap["set_cell"] = &eval_set_cell;
	blang_funcmap["get_cell"] = &eval_get_cell;
	blang_funcmap["gct"] = &eval_get_cell_type;
	blang_funcmap["QUIT"] = &eval_QUIT;
}


