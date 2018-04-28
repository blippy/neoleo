#include "io-abstract.h"
#include "io-term.h"
#include "logging.h"
#include "spans.h"

void flush_span (span_t& sp)
{
	/* flush(sp); sp-> 0 */
	sp.clear();
}

void flush_spans ()
{
	flush_span(the_wids);
	flush_span(the_hgts);
}

void
set_user_scales (double hs, double ws)
{
	Global->user_height_scale = hs;
	Global->user_width_scale = ws;
	io_repaint ();
}

int
get_scaled_height (CELLREF r)
{
	return ((Global->user_height_scale <= 0.)
			? 1
			:  (int) (get_height (r) * Global->height_scale * Global->user_height_scale));
}

int
get_scaled_width (CELLREF c)
{
	return ((Global->user_width_scale <= 0.)
			? 1
			: (int)(get_width (c) * Global->width_scale * Global->user_width_scale));
}

int get_span(const span_t& span, int ref, int incr, int def)
{
	auto search = span.find(ref);
	if(search != span.end() && search->second !=0)
		return search->second + incr;
	else
		return def;
}

int 
get_width (CELLREF col)
{
	/*
	int *ptr;

	ptr = (IPTR) find (col, Global->wids, sizeof (int));
	if (!ptr || !*ptr)
		return default_width;
	return (*ptr) - 1;
	*/
	return get_span(the_wids, col, -1, default_width);
}


int 
get_nodef_width (CELLREF col)
{
	/*
	int *ptr;

	ptr = (IPTR) find (col, Global->wids, sizeof (int));
	return ptr ? *ptr : 0;
	*/
	return get_span(the_wids, col, 0, 0);

}

void 
set_width (CELLREF col, int wid)
{
	/*
	int *ptr;

	ptr = (IPTR) make (col, &Global->wids, sizeof (int), COL_BUF);
	*ptr = wid;
	*/
	the_wids[col] = wid;
	io_update_width(col, wid);
}

/*
class spanner_c {
	public:
		spanner_c(span_t& span, int lo, int hi) : span(span), lo(lo), hi(hi), n(lo) {};
		unsigned short next(int& pos) {
			unsigned short ret =0;
			while
			get_span(span, n, 0, 0);
			return ret;

		};
	private:
		int lo, hi, n;
		span_t span;
};
*/

/*
struct find*
find_span(struct list** spans, CELLREF lo, CELLREF hi)
{
	return (FPTR) find_rng(spans, lo, hi, sizeof (int));
}
*/



span_find_t find_span(span_t& spans, CELLREF lo, CELLREF hi)
{
	span_find_t res;
	for(int i=lo; i<=hi; ++i)
		if(spans.find(i) != spans.end())
			res.dq.emplace_back(std::make_pair(i, spans[i]));
	return res;
}


/*
int next_span(struct find* s_find, CELLREF *posp)
{
	int *ptr;

	do
		ptr = (IPTR) next_rng(s_find, posp);
	while (ptr && !*ptr);
	return ptr ? *ptr : 0;
}
*/



int next_span(span_find_t& sp, CELLREF& n)
{
again:
	if(sp.dq.empty()) return 0;
	auto [n1, span]  = sp.dq[0];
	sp.dq.pop_front();
	n = n1;
	//if(span == 0) goto again;
	return span;
}

int 
get_height (CELLREF row)
{
	/*
	int *ptr;

	ptr = (IPTR) find (row, Global->hgts, sizeof (int));
	if (!ptr || !*ptr)
		return default_height;
	return *ptr - 1 + (Global->display_formula_mode && using_curses);
	*/
	int incr = - 1 + (Global->display_formula_mode && using_curses);
	return get_span(the_hgts, row, incr, default_height);
}

int 
get_nodef_height (CELLREF row)
{
	/*
	int *ptr;

	ptr = (IPTR) find (row, Global->hgts, sizeof (int));
	return ptr ? *ptr : 0;
	*/
	return get_span(the_hgts, row, 0, 0);
}

void set_height (CELLREF row, int hgt)
{
	/*
	int *ptr;

	ptr = (IPTR) make (row, &Global->hgts, sizeof (int), ROW_BUF);
	*ptr = hgt;
	*/
	the_hgts[row] = hgt;
}


/*
void shift_spans (struct list** spans, int over, CELLREF lo, CELLREF hi)
{
	do_shift(over, lo, hi, spans, ROW_BUF);
}
*/


void shift_spans (span_t&  spans, int over, CELLREF lo, CELLREF hi)
{
	log_debug("lists:cc:TODO:shift_spans: this actually needs to do something");
}
