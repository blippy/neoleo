#pragma once


#include "neotypes.h"
#include "wsht.hh"

class wsht;




int 		get_width(wsht& wsh, CELLREF col);
void 		set_width(wsht& wsh, CELLREF col, int wid);
void 		flush_spans(wsht& wsh);
int 		get_height (wsht& wsh, CELLREF row);
int 		get_scaled_height (wsht& wsh, CELLREF r);
int 		get_scaled_width (wsht& wsh, CELLREF c);
