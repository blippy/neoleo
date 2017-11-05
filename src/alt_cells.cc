#include <iostream>
#include <map>
#include <tuple>

#include "alt_cells.h"
#include "byte-compile.h"
#include "cell.h"
#include "decompile.h"

using std::cout;
using std::endl;
using std::map;
using std::string;

bool operator < (const point_t& left, const point_t& right)
{
	return std::tie(left.r, left.c) < std::tie(right.r, right.c);
}

map<point_t, cell> alt_the_cells;

void
alt_set_cell(int row, int col, const std::string& s)
{
	cur_row = row;
	cur_col = col;

	cell c;
	mem parser_mem(true);
	auto ret = (unsigned char*) parse_and_compile(s.c_str(), parser_mem);
	c.set_cell_formula(ret);

	point_t p{row, col};
	auto it = alt_the_cells.find(p);
	if(it != alt_the_cells.end()) alt_the_cells.erase(it);
	alt_the_cells.insert(std::make_pair(p, c)); 
}

char*
alt_new_value(int row, int col, const std::string& s)
{
	// TODO check lock
	
	alt_set_cell(row, col, s);

	// TODO more stuff

}

char*
alt_set_cell_from_string(int r, int c, const std::string& s)
{
	return alt_new_value(r, c, s);
}

std::string
alt_cell_value_string(const point_t& pt)
{
	auto it = alt_the_cells.find(pt); 
	if(it == alt_the_cells.end()) return "";

	auto val = it->second;
	switch(val.get_cell_type()) {
		case TYP_NUL: return "";
		case TYP_INT: return std::to_string(val.get_cell_int());
		case TYP_STR: return val.cell_str(); 
		default:
			      panic("alt_cell_value_string() didn't handle a type");
	}
}


std::string
alt_decomp(const point_t& pt)
{
	return decomp(pt.r, pt.c);
}

template <class T>
void
test_eq(std::string test_name, const T& lhs, const T& rhs)
{
	if(lhs == rhs)
		cout << "PASS";
	else
		cout << "FAIL";

	cout << " " << test_name << endl;
}


void
altc_test01()
{
	point_t pt{1,1};
	alt_set_cell_from_string(1, 1, "42");
	test_eq("altc01", string("42"), string(alt_decomp(pt))); // will actually equal 63.36
}

bool
run_alt_cells_tests()
{
	cout << "Running alt cells tests ...\n";
	cout << "Pass/fail is skipped, because it mostly doesn't work\n";
	altc_test01();
	cout << "... Finished running alt cells tests\n";
	return false;
}
