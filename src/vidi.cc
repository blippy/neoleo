#include <cmath>

#include "cell.h"
#include "sheet.h"
#include "spans.h"
#include "vidi.h"
#include "wsht.hh"

#define wsh the_wsht

static void recenter_axis (CELLREF cur, int (*get) (CELLREF), int total, CELLREF *loP, CELLREF *hiP);

int window_c::lh_wid() const {
	return _lh_wid;
};

// 26/3 created
// we need to ensure that the cell fits within the width of the display
int window_c::get_vid_col_width (int c)
{
	return std::min(numc, get_width(wsh, c));
}


void window_c::find_nonzero (CELLREF *curp, WinWhich which, CELLREF lo, CELLREF hi)
{
	CELLREF cc;
	int n;

	cc = *curp;

	if (cc < hi)
	{
		cc++;
		while ((n = get_which(which, cc)) == 0)
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
		while ((n = get_which(which, cc)) == 0)
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
	if(curow < screen.lr || curow > screen.hr)
		recenter_axis(curow, WinWhich::height, numr, &screen.lr, &screen.hr);

	// reconfigure the column settings
	// NB this is dependent on the margin we need for printing th row numbers
	_lh_wid = std::log10(screen.hr) + 3; // est. num of lines taken up by row numbers
	win_over = _lh_wid + 1;
	int numc_old = numc;
	numc = num_cols - _lh_wid;

	// recompute struct screen, assuming cursor will be OK. It's the best we can do at his stage
	// NB column widths might have changed, too.
	screen.hc = screen.lc;
	int ncols = get_vid_col_width(screen.lc);
	while (ncols + get_vid_col_width(screen.hc + 1) <= numc)	ncols += get_vid_col_width(++screen.hc);
	if(cucol < screen.lc || cucol > screen.hc) {
		// drats, didn't work, so just recenter
		recenter_axis(cucol, WinWhich::width, numc, &screen.lc, &screen.hc);
	}


}

int window_c::get_which(WinWhich which, CELLREF ref)
{
	if(which == WinWhich::width)
		return get_vid_col_width(ref);
	else
		return get_scaled_height(ref);
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

	if (get_vid_col_width(cucol) == 0)
		find_nonzero(&cucol, WinWhich::width , screen.lc, screen.hc);
	if (get_scaled_height(curow) == 0)
		find_nonzero(&curow, WinWhich::height, screen.lr, screen.hr);
}

//void window_c::recenter_axis (CELLREF cur, auto  get, int total, CELLREF *loP, CELLREF *hiP)
void window_c::recenter_axis(CELLREF cur, WinWhich which, int total, CELLREF *loP, CELLREF *hiP)
{


	CELLREF lo, hi;
	int tot;
	int n;
	int more;

	lo = hi = cur;
	n = tot = get_which(which, cur);
	do
	{
		if (lo > MIN_ROW && tot + (n = get_which(which, lo - 1)) <= total)
		{
			--lo;
			tot += n;
			more = 1;
		}
		else
			more = 0;
		if (hi < MAX_ROW && tot + (n = get_which(which, hi + 1)) <= total)
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

void window_c::page_axis (CELLREF cur, WinWhich which, int total, CELLREF *loP, CELLREF *hiP)
{
	CELLREF lo, hi;
	int w, ww;

	lo = hi = MIN_ROW;
	w = get_which(which, hi);
	for (;;)
	{
		ww = get_which(which, hi + 1);
		while (w + ww <= total && hi < MAX_ROW)
		{
			hi++;
			w += ww;
			ww = get_which(which, hi + 1);
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
		page_axis (curow, WinWhich::height, numr, &(screen.lr), &(screen.hr));
	else
		recenter_axis (curow, WinWhich::height, numr, &(screen.lr), &(screen.hr));
	// 26/3 win->set_numcols(win->screen.hr);
	if (win_flags & WIN_PAG_HZ)
		page_axis (cucol, WinWhich::width, numc, &(screen.lc), &(screen.hc));
	else
		recenter_axis (cucol, WinWhich::width, numc, &(screen.lc), &(screen.hc));
}



std::vector<vcell_t> window_c::get_vidi_cells()
{
	std::vector<vcell_t> res;
	res.reserve(200); // just for good measure
	//ioc_print_cells();
	for(int r = screen.lr ; r <= screen.hr; r++) {
		size_t msw = 0; // maximum slop width - allowing cells to overspill to the next column
		for(CELLREF c = screen.hc; c >= screen.lc; c--) {
			vcell_t vc;
			CELL* cp = find_cell(r, c);
			size_t width = get_vid_col_width(c);
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
		cell_cursor_col += get_vid_col_width(cc);

	int cell_cursor_row = win_down;
	for (int rr = screen.lr; rr < r; rr++)
		cell_cursor_row += get_height(rr);

	return{cell_cursor_row, cell_cursor_col};
}


void window_c::set_cursor_from_mouse(int mouse_x, int mouse_y)
{
	//log("window_c::set_cursor_from_mouse: called ", mouse_x , " ", mouse_y);
	int col = 0;
	int x = mouse_x - _lh_wid;
	if(x >= 0) {
		int tot = 0;
		for(int c = screen.lc; c <= screen.hc; c++) {
			tot += get_vid_col_width(c);
			if(x <= tot) {
				col = c;
				break;

			}

		}
	}
	//log("window_c::set_cursor_from_mouse: col ", col);

	//int row = 0;
	int row = std::max(0, mouse_y + screen.lr -win_down);
	//log("window_c::set_cursor_from_mouse: row ", row);

	if(col >0 && row >0) {
		cucol = col;
		curow = row;
	}

}
