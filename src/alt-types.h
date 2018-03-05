#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "numeric.h"

typedef std::variant<num_t, std::string> value_t;
typedef std::vector<value_t> values;

class BaseNode
{
	public:
		virtual ~BaseNode() {};
		virtual value_t  eval() =0;

};

typedef std::unique_ptr<BaseNode> base_ptr;
typedef std::vector<base_ptr> base_ptrs;

typedef struct cell_t {
	std::string user_text;
	base_ptr ast; // abstract syntax tree of the user_text
} cell_t;

