#include <cmath>

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
