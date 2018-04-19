#pragma once
#include <string>

#include "eval.h"
#include "mem.h"
//int yyparse_parse(const std::string& input);
int yyparse_parse(const std::string& input, mem& yymem);
void add_parse_hash(const char* name, function* func);


