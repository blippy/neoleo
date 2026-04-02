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

typedef struct span_find {
	std::deque<std::pair<int, int>> dq;
} span_find_t;

typedef std::map<int,int> span_t; // used for the widths and heights of cells


class wsht {
public:
	span_t the_wids;
	span_find_t	find_span(CELLREF lo, CELLREF hi);
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

/*
#define get_scaled_width the_wsht.get_scaled_width
#define next_span the_wsht.next_span
#define find_span the_wsht.find_span
#define flush_spans the_wsht.flush_spans
#define set_width the_wsht.set_width
#define get_width the_wsht.get_width
*/
