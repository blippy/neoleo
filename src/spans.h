#pragma once

#include <deque>
#include <map>

#include "global.h"

extern float height_scale;
extern float width_scale;
extern float user_height_scale;
extern float user_width_scale;

typedef std::map<int,int> span_t; // used for the widths and heights of cells

typedef struct span_find {
	std::deque<std::pair<int, int>> dq;
} span_find_t;

inline span_t the_wids; // , the_hgts;

void set_width (CELLREF col, int wid);
span_find_t find_span(span_t& spans, CELLREF lo, CELLREF hi);
void flush_spans();
int next_span(span_find_t& sp, CELLREF& n);
int get_width ();
int get_width (CELLREF col);
int get_scaled_height (CELLREF r);
void set_width (int wid); // of current column
int get_height (CELLREF row);
int get_scaled_width (CELLREF c);
