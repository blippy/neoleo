#pragma once
#include <string>

#include "eval.h"
#include "mem.h"

inline char *instr = 0;
inline int parse_error = 0;
#define YYREGSTYPE struct node*

//int yyparse_parse(const std::string& input);
int yyparse_parse(const std::string& input, mem& yymem);
void add_parse_hash(const char* name, function* func);

void* alloc_parsing_memory(size_t nbytes);

