#pragma once

#include <cctype>
#include <memory>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <variant>
#include <vector>

#include "numeric.h"

typedef std::variant<num_t, std::string> value_t;
typedef std::vector<value_t> values;

typedef std::vector<std::string> strings;

class cell;
typedef cell cell_t;
typedef uint32_t coord_t; // definition cell location
