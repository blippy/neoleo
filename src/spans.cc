#include "wsht.hh"
#include "cell.h"


void flush_spans (wsht& wsh)
{
	wsh.the_wids.clear();
	//the_hgts.clear();
}

int get_scaled_height (wsht& wsh, CELLREF r)
{
	return get_height(r);
}

int get_scaled_width (wsht& wsh, CELLREF c)
{
	return get_width(wsh, c);
}


int get_width (wsht& wsh, CELLREF col) // FN
{
	try {
		return wsh.the_wids.at(col);
	} catch(const std::out_of_range& ex) {
		return default_width;
	}

	//return get_span(wsh.the_wids, col, 0, default_width);
}

void set_width (wsht& wsh, CELLREF col, int wid) // FN
{
	wsh.the_wids[col] = wid;
}



int get_height (wsht& wsh, CELLREF row) // FN
{
	return 1; // all heights are assumed 1
}

