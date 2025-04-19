#include "io-term.h"
#include "logging.h"
#include "spans.h"


void flush_spans ()
{
	the_wids.clear();
	the_hgts.clear();
}

int get_scaled_height (CELLREF r)
{
	return get_height(r);
}

int get_scaled_width (CELLREF c)
{
	return get_width(c);
}

int get_span(const span_t& span, int ref, int incr, int def)
{
	auto search = span.find(ref);
	if(search != span.end() && search->second !=0)
		return search->second + incr;
	else
		return def;
}

#define COL  cucol

int get_width() { return get_width(COL);} // FN

void set_width (int wid) { set_width(COL, wid); } // FN

int get_width (CELLREF col) // FN
{
	return get_span(the_wids, col, 0, default_width);
}

void set_width (CELLREF col, int wid) // FN
{
	the_wids[col] = wid;
	//io_update_width(col, wid);
}



span_find_t find_span (span_t& spans, CELLREF lo, CELLREF hi) // FN
{
	span_find_t res;
	for(int i=lo; i<=hi; ++i)
		if(spans.find(i) != spans.end())
			res.dq.emplace_back(std::make_pair(i, spans[i]));
	return res;
}





int next_span (span_find_t& sp, CELLREF& n) // FN
{
	if(sp.dq.empty()) return 0;
	auto [n1, span]  = sp.dq[0];
	sp.dq.pop_front();
	n = n1;
	return span;
}

int get_height (CELLREF row) // FN
{
	int incr = - 1; // NB Why?
	int h = get_span(the_hgts, row, incr, default_height);
	assert(h==1);
	return h;
}


void set_height (CELLREF row, int hgt) // FN
{
	the_hgts[row] = hgt;
}
