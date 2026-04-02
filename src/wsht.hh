#pragma once
/*
 * wsht.hh
 *
 * An attempt to  aggregate all the bits together
 *
 *  Created on: 2 Apr 2026
 *      Author: pi
 */

#include "spans.h"




class wsht {
public:
	span_t the_wids;
	void 		flush_spans();
	int 		get_height (CELLREF row);
	int 		get_scaled_height (CELLREF r);
	int 		get_scaled_width (CELLREF c);
	int			get_span(const span_t& span, int ref, int incr, int def);
	int 		get_width (CELLREF col);
	int 		next_span(span_find_t& sp, CELLREF& n);
	void 		set_width (CELLREF col, int wid);

};

inline wsht the_wsht;
//inline auto& the_wids = the_wsht.the_wids;

inline constexpr int get_scaled_height(auto r) { return 1; }
inline constexpr int get_height(auto r) { return 1; }
