#pragma once

//import std;

#include <array>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <fstream>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

//#include "win.h"


using num_t = double;
using CELLREF = uint16_t;
//static_assert(sizeof(unsigned short) == 2);
constexpr auto MAX_ROW = std::numeric_limits<CELLREF>::max();
constexpr auto MAX_COL = std::numeric_limits<CELLREF>::max();
static_assert(MAX_ROW>256);
constexpr auto MIN_ROW = 1;
constexpr auto MIN_COL = 1;
/* The location of a cell that can never be referenced */
constexpr auto NON_ROW = 0;
constexpr auto NON_COL = 0;

#define FMT_DEF		0	/* Default */
#define FMT_HID		1	/* Hidden */
//#define FMT_GPH		2	/* Graph */
#define FMT_DOL		3	/* Dollar */
#define FMT_CMA		4	/* Comma */
#define FMT_PCT		5	/* Percent */
#define FMT_USR		6	/* User defined */
#define FMT_FXT		7
#define FMT_EXP		8
#define FMT_GEN		9
#define	FMT_DATE	10	/* Date */

/* Other cell defaults: */


inline int default_fmt = FMT_GEN;
inline int default_prc = 0x0F;		/* FIX ME */



/*
#define JST_DEF 	0
#define JST_LFT		1
#define jst::rgt		2
#define jst::cnt		3
*/

enum class jst { def, lft, rgt, cnt};
inline enum jst base_default_jst = jst::rgt;
inline enum jst default_jst = base_default_jst;


typedef struct rng {
	CELLREF lr, lc, hr, hc;
	bool operator==(const rng&) const = default;
} rng_t;


// FN bool_t .
typedef struct bol {
	bool v;
	bool operator==(const bol&) const = default;
} bool_t;
// FN-END

typedef std::vector<std::string> strings;

class cell;
typedef cell cell_t;
typedef cell CELL;
typedef uint32_t coord_t; // definition cell location
static_assert(sizeof(CELLREF) == 2, "coord_t won't work properly otherwise");

typedef std::vector<rng_t> ranges_t;


//enum ValType { TYP_NUL=0, // taken to mean 'undefined'
//	TYP_FLT=1, TYP_STR=3, TYP_BOL=4, TYP_ERR=5, TYP_RNG=7 };

typedef struct err_t {
	int num;
	std::string what;
	bool operator==(const err_t&) const = default;
} err_t;

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

// FN value_t .
// monostate basically means "empty" as a valid type
typedef std::variant<std::monostate, num_t, std::string, err_t, rng_t, bool_t> value_t;
// FN-END




const auto VERSION = "16.0";
const auto  PACKAGE = "neoleo";
const auto PACKAGE_NAME = PACKAGE;


inline CELLREF curow = MIN_ROW;
inline CELLREF cucol = MIN_COL;

inline std::string option_tests_argument = "";

inline bool Global_modified = false;

typedef struct point_t {int r; int c;} point_t;

void raise_error (const char *str, ...); 
void raise_error (const std::string& msg);


class Log
{
	public:
		Log();
		void debug(std::string s);
		~Log();
		int m_count = 0;
	private:
		std::ofstream m_ofs;
};

void log_debug(const char* s);
void log_debug(const std::string& s);

template<typename... Args>
void log(Args ... args) {
	std::ostringstream ss;
	(ss << ... << args);
	log_debug(ss.str());
}

std::string trim(const std::string& str);

// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf/3742999#3742999
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

extern struct user_fmt fxt;


void bind_char(char c, std::string blang_code); // bind a character to some blang code requiring interpreting
std::string 	cell_value_string (CELLREF row, CELLREF col, int add_quote);
char 			*adjust_prc (char *oldp, CELL * cp, int width, int smallwid, enum jst just);
std::string 	trim(const std::string& str);
std::string 	print_cell_flt (num_t flt, unsigned int precision, unsigned int j);
std::string 	print_cell (CELL * cp);
std::string 	print_cell ();
std::string 	spaces(int n);
std::string 	pad_left(const std::string& s, int width);
std::string 	pad_right(const std::string& s, int width);
std::string 	pad_centre(const std::string& s, int width);
std::string 	pad_jst(const std::string& s, int width, enum jst j);
std::string 	stringify_value_file_style(const value_t& val);
void 			panic (const char *s,...);
bool 			is_num(const value_t& val);
bool 			is_range(const value_t& val);
char* 			pr_flt (num_t val, struct user_fmt *fmt, int prec, bool use_prec = true);
std::string  	fmt_std_date(int t);
std::string 	fmt_value (value_t& val, int p = default_prc, int j = FMT_DEF);



class defer {
public:
	//defer(std::function<void>() unwind) : m_unwind{unwind} {};
	defer(std::function<void()> fn_unwind) : m_unwind{fn_unwind} {};
	~defer() {m_unwind();};
private:
	std::function<void()> m_unwind;
};


template <typename R, typename T>
class defer1 {
public:
	//defer(std::function<void>() unwind) : m_unwind{unwind} {};
	defer1(R fn_unwind, T param)  : m_unwind{fn_unwind}, m_param{param}  {};
	~defer1() { m_unwind(m_param) ; };
private:
	R m_unwind;
	T m_param;
};



/* https://www.quora.com/How-does-one-write-a-custom-exception-class-in-C++
 * */
class OleoJmp : public std::exception
{
	public:
		OleoJmp() {}
		OleoJmp(const std::string& msg) : msg_(msg) {}

		virtual const char* what() const throw()
		{
			return msg_.c_str() ;
		}

	private:
		std::string msg_ = "OleoJmp";
};

// 25/4 You can use it like
// auto v = to_int(mystr);
std::optional<int> to_int(const std::string& str);



class ValErr : public std::exception
{
	public:
	       ValErr() {}
	       ValErr(const int n) : n(n) {}
	       const char* what() const throw();
	       const int num() const throw();

	private:
	       int n = 0;
	       std::string msg;
};




