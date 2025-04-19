#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <variant>
#include <vector>

#include "errors.h"

using num_t = double;
using CELLREF = uint16_t;
//static_assert(sizeof(unsigned short) == 2);
constexpr auto MAX_ROW = std::numeric_limits<CELLREF>::max();
constexpr auto MAX_COL = std::numeric_limits<CELLREF>::max();

typedef struct rng { CELLREF lr, lc, hr, hc; } rng_t;

typedef struct { bool v; } bool_t;

typedef std::vector<std::string> strings;

class cell;
typedef cell cell_t;
typedef cell CELL;
typedef uint32_t coord_t; // definition cell location

typedef std::vector<rng_t> ranges_t;
