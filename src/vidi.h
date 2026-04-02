#pragma once
/* 26/3 created
 * It's designed to abstract away ncurses.
 * abbrev: vidi - virtual display
 *
 */

#include <functional>
#include <string>
#include <tuple>

#include "neotypes.h"
#include "spans.h"

constexpr auto WIN_LCK_HZ	=	0x01; // locked horizontally
constexpr auto WIN_LCK_VT	=	0x02; // locked vertically
constexpr auto WIN_PAG_HZ	=	0x04; // page horizontally
constexpr auto WIN_PAG_VT	=	0x08; // page vertically
constexpr auto WIN_EDGES	=	0x10; // edged disabled
constexpr auto WIN_EDGE_REV	=	0x20; // edges standout
inline const int win_flags = WIN_EDGES | WIN_EDGE_REV;


inline constexpr int grid_starts = 4;	// y-position where data grid starts
inline constexpr int status = 1;


typedef struct  {int cursr; int cursc; std::string str; struct cell_flags_s cell_flags;} vcell_t;

enum class WinWhich { width, height }; // used for example by recenter_axis (but which axis)

class window_c {
public:
	window_c() {};


	/* Do not change these directly. */
	const int id = 1; // a window id
	int win_down = grid_starts; // should be const
	int win_over = 0;// x-position Where the data in this window starts. Can change due to row number
	struct rng screen { 0 }; /* Cells visible. recenter_* updates this. */

	/* Number of lines of spreadsheet that can fit in this window.
	 This only changes when the screen is resized,
	 win->flags&WIN_EDGES changes, or a window is either
	 created or destroyed */
	int numr = 0;

	/* Number of text columns that can fit in this window.
	 This changes when the screen is resized,
	 win->flags&WIN_EDGES changes, a window is created or
	 destroyed, or win->lh_wid changes.  In the last case
	 win->numc+win->lh_wid remains a constant. */
	int numc = 0;

	/*
	 * Number of columns and rows for right and bottom edges.
	 * As this changes, numc and numr change accordingly.
	 */
	//int bottom_edge_r = 0;
	//int right_edge_c = 0;

	/* Number of columns taken up by the row numbers at the
	 left hand edge of the screen.  Zero if edges is
	 win->flags&WIN_EDGES is off (by definition).  Seven (or
	 five) if win->flags&WIN_PAG_HZ (to make things easier).
	 Ranges between three "R9 " to seven "R32767 " depending on
	 the number of the highest row on the screen.  */
	int lh_wid() const;


	// terminal size might have changed
	// 26/3 Created
	void update(int num_cols, int num_lines);
	std::vector<vcell_t> get_vidi_cells();
	std::tuple<int, int> get_cursor(int curow, int cucol);
	void  recenter_window ();
	void io_move_cell_cursor (CELLREF rr, CELLREF cc);
	int get_vid_col_width(int c); // width of cell, possibly restricted by display dimensions
	//void recenter_axis(CELLREF cur, int (*get) (CELLREF), int total, CELLREF *loP, CELLREF *hiP);
	void recenter_axis(CELLREF cur, WinWhich which, int total, CELLREF *loP, CELLREF *hiP);
	int get_which(WinWhich which, CELLREF ref);
	void set_cursor_from_mouse(int mouse_x, int mouse_y);


private:
	int _lh_wid = 3; // number of cols taken up by the row numbers, e.g. "R123 "
	int m_num_cols = 0, m_num_lines = 0; // corresponds to COLS and LINES in an ncurses display
	void page_axis(CELLREF cur, WinWhich which, int total, CELLREF *loP, CELLREF *hiP);
	void find_nonzero (CELLREF *curp, WinWhich which, CELLREF lo, CELLREF hi);

};

//using vidi_get = std::function<int(int)>;
