#pragma once


#include "neotypes.h"
#include "wsht.hh"

class wsht;




span_find_t	find_span(wsht& wsh, CELLREF lo, CELLREF hi);
int 		get_width(wsht& wsh, CELLREF col);
void 		set_width(wsht& wsh, CELLREF col, int wid);
void 		flush_spans(wsht& wsh);
int 		get_height (wsht& wsh, CELLREF row);
int 		get_scaled_height (wsht& wsh, CELLREF r);
int 		get_scaled_width (wsht& wsh, CELLREF c);
//int			get_span(const span_t& span, int ref, int incr, int def);
int 		next_span(wsht& wsh,span_find_t& sp, CELLREF& n);
