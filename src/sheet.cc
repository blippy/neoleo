
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
#include <set>
#include <tuple>
#include <utility>
#include <vector>



//#include "neotypes.h"
#include "basic.h"
#include "cell.h"
#include "sheet.h"
#include "spans.h"

//import utl;

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::vector;

typedef std::vector<cell_t*> cellmap_t;


/* mcarter 30-Apr-2018
 * Counterintuitively, most functions loop over column, then row. This
 * is the way files are written for example. That being the case, we
 * store coordinates as 32-bits with the column in the high 16-bit word
 * rather than the row. This creates a more natural order, eliminating
 * the need to do some other sort ... unless you want row order, of course
 */

coord_t to_coord(coord_t row, coord_t col) { return  (col << 16) + row; }
int get_col(coord_t coord) { return coord >> 16; }
int get_row(coord_t coord) { return coord & 0xFFFF; }

int get_col(const CELL* cp) 
{ 
	auto [r,c] = decoord(cp); 
	return c;
}
int get_row(const CELL* cp)
{ 
	auto [r,c] = decoord(cp); 
	return r;
}


void decoord(const CELL* cp, CELLREF& r, CELLREF& c)
{
	coord_t coord = cp->get_coord();
	assert(coord);
	r = get_row(coord);
	c = get_col(coord);

}

std::tuple<CELLREF, CELLREF> decoord(const CELL* cp)
{
	CELLREF r, c;
	decoord(cp, r, c);
	return std::tuple{r, c};
}
cellmap_t the_cells;





void clear_spreadsheet (void)
{
	flush_spans();
	//default_width = saved_default_width;
	default_height = saved_default_height;
	default_jst = base_default_jst;
	default_fmt = FMT_GEN;
	default_lock = LCK_UNL;
	delete_all_cells();
}



int binary_cell_search(int l, int r, coord_t target)
{
	if(r >= l) {
		int mid = l + (r - l)/2;
		coord_t co = the_cells[mid]->get_coord();
		if(co == target)
			return mid;
		if(co > target)
			return binary_cell_search(l, mid-1, target);
		return binary_cell_search(mid+1, r, target);
	}
	return -1;
}


/* mcarter 07-May-2018
 * Implementation of find_cell() assumes the cells are sorted
 * as std::lower_bound() likely performs a binary search
 */
cell_t* find_cell (coord_t coord)
{
	int idx = binary_cell_search(0, the_cells.size() -1, coord);
	if(idx == -1) return nullptr;
	return the_cells[idx];
}

cell_t* find_cell (CELLREF r, CELLREF c) { return find_cell(to_coord(r, c)); }

cell_t* find_or_make_cell (coord_t coord)
{
	cell_t* ptr = find_cell(coord);
	if(ptr) return ptr;	
	ptr = new cell_t(coord);

	/* mcarter 07-May-2018
	 * It would be more efficient to use std::lower_bound() to find the appropriate
	 * insertion point, rather than just adding it on the back and calling sort()
	 */
	the_cells.push_back(ptr);
	static auto compare_cell = [](CELL* a, CELL* b) { return a->get_coord() < b->get_coord(); };
	std::sort(the_cells.begin(), the_cells.end(), compare_cell);

	return ptr;
}
cell_t* find_or_make_cell (CELLREF row, CELLREF col) { return find_or_make_cell(to_coord(row, col)); }
cell_t* find_or_make_cell() {return find_or_make_cell(curow, cucol);}



bool inside(int val, int lo, int hi) { return (lo <= val) && (val <= hi) ; } //FN

bool inside(int r, int c, const struct rng *a_rng) // FN
{ 
	return inside(r, a_rng->lr, a_rng->hr) && inside(c, a_rng->lc,a_rng->hc);
}

bool inside(int r, int c, const struct rng &a_rng) //
{
	return inside(r, c, &a_rng);
}

bool inside(CELL *cp, const struct rng &a_rng) //
{
	coord_t coord = cp->get_coord();
	return inside(get_row(coord), get_col(coord), &a_rng);
}

void make_cells_in_range (const struct rng& a_rng) // FN
{
	for(int i = a_rng.lr; i <= a_rng.hr; ++i)
	       	for(int j  = a_rng.lc; j <= a_rng.hc; ++j)
	       		find_or_make_cell(i, j);
}


CELLREF max_row()
{
	int hi = 1;
	for(CELL* cp : the_cells) {
		int n = get_row(cp->get_coord());
		hi = std::max(hi, n);
	}
	return hi;
}
CELLREF max_col()
{
	int hi = 1;
	for(CELL* cp : the_cells) {
		int n = get_col(cp->get_coord());
		hi = std::max(hi, n);
	}

	return hi;
}

CELLREF highest_row() { return max_row(); }
CELLREF highest_col() { return max_col(); }


void delete_all_cells()
{
	for(auto& c:the_cells)
		delete c;
	the_cells.clear();

}


bool inside(CELL* cp, rng_t* a_rng)
{
	auto [r, c] = decoord(cp);
	return inside(r, c, a_rng); 
}

class exit_c { // this class is only ever used once (see below)
	public:
		exit_c(std::function<void()> fn) : fn(fn) {}
		~exit_c() { fn(); }
	private:
		std::function<void()> fn;
};

exit_c exit_cells(delete_all_cells);


void dump_sheet()
{
	cout << "--- dump_sheet:begin ---\n";
	for(CELL* cp:the_cells) cp->dump_cell();
	cout << "--- dump_sheet:end ---\n";
}




using cell_s = struct { CELLREF r, c; struct cell_flags_s cell_flags; string formula; };
void recreate_cells(const vector<cell_s>& cells)
{
	delete_all_cells();

	for(auto [r, c, flags, formula]: cells) {
		set_and_eval(r, c, formula, true);
		CELL* cp = find_cell(r, c);
		cp->cell_flags = flags;
	}
	Global_modified = 1;
}

// FN insert_col_left
void insert_col_left (coord_t col)
{
	vector<cell_s> cells;
	for(CELL* cp: the_cells) {
		auto [r, c] = decoord(cp);
		if(c>=col) c++;
		cells.push_back(cell_s{r, c, cp->cell_flags, cp->get_formula_text()});
	}

	recreate_cells(cells);

}
// FN-END

// FN insert_row_above
void insert_row_above (coord_t row)
{
	vector<cell_s> cells;
	for(CELL* cp: the_cells) {
		auto [r, c] = decoord(cp);
		if(r>=row) r++;
		cells.push_back(cell_s{r, c, cp->cell_flags, cp->get_formula_text()});
	}

	recreate_cells(cells);

}
// FN-END

void delete_sheet_row(coord_t row)
{
	vector<cell_s> cells;
	for(CELL* cp: the_cells) {
		auto [r, c] = decoord(cp);
		if(r==row) continue;
		if(r>=row) r--;
		cells.push_back(cell_s{r, c, cp->cell_flags, cp->get_formula_text()});
	}
	recreate_cells(cells);
}

void copy_row(CELLREF dst_row, CELLREF src_row)
{

	if(src_row == dst_row) return;
	for(auto cp: the_cells) {
		auto [r, c] = decoord(cp);
		if(dst_row == r) cp->set_formula_text(""); // clear stuff out
		if(src_row != r) continue;
		set_and_eval(dst_row, c, cp->get_formula_text(), true);
		auto dst_cp = find_cell(dst_row, c);
		dst_cp->cell_flags = cp->cell_flags;
	}
}

///////////////////////////////////////////////////////////////////////////
// Looping routines

// FN get_cells_in_range .
std::vector<CELL*> get_cells_in_range(const rng_t& a_rng)
{
	std::vector<CELL*> result;
	for(auto const cp: the_cells) {
		if(cp && inside(cp, a_rng)) result.push_back(cp);
	}
	return result;
	
}
// FN-END
