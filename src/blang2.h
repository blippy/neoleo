#pragma once

#include <functional>
#include <map>
#include <string>
#include <exception>
#include <string>
#include <variant>
#include <vector>

typedef double blang_num_t;

struct blang_funcall_t;
struct blang_usr_funcall_t;

// FN blang_expr_t .
// NB We need to be able to return funcall_t in the case of function definitions
using blang_expr_t = std::variant<std::monostate, blang_num_t, int, std::string, blang_funcall_t, blang_usr_funcall_t>;
// FN-END

// FN blang_exprs_t .
using blang_exprs_t =std::vector<blang_expr_t>;
// FN-END

// FN function_t .
using blang_function_t =  std::function<blang_expr_t(blang_exprs_t)>;
// FN-END

blang_expr_t eval_plus (blang_exprs_t args);



// FN blang_funcall_t .
typedef struct blang_funcall_t {
	blang_function_t fn;
	blang_exprs_t exprs;
} funcall_t;
// FN-END

// FN blang_usr_funcall_t .
typedef struct blang_usr_funcall_t {
	std::string func_name; // good for debugging purposes
	blang_exprs_t codeblock;
} blang_usr_funcall_t;
// FN-END

extern std::map<std::string, blang_function_t> blang_funcmap;


class BlangException : public std::exception {
private:
    std::string message;
public:

    // Constructor accepting const char*
    BlangException(const std::string msg) : message(msg) {}

    // Override what() method, marked
    // noexcept for modern C++
    const char* what() const noexcept {
        return message.c_str();
    }
};


class BlangLexer;

// FN Parser .
class BlangParser {
public:
	BlangParser(BlangLexer& lxr);
	blang_expr_t parse_top();
	void db(std::string s); // debug message
	void dbx(std::string s);
	~BlangParser();
private:
	using expr_t = blang_expr_t;
	//Expr parse_e();
	BlangLexer& lxr;
	void consume(std::string s);
	void parser_error(std::string msg);
	//expr_t parse_block();
	expr_t parse_block();
	expr_t parse_bra();
	expr_t parse_call();
	expr_t parse_defsub(); // we're defining a sub
	expr_t parse_e();
	expr_t parse_fncall(std::string func_name); // for calling a sub
	expr_t parse_if();
	expr_t parse_let();
	expr_t parse_p();
	expr_t parse_t();
	expr_t parse_varname();
	expr_t parse_while();
	//Expr binop(Expr& left, string& op, Expr& right);
};
// FN-END



// FUNCTIONS

blang_expr_t 	blang_interpret_string(const std::string& s);
blang_expr_t 	eval (blang_expr_t expr);
blang_num_t		to_num (blang_expr_t val);
std::string 	to_string (const blang_expr_t& val);
