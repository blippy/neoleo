#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "numeric.h"

typedef std::variant<num_t, std::string> value_t;
typedef std::vector<value_t> values;


typedef struct cell_t {
	std::string user_text;
	//base_ptr ast; // abstract syntax tree of the user_text
} cell_t;

