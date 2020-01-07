#pragma once

#include <set>
#include <string>

#include "eval.h"
#include "mem.h"
#include "node.h"
#include "sheet.h"

inline char *instr = 0;
inline int parse_error = 0;
#define YYREGSTYPE struct node*

inline YYREGSTYPE  parse_return;
YYREGSTYPE make_list (YYREGSTYPE, YYREGSTYPE);

int yyparse_parse(const std::string& input, mem_c& yymem);
void add_parse_hash(const char* name, function_t* func);
void* alloc_parsing_memory(size_t nbytes);

/* This turns user input for a cell into a parse tree */
class FormulaParser {
	public:
		//FormulaParse();
		bool parse(const std::string& text);
		node* root();
		void clear();
		~FormulaParser();
	private:
		mem_c parser_mem{true};
		node* m_root = nullptr;
};

void parse_range(struct node* n);
void clear_parse_prec_cells();
std::set<coord_t> get_parse_prec_cells();
