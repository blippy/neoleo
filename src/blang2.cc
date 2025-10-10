// 25/10 Let's try a new lexer (not yet started)

#include <cassert>
#include <ctype.h>
#include <deque>
#include <iostream>
//#include <map>
#include <cmath>
#include <stacktrace>


#include "blang2.h"

using expr_t = blang_expr_t;
using usr_funcall_t = blang_usr_funcall_t;

using namespace std;



map<string, blang_usr_funcall_t> usr_funcmap;


///////////////////////////////////////////////////////////////////////////////////////////////////
// TYPE DECLARATIONS




//class ParseException : public std::exception {};

blang_num_t to_num (blang_expr_t val);

void parse_error (string msg = "")
{
	std::cout << std::stacktrace::current();
	msg = "parse error: " + msg;
	throw std::runtime_error(msg);
}

void eval_error (string msg = "")
{
	std::cout << std::stacktrace::current();
	msg = "runtime error: " + msg;
	throw std::runtime_error(msg);
}

void blang_unknown_function(string function_name)
{
	string msg{"Unknown function " + function_name};
	parse_error(msg);
}

static blang_function_t* blang_fn_lookup (string function_name)
{
	if(blang_funcmap.count(function_name) == 0)
		blang_unknown_function(function_name);
	return &blang_funcmap[function_name];
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES

static map<string, blang_expr_t> blang_varmap;

//void setvar(string)

///////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS

blang_expr_t eval(blang_expr_t expr);
blang_num_t num_eval(blang_expr_t expr);
string str_eval(blang_expr_t expr);

// FN to_string .
std::string to_string (const blang_expr_t& val)
{
	if(holds_alternative<monostate>(val)) return ""; // don't use get_if in the case ∵ of unused variable
	if(auto v = std::get_if<blang_num_t>(&val)) 	return to_string(*v);
	if(auto v = std::get_if<int>(&val)) 	return to_string(*v);
	if(auto v = std::get_if<std::string>(&val)) 	return *v;
	throw std::logic_error("to_string: Unhandled stringify expression type index " + to_string(val.index()));
}
// FN-END



bool isstring(blang_expr_t v)
{
	return holds_alternative<string>(v);
}

// FN eval_block .
// A block is code between { ... }
// Returns the value of the last statement executed
blang_expr_t eval_block (blang_exprs_t args)
{
	//cout << "I'm evalutiong a block\n";
	blang_expr_t v{monostate{}};
	for(auto const& a: args) {
		v = eval(a);
	}
	return v;

}
//FN-END

// FN eval_bodmas .
// A special function used by parsing to get the +|- *|/ right
blang_expr_t eval_bodmas (blang_exprs_t args)
{

	//cout << "eval_bodmas num args " << args.size() << endl;

	// may have to deal with strings
	blang_expr_t result = eval(args[1]);
	if(isstring(result)) {
		//cout << "eval_bodmas with string type\n";
		string result_str = std::get<string>(result);;
		for(size_t i = 2 ; i < args.size();) {
			int op = num_eval(args[i]);
			if( op == '+') {
				result_str += to_string(eval(args[i+1]));
			} else {
				eval_error("Can only use + operator on string");
			}
			i += 2;
		}
		return result_str;
	}

	// OK, for most purposes we're dealing with numbers
	//cout << "bodmans: called\n";
	//num_t result_num = 0;
	blang_num_t result_num = to_num(result);
	for(size_t i = 2; i < args.size(); ) {
		//cout << "i = " << i << endl;
		int op = num_eval(args[i]);		// operator. One of + - * /
		blang_num_t arg = num_eval(args[i+1]);  	// operand, e.g. "3"
		//cout << "eval_bodname op " << op << " arg " << arg <<endl;
		switch(op) {
		case '+' : result_num += arg; break;
		case '-' : result_num -= arg; break;
		case '*' : result_num *= arg; break;
		case '/' : result_num /= arg; break;
		}

		i += 2; // we take arguments pairwise
	}
	return  result_num;
}
// FN-END

// FN eval_plus .
blang_expr_t eval_plus (blang_exprs_t args)
{
	blang_num_t val = 0;
	for(auto& arg: args) val += num_eval(arg );
	return val;
}
// FN-END

blang_expr_t eval_minus(blang_exprs_t args)
{
	if(args.size() == 0) return 0.0;
	blang_num_t val = num_eval(args[0]);
	if(args.size() == 1) return -val; // if there is only one argument, then return the negative of it
	for(size_t i = 1; i<args.size(); ++i) val -= num_eval(args[i]);
	return val;
}
blang_expr_t eval_mul(blang_exprs_t args)
{
	blang_num_t val = 1.0;
	for(auto& arg: args) {
		//num_t a = eval(arg);
		val *=  num_eval(arg);
		//cout << "do_mul a and val " << a << " " << val << "\n";
	}
	return val;
}
blang_expr_t eval_div(blang_exprs_t args)
{
	if(args.size() == 0) return 0.0;
	blang_num_t val = num_eval(args[0]);
	//cout << "do_div 1/val " << 1.0/val << "\n";
	if(args.size() == 1) return 1.0/val;
	for(size_t i = 1;  i<args.size(); ++i) val /= num_eval(args[i]);
	return val;
}

blang_expr_t eval_life(blang_exprs_t args)
{
	return 48.0;
}
blang_expr_t eval_sqrt(blang_exprs_t args)
{
	if(args.size() !=1) parse_error("sqrt: requires  argument");
	blang_num_t val = num_eval(args[0]);
	return sqrt(val);
}
blang_expr_t eval_hypot(blang_exprs_t args)
{
	if(args.size() !=2) parse_error("hypot: requires 2 arguments");
	blang_num_t v1 = num_eval(args[0]);
	blang_num_t v2 = num_eval(args[1]);
	return sqrt(v1*v1 + v2*v2);
}
blang_expr_t eval_plusfn(blang_exprs_t args)
{
	blang_num_t val = 0;
	for(auto& v: args) val += num_eval(v);
	return val;
}

blang_expr_t eval_strlen(blang_exprs_t args)
{
	//cout << "eval_strlen: called" << endl;
	if(args.size() !=1) parse_error("strlen: requires 1 argument");
	string s = to_string(eval(args[0]));
	//cout << " eval_strlen value " << s << endl;
	return (float) s.size();
}

blang_expr_t eval_print(blang_exprs_t args)
{
	//cout << "eval_print: called" << endl;
	for(const auto& a : args) {
		cout << to_string(eval(a));
	}
	cout << endl;

	//if(args.size() !=1) parse_error();
	//return (int) str_eval(args.at(0)).size();
	return std::monostate{};
}


// We get the value of a variable
blang_expr_t eval_getvar(blang_exprs_t args)
{
	string name = to_string(args[0]);
	if(blang_varmap.contains(name)) {
		return blang_varmap[name];
	} else {
		return monostate{};
	}
}

// this is where we are assigning a variable
blang_expr_t eval_let(blang_exprs_t args)
{
	string varname = to_string(args[0]);
	blang_expr_t result = eval(args[1]);
	blang_varmap[varname] = result;
	return result;
}


blang_expr_t eval_interpret (blang_exprs_t args)
{
	//cout << "eval_interpret called" << endl;
	string code = to_string(eval(args[0]));
	//cout << "code is " << code << endl;
	return blang_interpret_string(code);
}

// These will be augmented by user defined functions using eval_userfn
map<string, blang_function_t> blang_funcmap= {
	{"strlen", eval_strlen},
	{"+", &eval_plus},
	{"-", &eval_minus},
	{"*", &eval_mul},
	{"/", &eval_div},
	{"interpret", &eval_interpret},
	{"life", eval_life},
	{"sqrt", eval_sqrt},
	{"hypot", eval_hypot},
	{"plus", eval_plusfn},
	{"print", eval_print}
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// LEXER


enum Tokens { EOI = 128, NUMBER, ID, STR, SUB, CALL, LET };

typedef struct {
	enum Tokens type;
	string text;
} 	token_t;

typedef deque<token_t> tokens_t;

// FN Lexer .
class BlangLexer {
public:
	BlangLexer(string input);
	//token_t yylex();
	bool isfirst(string s);
	void consume(string s);
	token_t front();
	token_t pop_front();
	void push_front(token_t t); // for when you want to undo a peek
	bool eof();
	~BlangLexer();

private:
	void tokenise(const string& str);
	void found(int type, const string& text);
	tokens_t tokens;
	//string m_input;
};
// FN-END

void BlangLexer::push_front(token_t t)
{
	tokens.push_front(t);
}

token_t BlangLexer::pop_front()
{
	if(tokens.empty()) return token_t{EOI, "EOI"};
	token_t t = tokens.front();
	tokens.pop_front();
	return t;
}
token_t BlangLexer::front()
{
	if(tokens.empty()) return token_t{EOI, "EOI"};
	return tokens.front();
}

BlangLexer::BlangLexer(string input)
{
	tokenise(input);

}

BlangLexer::~BlangLexer() {
	tokens.clear();
}

bool BlangLexer::eof()
{
	//if(tokens.empty()) cout << "Lexer::eof: empty\n";
	return tokens.empty();
	//return tokens.size() == 0;
}
bool BlangLexer::isfirst(string c)
{
	return tokens.front().text == c;
}

void BlangLexer::found(int type, const string& text)
{
	token_t t{(Tokens) type, text};
	tokens.push_back(t);
}


void BlangLexer::consume(string s)
{
	if(isfirst(s))
		tokens.pop_front();
	else
		parse_error("consume: looking for " + s + ", but found " + tokens.front().text);
}

void BlangLexer::tokenise(const string& str)
{
	//cout << "Parsing: " << str << "\n";
	const char* cstr = str.c_str();
	int pos = 0;
	//auto it = str.begin();
loop:
	string token;
	char ch = cstr[pos];
	if(ch == 0) {
		//goto finis;
		return;
	} else if(ch == '#') {
		while(ch != '\n') ch = cstr[++pos];
	} else if(isspace(ch)) {
		while(isspace(ch)) { ch = cstr[++pos]; }
	} else if ( isdigit(ch)) {
		while(isdigit(ch) || ch == '.' ) { token += ch; ch = cstr[++pos]; }
		found(NUMBER, token);
	} else if (ch == '$' && isalpha(cstr[pos+1])) {
		//cout << "found $" <<endl;
		pos++;
		found('$', "$"); // start of a vairable name
	} else if (isalpha(ch)) {
		while(isalnum(ch) && ch) { token += ch; ch = cstr[++pos]; }
		if(token == "sub") {
			found(SUB, token);
		} else if (token == "call") {
			found(CALL, token);
		} else if (token == "let") {
			//cout << "Found LET" << endl;
			found(LET, token);
		} else {
			found(ID, token);
		}
		//cout << "found id: " << token << "\n";
	} else if(ch == '"') {
		while(1) {
			ch = cstr[++pos];
			if(ch == 0 || ch == '"') {pos++; break; }
			if(ch == '\\') {
				ch = cstr[++pos];
				switch (ch) {
					case 0: ch = '\\'; break;
					case 'n': ch = '\n'; break;
					case 't': ch = '\t'; break;
				}
				//if(ch == '\"')  				
			}
			token += ch;
		}
		//cout << "tokenise string is <" << token << ">\n";
		found(STR, token);
	}else {
		token = ch;
		pos++;
		found(ch, token);
	}
	goto loop;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// SCANNER (the "yacc" side of things)



	template<class Q>
Q rest(Q qs)
{
	qs.pop_front();
	return qs;
}


typedef deque<string> ops_t;

// ARITHMETIC
// Adopt the algorithm at 
// https://www.engr.mun.ca/~theo/Misc/exp_parsing.htm#classic
// for computing arithmetic
//
// Here's the original derivations
//  E --> T {( "+" | "-" ) T}
//  T --> F {( "*" | "/" ) F}
//  F --> P ["^" F]
//  P --> v | "(" E ")" | "-" T
//
// Here's mine:
// I extend BNF with the notion of a function, prefixed by &
// {} zero or more repetitions
// [] optional
//  &M(o, X) --> X {o X} // e.g. &M(("+"|"-"), T) --> T { ("+"|"-") T }
//  E --> &M(( "<" | "<=" | ">" | ">=" | "==" | "!=" ), R)
//  R --> &M(( "+" | "-" ), T)
//  T --> &M(( "*" | "/" ), F)
//  F --> ["+"|"-"] (v | "(" E ")")



BlangParser::BlangParser(BlangLexer& lxr) : lxr{lxr}
{
	//this->lxr = lxr;
}
BlangParser::~BlangParser()
{
}

void BlangParser::db (string s)
{
	cout << "Parser::db: " << s << endl;
}

void BlangParser::dbx (string s)
{
	// hushed debug messages
}


void BlangParser::consume(string s)
{
	lxr.consume(s);
}




// FN parse_bra .
blang_expr_t BlangParser::parse_bra ()
{
	blang_expr_t x = parse_e();
	lxr.consume(")");
	return x;

}
// FN-END

// FN parse_fncall .
// 'parse_p' has already identified a function, and its name
blang_expr_t BlangParser::parse_fncall (string func_name)
{
	funcall_t fc;
	fc.fn = *blang_fn_lookup(func_name);
	lxr.consume("(");

	while(1) {
		if(lxr.isfirst(")")) break;
		fc.exprs.push_back(parse_e());
		if(lxr.isfirst(",")) lxr.consume(",");
	}

	lxr.consume(")");
	return fc;
}
// FN-END

// FN parse_let .
// LET $foo = 1 +2
blang_expr_t BlangParser::parse_let ()
{
	//cout << "parse_let called" << endl;
	consume("$");
	string varname = lxr.pop_front().text;
	//cout << "parse_let: varname:" << varname << endl;
	consume("=");

	funcall_t fc;
	fc.fn = eval_let;
	fc.exprs = { varname, parse_t()};
	//let_t let;
	//let.name = varname;
	//let.exprs = {parse_e()};
	return fc;
}
// END-FN

// FN parse_fndef .
// 'parse_p' has already identified a function def
blang_expr_t BlangParser::parse_defsub ()
{
	//cout << "parse_defsub: called" << endl;
	consume("sub");
	string sub_name = lxr.pop_front().text;
	//cout << "sub name is " << sub_name << endl;

	usr_funcall_t ufc;
	ufc.func_name = sub_name;

	consume("{");
	while(1) {
		auto type = lxr.front().type;
		//cout << "Parser::parse_block looping with text " << lxr.front().text << "\n";
		if(type == '}') break;
		ufc.codeblock.push_back(parse_e());
	}
	consume("}");
	usr_funcmap[sub_name] = ufc;
	return monostate{};
}
// FN-END

// FN parse_varname .
// This is where we get the value of a variable
blang_expr_t BlangParser::parse_varname ()
{
	string varname = lxr.pop_front().text;
	//cout << "parse_varname is " << varname << endl;
	funcall_t fc;
	fc.fn = eval_getvar;
	fc.exprs = {varname};
	return fc;

}

// FN parse_p .
// P -> V | ( E ) | -T | FN
blang_expr_t BlangParser::parse_p ()
{
//#define parse_x parse_e
	token_t toke{lxr.pop_front()};
	switch((int)toke.type) {
	case NUMBER: dbx("parse_p pushing NUMBER " + toke.text); return stof(toke.text);
	case '$': return parse_varname();
	case STR: dbx("parse_p STR of <" + toke.text + ">" ) ; return toke.text;
	case ID: return parse_fncall(toke.text);
	case CALL: return parse_call();
	case LET: dbx("parse_p found LET"); return parse_let();
	case '(': return parse_bra();
	// case '{': return parse_block(); NB No, not a general expression
	}

	return monostate{};
}
// FN-END

// FN parse_t .
blang_expr_t BlangParser::parse_t ()
{
#define  parse_x parse_p // reference to next level down in the hierarchy

		funcall_t fc;
		fc.fn = eval_bodmas;
		fc.exprs = { '+', parse_x()}; // sic
		while(1) {
			auto type = lxr.front().type;
			if(type == EOI) { dbx("parse_t breaking"); break;}
			if(type == '*' || type == '/') {
				lxr.pop_front();
				fc.exprs.push_back(type);
				fc.exprs.push_back(parse_x());
				//cout << "pushing */\n";
			} else {
				break;
			}
		}
		return fc;
#undef parse_x
}
// FN parse_e .
blang_expr_t BlangParser::parse_e ()
{
#define  parse_x parse_t // reference to next level down in the hierarchy

	// must be dealing with numerical data of some sort
	funcall_t fc;
	fc.fn = eval_bodmas;
	fc.exprs = { '+', parse_x()};
	while(1) {
		auto type = lxr.front().type;
		if(type == EOI) {dbx("parse_e breaking"); break;}
		//if(type == '{') return parse_block();
		if(type == '+' || type == '-') {
			lxr.pop_front();
			fc.exprs.push_back(type);
			fc.exprs.push_back(parse_x());
			//cout << "pushing +-\n";
		} else {
			break;
		}
	}
	return fc;
#undef parse_x
}
// FN-END

// FN parse_call .
blang_expr_t BlangParser:: parse_call ()
{
	//consume("call");
	string sub_name = lxr.pop_front().text;
	auto ufc = usr_funcmap[sub_name];
	return ufc;
}
// FN parse_top .
blang_expr_t BlangParser::parse_top ()
{
	//cout << "parse_top " << lxr.front().type << endl;
	switch(lxr.front().type ) {
	case SUB: return parse_defsub();
	default: return parse_e(); // expression
	}


}



///////////////////////////////////////////////////////////////////////////////////////////////////
// EVAL

//template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
//template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// FN eval .
blang_expr_t eval (blang_expr_t expr)
{
	// might be a little more comprehensible if I hard-coded the function names
	if(auto fc = std::get_if<funcall_t>(&expr)) {
		auto fn = fc->fn;
		return fn(fc->exprs);
	}

	if(auto ufc = std::get_if<usr_funcall_t>(&expr)) {
		//cout << "eval on usr " << ufc->func_name << endl;
		blang_expr_t result{monostate{}};
		for(auto const& cb : ufc->codeblock) {
			result = eval(cb);
		}
		return result;
	}

	return expr;  // it's just a constant
}
// FN-END

blang_num_t to_num (blang_expr_t val)
{
	if(holds_alternative<monostate>(val)) return 0; // don't use get_if() in this case ∵ an unused var will be generated otherwise
	if(auto v = std::get_if<blang_num_t>(&val)) 	return *v;
	if(auto v = std::get_if<int>(&val)) 	return *v;
	//if(auto v = std::get_if<std::string>(&val)) 	return "\""s + *v + "\""s;
	throw std::logic_error("to_num: Unhandled stringify expression type index " + to_string(val.index()));
}


blang_num_t num_eval (blang_expr_t expr) { return to_num(eval(expr)); }
string to_str (blang_expr_t v) { return std::get<string>(v); }
string str_eval (blang_expr_t expr) { return to_str(eval(expr)); }
///////////////////////////////////////////////////////////////////////////////////////////////////

blang_expr_t blang_interpret_string(const string& s)
{
	BlangLexer lxr(s);
	BlangParser p(lxr);
	blang_expr_t result; // of last expression
	while(!lxr.eof()) {
		blang_expr_t expr{p.parse_top()};
		result = eval(expr); // this evaluated precisely 1 expression
	}
	return result;
}
