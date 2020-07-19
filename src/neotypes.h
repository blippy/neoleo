#pragma once

#include <string>
#include <variant>
#include <vector>

#include "errors.h"

typedef double num_t;
typedef unsigned short CELLREF;

#if 1
typedef struct rng { CELLREF lr, lc, hr, hc; } rng_t;
#else
typedef struct rng { CELLREF lr, lc, hr, hc; int mask = 0; } rng_t;
#endif

typedef struct {}  empty_t;
typedef struct { bool v; } bool_t;
//typedef std::variant<num_t, std::string, err_t, rng_t, bool_t, empty_t> value_t;
//typedef std::vector<value_t> values;

typedef std::vector<std::string> strings;

class cell;
typedef cell cell_t;
typedef cell CELL;
typedef uint32_t coord_t; // definition cell location

typedef std::vector<rng_t> ranges_t;
