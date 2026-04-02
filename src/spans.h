#pragma once

#include <deque>
#include <map>

#include "neotypes.h"

/*
extern float height_scale;
extern float width_scale;
extern float user_height_scale;
extern float user_width_scale;
*/

typedef struct span_find {
	std::deque<std::pair<int, int>> dq;
} span_find_t;

typedef std::map<int,int> span_t; // used for the widths and heights of cells
