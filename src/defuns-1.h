#pragma once

#include <map>
#include <string_view>

#include "funcs.h"

typedef struct { 
	interactive_function func_func;
	std::string_view cpp_name; 
} cmd_func_t;

typedef std::map<std::string_view, cmd_func_t> cmd_func_map;
