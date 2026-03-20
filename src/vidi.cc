#include <cmath>

#include "cell.h"
#include "sheet.h"
#include "vidi.h"



int window_c::lh_wid() const {
	return _lh_wid;
};



// terminal size might have changed
// 26/3 Created
void window_c::update(int num_cols, int num_lines)
{
	m_num_cols = num_cols;
	m_num_lines = num_lines;

	// reconfigure the row settings
	int numr_old = numr;
	numr = num_lines - grid_starts;
	if (numr < numr_old) { // tty got smaller
		screen.lr = curow; //  For simplicity, make curow the top row
	}
	screen.hr = screen.lr + numr - 1; // figure out how many cells are in a row

	// reconfigure the column settings
	// NB this is dependent on the margin we need for printing th row numbers
	_lh_wid = std::log10(screen.hr) + 3; // est. num of lines taken up by row numbers
	win_over = _lh_wid + 1;
	int numc_old = numc;
	numc = num_cols - _lh_wid;
	if (numc < numc_old) { // tty shrank
		screen.lc = cucol; // For simplicity, make cucol the left-most col
	}
	if (numc != numc_old) { // a change is made, so let's figure out the new extent
		screen.hc = screen.lc;
		int num_cols = get_width(screen.hc);
		while (num_cols + get_width(screen.hc + 1) <= numc)
			num_cols += get_width(++screen.hc);
	}
}



std::vector<vcell_t> window_c::get_vidi_cells()
{
	std::vector<vcell_t> res;
	res.reserve(200); // just for good measure
	//ioc_print_cells();
	for(int r = screen.lr ; r < screen.hr; r++) {
		size_t msw = 0; // maximum slop width - allowing cells to overspill to the next column
		for(CELLREF c = screen.hc; c >= screen.lc; c--) {
			vcell_t vc;
			CELL* cp = find_cell(r, c);
			size_t width = get_width(c);
			msw += width;
			if(cp == nullptr || width == 0) continue;
			auto str = utl_fmt_cell(cp, width-1, msw-1);

			msw = 0;


			auto [cell_cursor_row, cell_cursor_col] = get_cursor(r, c);

			vc.cell_flags = cp->cell_flags;
			vc.cursc = cell_cursor_col;
			vc.cursr = cell_cursor_row;
			vc.str = str;
			res.push_back(vc);
		}
	}

	return res;
}

std::tuple<int, int> window_c::get_cursor(int r, int c)
{
	int cell_cursor_col = win_over;
	for (int cc = screen.lc; cc < c; cc++)
		cell_cursor_col += get_width(cc);

	int cell_cursor_row = win_down;
	for (int rr = screen.lr; rr < r; rr++)
		cell_cursor_row += get_height(rr);

	return{cell_cursor_row, cell_cursor_col};
}


