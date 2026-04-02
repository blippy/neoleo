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

};

inline wsht the_wsht;
inline auto& the_wids = the_wsht.the_wids;
