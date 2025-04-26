#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <variant>
#include <vector>


using num_t = double;
using CELLREF = uint16_t;
//static_assert(sizeof(unsigned short) == 2);
constexpr auto MAX_ROW = std::numeric_limits<CELLREF>::max();
constexpr auto MAX_COL = std::numeric_limits<CELLREF>::max();
constexpr auto MIN_ROW = 1;
constexpr auto MIN_COL = 1;

typedef struct rng { CELLREF lr, lc, hr, hc; } rng_t;

typedef struct { bool v; } bool_t;

typedef std::vector<std::string> strings;

class cell;
typedef cell cell_t;
typedef cell CELL;
typedef uint32_t coord_t; // definition cell location
static_assert(sizeof(CELLREF) == 2, "coord_t won't work properly otherwise");

typedef std::vector<rng_t> ranges_t;


enum ValType { TYP_NUL=0, // taken to mean 'undefined'
	TYP_FLT=1, TYP_INT=2, TYP_STR=3, TYP_BOL=4, TYP_ERR=5, TYP_RNG=7 };
typedef struct err_t { int num; std::string what; } err_t;

/* These are all the possible error codes that eval_expression() can return. */
enum { ERR_CMD =1, BAD_INPUT,  NON_NUMBER,  NON_STRING, NON_BOOL, NON_RANGE,  OUT_OF_RANGE, NO_VALUES,
	DIV_ZERO, BAD_NAME, NOT_AVAL, PARSE_ERR, NO_OPEN, NO_CLOSE, NO_QUOTE, BAD_CHAR, BAD_FUNC, CYCLE};

#define CCC1(x) x
inline const char *ename_desc[] =
{
	CCC1("#WHAT?"),
	CCC1("#ERROR"), CCC1("#BAD_INPUT"), CCC1("#NON_NUMBER"), CCC1("#NON_STRING"),
	CCC1("#NON_BOOL"), CCC1("#NON_RANGE"), CCC1("#OUT_OF_RANGE"), CCC1("#NO_VALUES"),
	CCC1("#DIV_BY_ZERO"), CCC1("#BAD_NAME"), CCC1("#NOT_AVAIL"), CCC1("#PARSE_ERROR"),
	CCC1("#NEED_OPEN"), CCC1("#NEED_CLOSE"), CCC1("#NEED_QUOTE"), CCC1("#UNK_CHAR"),
	CCC1("#UNK_FUNC"), CCC1("#CYCLE"),
	CCC1(0)
};

// monostate basically means "empty" as a valid type
typedef std::variant<std::monostate, num_t, std::string, err_t, rng_t, bool_t> value_t;

#if 0
bool is_bool(const value_t& val);
bool is_err(const value_t& val);
bool is_string(const value_t& val);
bool is_nul(const value_t& val);
bool is_range(const value_t& val);
bool is_num(const value_t& val);


bool operator==(const err_t& lhs, const err_t& rhs);
bool operator!=(const err_t& lhs, const err_t& rhs);
bool operator!=(const rng_t& lhs, const rng_t& rhs);
bool operator!=(const bool_t& lhs, const bool_t& rhs);
#endif
