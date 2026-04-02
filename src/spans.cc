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

int get_span(const span_t& span, int ref, int incr, int def)
{
	auto search = span.find(ref);
	if(search != span.end() && search->second !=0)
		return search->second + incr;
	else
		return def;
}


int get_width (wsht& wsh, CELLREF col) // FN
{
	return get_span(wsh.the_wids, col, 0, default_width);
}

void set_width (wsht& wsh, CELLREF col, int wid) // FN
{
	wsh.the_wids[col] = wid;
}



span_find_t find_span (wsht& wsh, CELLREF lo, CELLREF hi) // FN
{
	span_find_t res;
	for(int i=lo; i<=hi; ++i)
		if(wsh.the_wids.find(i) != wsh.the_wids.end())
			res.dq.emplace_back(std::make_pair(i, wsh.the_wids[i]));
	return res;
}





int next_span (wsht& wsh, span_find_t& sp, CELLREF& n) // FN
{
	if(sp.dq.empty()) return 0;
	auto [n1, span]  = sp.dq[0];
	sp.dq.pop_front();
	n = n1;
	return span;
}

int get_height (wsht& wsh, CELLREF row) // FN
{
	return 1; // all heights are assumed 1
}

