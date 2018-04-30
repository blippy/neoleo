#include "lists.h"
#ifdef USE_ALT_CELLS

#include <algorithm>
#include <cctype>
//#include <functional>
#include <iostream>
#include <map>
#include <tuple>
#include <utility>


//#include "alt-cells.h"
//#include "alt-parse.h"
//#include "byte-compile.h"
#include "cell.h"
//#include "decompile.h"
#include "mem.h"

using std::cout;
using std::endl;
using std::map;
using std::string;

typedef struct cell cell_t;
typedef uint32_t coord_t;
typedef std::map<coord_t, cell_t*> cellmap_t;

cellmap_t the_cells;


coord_t to_coord(coord_t row, coord_t col) { return  row << 16 + col; }
int col(coord_t coord) { return coord & 0xFF; }
int row(coord_t coord) { return coord >> 16; }

void flush_cols()
{
}

cell_t* find_cell (CELLREF row, CELLREF col)
{
	auto it = the_cells.find(to_coord(row, col));
	if(it == the_cells.end())
		return nullptr;
	else
		return it->second;
}

cell_t* find_or_make_cell (CELLREF row, CELLREF col)
{
	cell_t* ptr = find_cell(row, col);
	if(ptr) return ptr;	
	ptr = new cell_t;
	the_cells[to_coord(row, col)] = ptr;
	return ptr;
}
struct cell* next_cell_in_range ()
{
	return nullptr; // TODO
}

struct cell* next_row_col_in_range (CELLREF *rowp, CELLREF *colp)
{
	return nullptr; // TODO
}

void init_cells ()
{
}

void find_cells_in_range (struct rng *r)
{
	// TODO
}
void make_cells_in_range (struct rng *r)
{
	// TODO
}
void no_more_cells ()
{
}

CELLREF max_row()
{
	int hi = 1;
	for(auto const& c : the_cells)
		hi = std::max(hi, row(c.first));
	return hi;
	//return std::reduce(the_cells.begin(), the_cells.end(), 1, std::max);
}
CELLREF max_col()
{
	int hi = 1;
	for(auto const& c : the_cells)
		hi = std::max(hi, col(c.first));
	return hi;
}

CELLREF highest_row() { return max_row(); }
CELLREF highest_col() { return max_col(); }


void delete_all_cells()
{
	for(auto& [k, v] : the_cells)
		delete v;
	the_cells.clear();

}


exit_c exit_cells(delete_all_cells);


/*
bool run_alt_cells_tests()
{ 
	return true; 
}
*/


#endif // USE_ALT_CELLS
