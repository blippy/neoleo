// scanner support routines

#include <cstring>
#include <sstream>

#include "parse_parse.h"
#include "logging.h"

extern int yyregparse();

void FormulaParser::clear()
{
	parser_mem.release_all();
	m_root = nullptr;
	parse_error = 0;
	parse_return =0;
}

bool  FormulaParser::parse(const std::string& text)
{

	clear();

	bool ok = true;

	// is the text purely whitespace?
	bool trivial = std::all_of(text.cbegin(), text.cend(), [](int c) { return std::isspace(c) !=0;});
	//cout << "FormulaParser::parse:trivial:"  << trivial ;
	if(trivial) return ok;

	bool ok1 = yyparse_parse(text, parser_mem) == 0;
	bool ok2 = parse_error == 0;
	ok  = ok1 && ok2; 
	m_root = parse_return;
	//cout << ":parse_error:" << parse_error << ":ok1:" << ok1 << ":ok2:" << ok2 << ":ok:" << ok << ":root:" << (void*) root() << "\n";
	//cout << ":ok:" << ok << "\n";
	return ok;
}

node* FormulaParser::root() { return m_root; }

FormulaParser::~FormulaParser()
{
	clear();
}

static mem_c* _mem_ptr = nullptr;

void* alloc_parsing_memory(size_t nbytes)
{
	void* ptr = malloc(nbytes);
	_mem_ptr->add_ptr(ptr);
	return ptr;
}

YYREGSTYPE
make_list (YYREGSTYPE car, YYREGSTYPE cdr)
{
	YYREGSTYPE ret;

	ret=(YYREGSTYPE)alloc_parsing_memory(sizeof(*ret));
	ret->comp_value = 0;
	ret->n_x.v_subs[0]=car;
	ret->n_x.v_subs[1]=cdr;
	return ret;
}


/* create a sentinel to check that yyparse() is only called
 * via yyparse_parse()
 */

static bool allow_yyparse = false;	
void check_parser_called_correctly()
{
	if(!allow_yyparse)
		throw std::logic_error("parse.yy:yyparse() called erroneously. Call yyparse_parse() instead");
}

int yyparse_parse(const std::string& input, mem_c& yymem)
{
	allow_yyparse = true;	
	instr = (char*) malloc(input.size()+1);
	yymem.add_ptr(instr);
	assert(instr);
	strcpy(instr, input.c_str());
	_mem_ptr = &yymem;
	_mem_ptr->auto_release();
	//int ret = yyparse();
	clear_parse_prec_cells();
	int ret = yyregparse();
	allow_yyparse = false;	
	return ret;
}

int yyparse_parse(const std::string& input)
{
	mem_c yymem;
	return yyparse_parse(input, yymem);
}

std::string to_string(const rng_t& rng)
{
	std::ostringstream ss;
        ss << "{" << rng.lr << ", " << rng.lc << ", " << rng.hr << ", " << rng.hc << "}";
	return std::move(ss).str();
}

static std::set<coord_t> m_prec_cells;

void clear_parse_prec_cells() { m_prec_cells.clear(); }
std::set<coord_t> get_parse_prec_cells() { return m_prec_cells; }

void parse_range(struct node* n)
{
	const rng_t& rng = n->n_x.v_rng;
	for(int r= rng.lr ; r <= rng.hr; ++r)
		for(int c= rng.lc; c <= rng.hc; ++c)
			m_prec_cells.insert(to_coord(r, c));
	//log_debug("parse_range: " + to_string(rng));

}
