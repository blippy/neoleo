#pragma once
/*
 * Copyright (c) 1990, 1992, 1993, 1999, 2000, 2001 Free Software Foundation, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <string_view>

#include "neotypes.h"

//inline constexpr auto BITS_PER_CELLREF = 16;

/* The location of a cell that can never be referenced */
#define NON_ROW		0
#define NON_COL		0



//typedef unsigned short CELLREF;





inline CELLREF curow = MIN_ROW;
inline CELLREF cucol = MIN_COL;
inline constexpr rng_t rng_all{.lr = MIN_ROW, .lc = MIN_COL, .hr = MAX_ROW, .hc = MAX_COL};



/* These have two uses.  During parsing, these contain the 
 * base address of all relative references.  During evaluation,
 * these contain the address of the cell that is being updated.
 * 
 * Wey are all these distinct uses bound up in one pair of GLOBAL
 * variables?  GOOD QUESTION?  Why didn't the person who created the mess at
 * least toss in a COMMENT like the above to explain what was happening? 
 * ANOTHER GOOD QUESTION!
 */
inline CELLREF cur_row, cur_col;

extern void panic (const char *, ...);
extern char *range_name (struct rng *);
extern char *cell_name (CELLREF, CELLREF);


struct var; /* in case it hasn't been declared yet */

/*
 * This structure is a start at cleaning up global variables that are
 * around all over.
 */
struct OleoGlobal {
	struct window			*cwin = 0; //, *wins = 0;
	int				bkgrnd_recalc = 1, auto_recalc = 1, a0 = 0, topclear = 0, sylk_a0 = 1;
	int				modified = 0;
	//const int				nwin = 0;
	int				return_from_error = 0;
	int				display_opened = 0;
	//const float			user_height_scale = 1.0, user_width_scale = 1.0, height_scale = 1.0, width_scale = 1.0;
/* From Window.c */
	const int			scr_lines = 24, scr_cols = 80,  input_rows = 1, status_rows = 1; 
	int 				user_input =1, user_status = 2;
	int				input = 0, status = 1;
	const int			label_rows = 1, label_emcols =1;
	struct info_buffer		*current_info;
	int				info_rows, info_line, info_over;
	int				default_right_border = 0, default_bottom_border = 0;
	int				win_id = 1;
};

inline OleoGlobal global;
//inline struct OleoGlobal *Global = new struct OleoGlobal;
inline struct OleoGlobal *Global = &global;
inline int nwin = 1; // 25/4 Elminate this
