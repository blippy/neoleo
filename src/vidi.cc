#include <cmath>

#include "cell.h"
#include "sheet.h"
#include "vidi.h"



int window_c::lh_wid() const {
	return _lh_wid;
};

static void find_nonzero (CELLREF *curp, CELLREF lo, CELLREF hi, int (*get) (CELLREF))
{
	CELLREF cc;
	int n;

	cc = *curp;

	if (cc < hi)
	{
		cc++;
		while ((n = (*get) (cc)) == 0)
		{
			if (cc == hi)
				break;
			cc++;
		}
		if (n)
		{
			*curp = cc;
			return;
		}
	}
	if (cc > lo)
	{
		--cc;
		while ((n = (*get) (cc)) == 0)
		{
			if (cc == lo)
				break;
			--cc;
		}
		if (n)
		{
			*curp = cc;
			return;
		}
	}
}


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


void window_c::io_move_cell_cursor (CELLREF rr, CELLREF cc)
{
	if(inside(rr, cc, screen)) {
		//win_io_hide_cell_cursor(); // 25/5 apparently unnecessary
		curow = rr;
		cucol = cc;
	} else 	{
		curow = rr;
		cucol = cc;
		recenter_window();
	}

	if (get_scaled_width(cucol) == 0)
		find_nonzero(&cucol, screen.lc, screen.hc, get_scaled_width);
	if (get_scaled_height (curow) == 0)
		find_nonzero(&curow, screen.lr, screen.hr, get_scaled_height);
}

static void recenter_axis (CELLREF cur, int (*get) (CELLREF), int total, CELLREF *loP, CELLREF *hiP)
{
	CELLREF lo, hi;
	int tot;
	int n;
	int more;

	lo = hi = cur;
	n = tot = (*get) (cur);
	do
	{
		if (lo > MIN_ROW && tot + (n = (*get) (lo - 1)) <= total)
		{
			--lo;
			tot += n;
			more = 1;
		}
		else
			more = 0;
		if (hi < MAX_ROW && tot + (n = (*get) (hi + 1)) <= total)
		{
			hi++;
			tot += n;
			more++;
		}
	}
	while (more);
	*loP = lo;
	*hiP = hi;
}

static void page_axis (CELLREF cur, int (*get) (CELLREF), int total, CELLREF *loP, CELLREF *hiP)
{
	CELLREF lo, hi;
	int w, ww;

	lo = hi = MIN_ROW;
	w = (*get) (hi);
	for (;;)
	{
		ww = (*get) (hi + 1);
		while (w + ww <= total && hi < MAX_ROW)
		{
			hi++;
			w += ww;
			ww = (*get) (hi + 1);
		}
		if (hi >= cur)
			break;
		hi++;
		lo = hi;
		w = ww;
	}
	if (lo > cur || hi > MAX_ROW)
		raise_error("Can't find a non-zero-sized cell page_axis");
	*loP = lo;
	*hiP = hi;
}


void  window_c::recenter_window () // FN
{
	//if(!win) win = cwin;
	if (win_flags & WIN_PAG_VT)
		page_axis (curow, get_scaled_height, numr, &(screen.lr), &(screen.hr));
	else
		recenter_axis (curow, get_scaled_height, numr, &(screen.lr), &(screen.hr));
	// 26/3 win->set_numcols(win->screen.hr);
	if (win_flags & WIN_PAG_HZ)
		page_axis (cucol, get_scaled_width, numc, &(screen.lc), &(screen.hc));
	else
		recenter_axis (cucol, get_scaled_width, numc, &(screen.lc), &(screen.hc));
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


