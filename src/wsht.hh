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
	// 26/4 expand further
};

inline wsht the_wsht;
//inline auto& the_wids = the_wsht.the_wids;

inline constexpr int get_scaled_height(auto r) { return 1; }
inline constexpr int get_height(auto r) { return 1; }
