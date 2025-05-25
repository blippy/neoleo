#include "spans.h"
#include "cell.h"

import std;
import utl;

void flush_spans ()
{
	the_wids.clear();
	//the_hgts.clear();
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
	return 1; // all heights are assumed 1
	#if 0
	int incr = - 1; // NB Why?
	int h = get_span(the_hgts, row, incr, default_height);
	assert(h==1);
	return h;
	#endif
}




/* ****************************************************************************************** */
// 25/4/ some test code that I will eventually want to replace a lot of the code above with



template<int WIDTH>
struct span1_t {
        //std::array<int, 10> vals;
        int vals[WIDTH];
        span1_t(int init_val) { for(int i = 0; i<WIDTH; i++) vals[i] =init_val; };
        int& operator[](int i) { return vals[i]; };
        //void operator[](std::size_t i, int i) { };
};

int example_2025()
{
	using std::cout;
	using std::endl;
        struct span1_t<10> arr(43);
        //cout << "hi" << endl;
        cout << arr[5] << endl;
        arr[7] = 12;
        cout << arr[7] << endl;
        //cout << "Hello from clang" << bar() << endl;
        return 0;
}
