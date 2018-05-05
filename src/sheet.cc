
/* mcarter TODO 05-May-2018
 * Now that I want to incorporate the coord in with the cell, be warned that
 * it will likely be outdated under a move or copy. I'll need to watch out 
 * for that!
 */

#include <algorithm>
#include <cctype>
#include <deque>
//#include <functional>
#include <iostream>
#include <map>
#include <tuple>
#include <utility>


#include "neotypes.h"
#include "cell.h"
#include "logging.h"
#include "mem.h"
#include "sheet.h"

using std::cout;
using std::endl;
using std::map;
using std::string;

static void log_debug_1(std::string msg)
{
	if constexpr(true) 
		log_debug("DBG:alt-cells.cc:" + msg);
}

/* mcarter 30-Apr-2018
 * Counterintuitively, most functions loop over column, then row. This
 * is the way files are written for example. That being the case, we
 * store coordinates as 32-bits with the column in the high 16-bit word
 * rather than the row. This creates a more natural order, eliminating
 * the need to do some other sort ... unless you want row order, of course
 */

coord_t to_coord(coord_t row, coord_t col) { return  (col << 16) + row; }
int get_col(coord_t coord) { return coord >> 16; }
int get_row(coord_t coord) { return coord & 0xFF; }


cellmap_t the_cells;




void flush_cols()
{
}

cell_t* find_cell (coord_t coord)
{
	auto it = the_cells.find(coord);
	if(it == the_cells.end())
		return nullptr;
	else
		return it->second;
}
cell_t* find_cell (CELLREF r, CELLREF c) { return find_cell(to_coord(r, c)); }

cell_t* find_or_make_cell (coord_t coord)
{
	cell_t* ptr = find_cell(coord);
	if(ptr) return ptr;	
	ptr = new cell_t;
	the_cells[coord] = ptr;
	return ptr;
}
cell_t* find_or_make_cell (CELLREF row, CELLREF col) { return find_or_make_cell(to_coord(row, col)); }


void init_cells ()
{
}

bool inside(int val, int lo, int hi) { return (lo <= val) && (val <= hi) ; }

bool inside(int r, int c, struct rng *a_rng) 
{ 
	return inside(r, a_rng->lr, a_rng->hr) && inside(c, a_rng->lc,a_rng->hc);
}

void make_cells_in_range (struct rng *r)
{
	log_debug_1("make_cells_in_range");
	for(int i = r->lr; i <= r->hr; ++i)
	       	for(int j  = r->lc; j <= r->hc; ++j)
			find_or_make_cell(i, j);
}
void no_more_cells ()
{
}

CELLREF max_row()
{
	int hi = 1;
	for(auto const& c : the_cells)
		hi = std::max(hi, get_row(c.first));
	return hi;
	//return std::reduce(the_cells.begin(), the_cells.end(), 1, std::max);
}
CELLREF max_col()
{
	int hi = 1;
	for(auto const& c : the_cells)
		hi = std::max(hi, get_col(c.first));
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

///////////////////////////////////////////////////////////////////////////
// Looping routines

celldeq_t get_cells_in_range(struct rng *r)
{
	celldeq_t res;
	for(auto const& a_cell: the_cells) {
		coord_t coord = a_cell.first;
		if(inside(get_row(coord), get_col(coord), r))
			res.push_back(a_cell.second);
	}
	return res;
	
}
static std::deque<coord_t> m_cell_find_dq;

void find_cells_in_range (struct rng *r)
{
	log_debug_1("find_cells_in_range");
	for(auto const& a_cell: the_cells) {
		coord_t coord = a_cell.first;
		if(inside(get_row(coord), get_col(coord), r))
			m_cell_find_dq.push_back(coord);
	}
}

cell* next_row_col_in_range (CELLREF *rowp, CELLREF *colp)
{
	log_debug_1("next_row_col_in_range:called");
	if(m_cell_find_dq.empty()) return nullptr;
	coord_t coord = m_cell_find_dq.front();
	m_cell_find_dq.pop_front();
	*rowp = get_row(coord);
	*colp = get_col(coord);
	// not especially efficient, as we could use m_cell_find_dq to store the cell, too
	auto ptr = find_cell(coord);
	return ptr;
}
cell* next_cell_in_range ()
{
	log_debug_1("next_cell_in_range:called");
	CELLREF r, c; // simply discarded
	return next_row_col_in_range(&r, &c);
}


