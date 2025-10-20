// 25/10 Let's try a new lexer (not yet started)
// 2025-10-11	Owned by blang2 repo

#include <cassert>
#include <chrono>
#include <cstdio>
#include <ctype.h>
#include <deque>
//#include <fcntl.h>
#include <filesystem>
#include <fstream>
//#include <ios>
#include <iostream>
//#include <map>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
//#include <stacktrace>
//#include <sys/syslimits.h>


#include "blang2.h"

namespace blang {


using expr_t = blang_expr_t;
using usr_funcall_t = blang_usr_funcall_t;

//using namespace std;
using std::cout;
using std::deque;
using std::endl;
using std::map;
using std::monostate;
//using std::string;
using string = std::string;



map<string, blang_usr_funcall_t> usr_funcmap;



///////////////////////////////////////////////////////////////////////////////////////////////////
// TYPE DECLARATIONS




//class ParseException : public std::exception {};

blang_num_t blang_to_num (blang_expr_t val);

double to_double(const blang_expr_t& val);


void eval_error (string msg = "")
{
	//std::cout << std::stacktrace::current();
	msg = "runtime error: " + msg;
	throw std::runtime_error(msg);
}

void blang_unknown_function (string function_name)
{
	string msg{"Unknown function " + function_name};
	throw BlangException(msg);
	//parse_error(msg);
}

static blang_function_t* blang_fn_lookup (string function_name)
{
	if(blang_funcmap.count(function_name) == 0)
		blang_unknown_function(function_name);
	return &blang_funcmap[function_name];
}

std::string slurp(const char *filename)
{
        std::ifstream in;
        in.open(filename, std::ifstream::in | std::ifstream::binary);
        std::stringstream sstr;
        sstr << in.rdbuf();
        in.close();
        return sstr.str();
}


bool spit (const string& path, const string &text)
{
        std::fstream file;
        file.open(path, std::ios_base::out);

        if(!file.is_open()) return false;

        file<<text;
        file.close();
        return true;
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
std::string blang_to_string (const blang_expr_t& val)
{
	if(holds_alternative<monostate>(val)) return ""; // don't use get_if in the case ∵ of unused variable

	if(auto v = std::get_if<blang_num_t>(&val)) {
		double n = *v;
		if(floor(n) == n)
			return std::to_string((int)n);
		else
			return std::to_string(n);
	}
	if(auto v = std::get_if<int>(&val)) return std::to_string(*v);
	if(auto v = std::get_if<std::string>(&val)) 	return *v;
	throw std::logic_error("to_string: Unhandled stringify expression type index " + std::to_string(val.index()));
}
// FN-END

double to_double(blang_expr_t& val)
{
	if(holds_alternative<monostate>(val)) return 0;
	if(auto v = std::get_if<int>(&val)) return *v;
	if(auto v = std::get_if<double>(&val)) return *v;

	throw std::logic_error("to_double: Can't convert type index " + std::to_string(val.index()));
}


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
				result_str += blang_to_string(eval(args[i+1]));
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
	blang_num_t result_num = blang_to_num(result);
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

blang_expr_t eval_if (blang_exprs_t args)
{

	if(blang_to_num(eval(args[0])))
			return eval(args[1]);

	// maybe an else
	if(args.size() > 2)
		return eval(args[2]);

	return monostate{};
}

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
	if(args.size() !=1) throw BlangException("sqrt: requires  argument");
	blang_num_t val = num_eval(args[0]);
	return sqrt(val);
}
blang_expr_t eval_hypot(blang_exprs_t args)
{
	if(args.size() !=2) throw BlangException("hypot: requires 2 arguments");
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
	if(args.size() !=1) BlangException("strlen: requires 1 argument");
	string s = blang_to_string(eval(args[0]));
	//cout << " eval_strlen value " << s << endl;
	return (float) s.size();
}

blang_expr_t eval_print(blang_exprs_t args)
{
	//cout << "eval_print: called" << endl;
	for(const auto& a : args) {
		cout << blang_to_string(eval(a));
	}
	cout << endl;

	//if(args.size() !=1) parse_error();
	//return (int) str_eval(args.at(0)).size();
	return std::monostate{};
}


// We get the value of a variable
blang_expr_t eval_getvar(blang_exprs_t args)
{
	string name = blang_to_string(args[0]);
	if(blang_varmap.contains(name)) {
		return blang_varmap[name];
	} else {
		return monostate{};
	}
}

// this is where we are assigning a variable
blang_expr_t eval_let(blang_exprs_t args)
{
	string varname = blang_to_string(args[0]);
	blang_expr_t result = eval(args[1]);
	blang_varmap[varname] = result;
	//cout << "eval_let set " << varname << " with " << to_num(result) << endl;
	return result;
}

blang_expr_t eval_while(blang_exprs_t args)
{
	blang_expr_t result;
	while(blang_to_num(eval(args[0])))
		result = eval(args[1]);
	return result;
}

blang_expr_t eval_interpret (blang_exprs_t args)
{
	//cout << "eval_interpret called" << endl;
	string code = blang_to_string(eval(args[0]));
	//cout << "code is " << code << endl;
	return interpret_string(code);
}

blang_expr_t eval_system (blang_exprs_t args)
{
	string cmd = blang_to_string(eval(args[0]));
	return system(cmd.c_str());
}

blang_expr_t eval_concat (blang_exprs_t args)
{
	string result;
	for(auto a: args) {
		result += blang_to_string(eval(a));
	}
	return result;
}

blang_expr_t eval_str (blang_exprs_t args)
{
	blang_expr_t a0 = eval(args[0]);
	if(holds_alternative<string>(a0)) return a0;

	if(args.size() == 1) return blang_to_string(a0);

	int prec = blang_to_num(eval(args[1]));

	char s[30];
	double v = to_double(a0);
	string fmt = "%." + std::to_string(prec) +"f";
	snprintf(s, sizeof(s), fmt.c_str(), v);

	return string{s};
}

// compiler gets a bit arsey about tmpname, but I want a temporary name anyway
blang_expr_t eval_tmpnam (blang_exprs_t args)
{

	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
	auto seed =  static_cast<uint64_t>(nanoseconds.count());
	srand(seed);

	string fname = "/tmp/blang-";
	for(int i = 0; i < 6; i++) {
		fname += ('a'+rand()%26);
	}

	return fname;
}

blang_expr_t eval_file_del (blang_exprs_t args)
{
	string path = blang_to_string(eval(args[0]));
	return std::filesystem::remove(path);
}

blang_expr_t eval_file_write (blang_exprs_t args)
{
	string path = blang_to_string(eval(args[0]));
	string contents = blang_to_string(eval(args[1]));

	return spit(path, contents);

}

blang_expr_t eval_file_read (blang_exprs_t args)
{
	string path = blang_to_string(eval(args[0]));
	return slurp(path.c_str());
}


// These will be augmented by user defined functions using eval_userfn
map<string, blang_function_t> blang_funcmap= {
		{"concat", eval_concat},
		{"file_del", eval_file_del},
		{"file_read", eval_file_read},
		{"file_write", eval_file_write},
		{"str", eval_str},
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
	{"print", eval_print},
	{"system", eval_system},
	{"tmpnam", eval_tmpnam}
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// LEXER


std::string repr(const token_t& t)
{
	std::string result = "<token(type=" ;
	int ty = t.type;
	std::string typestr = string{(char)ty};
	switch(ty) {
	case EOI:  typestr = "EOI"; break;
	case NUMBER: typestr = "NUMBER"; break;
	case ID: typestr = "ID"; break;
	case STR: typestr = "STR"; break;
	case SUB: typestr = "SUB"; break;
	case CALL: typestr = "CALL"; break;
	case LET: typestr = "LET"; break;
	case IF: typestr = "IF"; break;
	case ELSE: typestr = "ELSE"; break;
	case WHILE: typestr = "WHILE"; break;
	case VAR: typestr = "VAR"; break;
	}
	result += typestr + ", text=" + t.text + ", lineno=" + std::to_string(t.lineno) + ">";
	//if(ty == EOI = 128, NUMBER, ID, STR, SUB, CALL, LET, IF, ELSE, WHILE, VAR
	return result;
}

#if 0
enum Tokens { EOI = 128, NUMBER, ID, STR, SUB, CALL, LET, IF, ELSE, WHILE };

typedef struct {
	enum Tokens type;
	string text;
	int lineno;
} 	token_t;
#endif

Lexer1::Lexer1(const std::filesystem::path& path)
{
    std::ifstream in;
    in.open(path, std::ifstream::in | std::ifstream::binary);
    std::stringstream sstr;
    sstr << in.rdbuf();
    in.close();
    std::string str = sstr.str();
    //cout << "Init with str " << str << endl;
    //istrm = std::make_unique<std::istringstream>(std::istringstream{str});
    input = str;
    is = new  std::istringstream(input);
    yylex(); // prime the pump, as it were
    //Lexer1::Lexer1(str);
    //return sstr.str();

	//istrm = std::make_unique<std::istringstream>(std::istringstream{input});
	//is& = std::istringstream{input};
}

Lexer1::Lexer1(const std::string& input)
{
	//istrm = std::make_unique<std::istringstream>(std::istringstream{input});
	this->input = input;
	is = new  std::istringstream(input);
    yylex(); // prime the pump, as it were
    //cout << repr(next) << "\n";

	//is& = std::istringstream{input};
}

/* You'd typically call it like:
 * 		Lexer1 lxr(&std::cin);
 */
Lexer1::Lexer1( std::istream* input)
{
	is = input;
    yylex(); // prime the pump, as it were

	//is = input;
	//istrm = std::make_unique<std::istream>(&input);
}

char Lexer1::peek_char()
{
#if 1
	return is->peek();
#else
	if(is)
		return is->peek();
	else
		return istrm->peek();
#endif
}

char Lexer1::get_char()
{
#if 1
	return is->get();
#else
	if(is)
		return is->get();
	else
		return istrm->get();
#endif
}

bool Lexer1::good()
{
#if 1
	return is->good();
#else
	if(is)
		return is->good();
	else
		return istrm->good();
#endif
}

bool Lexer1::isfirst(string c)
{
	return peek().text == c;
}

bool Lexer1::eof()
{
	bool result = peek().type == EOI;
	//cout << "Lexer1::eof=" << result << "\n";
	return result;
}

token_t Lexer1::found(int type, const string& text)
{
	token_t t{(Tokens) type, text, lineno};
	next = t;
	//cout << "Lexer1::found " << repr(t) << endl;
	return t;
}

token_t Lexer1::varname()
{
	get_char(); // eat $
	std::string token;
	if(!isalpha(peek_char())) throw BlangException("Error lexing variable name: must beging with a letter. Found "
			+ string{peek_char()} + ", line: " + blang_to_string(lineno));
	while(isalnum(peek_char())  || peek_char() == '_') token += get_char();
	return found(VAR, token);
}

token_t Lexer1::quoted_string()
{
	std::string token;
	get_char(); // eat double-quote

	char c;
	while(1) {
		c = get_char();
		if(c == '\"' || c == '\n' || !good()) break;
		if(c == '\\') {
			char c1 = get_char();
			switch(c1) {
			case 0: c = '?'; break;
			case -1: c = '?'; break;
			case 'n' : c = '\n'; break;
			case 't' : c = '\t'; break;
			default: c = c1;
			}
		}
		token += c;
	}
	return found(STR, token);
}

token_t Lexer1::yylex()
{
	//cout << "yylex called\n";

	// comment and white space stuff
	unneeded:
	while(isspace(peek_char())) {
		if(get_char() == '\n') lineno++;
	}
	if(peek_char() == '#') {
		// eat comments
		while(peek_char() != '\n' && good()) get_char();
		goto unneeded; // there may be multiple comments
	}



	if(!peek_char() || !good()) return found(EOI, "EOI");
	//cout << "peek " << get() << endl;

	std::string token;


	if(isalpha(peek_char())) {
		while(isalnum(peek_char())  || peek_char() == '_') token += get_char();
		if(token == "sub") {
			return found(SUB, token);
		} else if(token == "if") {
			return found(IF, token);
		} else if(token == "else") {
			return found(ELSE, token);
		} else if (token == "call") {
			return found(CALL, token);
		} else if (token == "let") {
			//cout << "Found LET" << endl;
			return found(LET, token);
		} else if (token == "while") {
			return found(WHILE, "while");
		} else {
			return found(ID, token);
		}
	}

	//cout << "X1\n";
	if(isdigit(peek_char())) {
		while(1) {
			int ch = peek_char();
			if(isdigit(ch) || ch == '.' )
				token += get_char(); // i.e. ch
			else
				break;
		}
		//std::cout << "Lexer1::yylex found NUMBER " <<  token << endl;
		return found(NUMBER, token);
	}

	if(peek_char() == '\"') return quoted_string();

	if(peek_char() == '$') return varname();

	//if(peek() == '#')

	//if()
	char c = get_char();
	//if(c==-1) return "EOI";
	//cout << "getting " << (int)c << endl;
	return found(c, string{c});

}

token_t Lexer1::peek()
{
	return next;
}
token_t Lexer1::get()
{
	token_t t{next};
	next = yylex();
	return t;
}

void Lexer1::print_tokens(){
	//std::string t;
	while(1) {
		token_t t{get()};
		if(t.type == EOI) break;
		cout << repr(t) << endl;
	}
}

Lexer1::~Lexer1()
{
	if(is && is != &std::cin) delete(is);
}




#if 0
typedef deque<token_t> tokens_t;

// FN Lexer .
class [[deprecated("Use the more recent lexer")]] BlangLexer {
public:
	BlangLexer(string input);
	//token_t yylex();
	bool isfirst(string s);
	void consume(string s);
	token_t front();
	token_t pop_front();
	void push_front_XXX(token_t t); // for when you want to undo a peek
	bool eof();
	~BlangLexer();

private:
	void tokenise(const string& str);
	void found(int type, const string& text);
	int lineno = 1;
	tokens_t tokens;
	//string m_input;
};
// FN-END



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
	token_t t{(Tokens) type, text, lineno};
	tokens.push_back(t);
}


void BlangLexer::consume(string s)
{
	if(isfirst(s))
		tokens.pop_front();
	else
		throw BlangException("consume: looking for " + s + ", but found " + tokens.front().text);
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
		while(isspace(ch)) {
			if(ch == '\n') lineno++;
			ch = cstr[++pos];
		}
	} else if ( isdigit(ch)) {
		while(isdigit(ch) || ch == '.' ) { token += ch; ch = cstr[++pos]; }
		found(NUMBER, token);
	} else if (ch == '$' && isalpha(cstr[pos+1])) {
		//cout << "found $" <<endl;
		pos++;
		found('$', "$"); // start of a variable name
	} else if (isalpha(ch)) {
		while(ch && (isalnum(ch) || ch == '_')) { token += ch; ch = cstr[++pos]; }
		if(token == "sub") {
			found(SUB, token);
		} else if(token == "if") {
			found(IF, token);
		} else if(token == "else") {
				found(ELSE, token);
		} else if (token == "call") {
			found(CALL, token);
		} else if (token == "let") {
			//cout << "Found LET" << endl;
			found(LET, token);
		} else if (token == "while") {
			found(WHILE, "while");
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
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// SCANNER (the "yacc" side of things)



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



BlangParser::BlangParser(Lexer1& lxr) : lxr{lxr}
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

void BlangParser::parser_error(std::string msg)
{
	//msg += " line " + to_string(lxr.lineno);
	throw BlangException(msg);
}

void BlangParser::consume(string s)
{
	token_t t = lxr.peek();
	if(t.text == s)
		lxr.get();
	else
		throw BlangException("parser error: consume: looking for " + s + ", but found '" + t.text + "', line: "+ blang_to_string(t.lineno));

	//lxr.consume(s);
}



// FN parse_block .
blang_expr_t BlangParser::parse_block ()
{
	funcall_t fc;
	fc.fn = eval_block;
#if 1
	while(lxr.peek().type != '}')
		fc.exprs.push_back(parse_e());
	consume("}");

#else
	while(1) {
		auto type = lxr.front().type;
		//cout << "Parser::parse_block looping with text " << lxr.front().text << "\n";
		if(type == '}') break;
		fc.exprs.push_back(parse_e());
	}
	consume("}");
#endif
	return fc;
}
// FN-END

// FN parse_bra .
blang_expr_t BlangParser::parse_bra ()
{
	blang_expr_t x = parse_e();
	consume(")");
	return x;

}
// FN-END

// FN parse_fncall .
// 'parse_p' has already identified a function, and its name
blang_expr_t BlangParser::parse_fncall (string func_name)
{
	funcall_t fc;
	fc.fn = *blang_fn_lookup(func_name);
	consume("(");

	while(1) {
		if(lxr.isfirst(")")) break;
		fc.exprs.push_back(parse_e());
		if(lxr.isfirst(",")) consume(",");
	}

	consume(")");
	return fc;
}
// FN-END

// FN parse_let .
// LET $foo = 1 +2
blang_expr_t BlangParser::parse_let ()
{
	//cout << "parse_let called" << endl;
	//consume("$");
	string varname = lxr.get().text;
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
	string sub_name = lxr.get().text;
	//cout << "sub name is " << sub_name << endl;

	usr_funcall_t ufc;
	ufc.func_name = sub_name;
	ufc.codeblock.push_back(parse_e());
	usr_funcmap[sub_name] = ufc;
	return monostate{};
}
// FN-END

blang_expr_t BlangParser::parse_if ()
{
	funcall_t fc;
	fc.fn = eval_if;
	fc.exprs.push_back(parse_e()); // condition
	fc.exprs.push_back(parse_e()); // then clause
	if(lxr.peek().type == ELSE) {
		lxr.get();
		fc.exprs.push_back(parse_e());
	}
	return fc;
}


// FN parse_varname .
// This is where we get the value of a variable
blang_expr_t BlangParser::parse_varname (const std::string& varname)
{
	//string varname = lxr.get().text;
	//cout << "parse_varname is " << varname << endl;
	funcall_t fc;
	fc.fn = eval_getvar;
	fc.exprs = {varname};
	return fc;
}

blang_expr_t BlangParser::parse_while()
{
	funcall_t fc;
	fc.fn = eval_while;
	fc.exprs.push_back(parse_e()); // test
	fc.exprs.push_back(parse_e()); // body
	return fc;
}


// FN parse_p .
// P -> V | ( E ) | -T | FN
blang_expr_t BlangParser::parse_p ()
{
//#define parse_x parse_e
	token_t toke{lxr.get()};
	switch((int)toke.type) {
	case NUMBER: dbx("parse_p pushing NUMBER " + toke.text); return stof(toke.text);
	case VAR:	return parse_varname(toke.text);
	case IF: 	return parse_if();
	case STR: 	dbx("parse_p STR of <" + toke.text + ">" ) ; return toke.text;
	case ID: 	return parse_fncall(toke.text);
	case CALL: 	return parse_call();
	case LET: 	dbx("parse_p found LET"); return parse_let();
	case WHILE:	return parse_while();
	case '(': 	return parse_bra();
	case '{': 	return parse_block();
	default: 	throw BlangException("Error parsing primitive: unrecognised token '" + toke.text + "', line " + std::to_string(toke.lineno));
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
			auto type = lxr.peek().type;
			if(type == EOI) { dbx("parse_t breaking"); break;}
			if(type == '*' || type == '/') {
				lxr.get();
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
		auto type = lxr.peek().type;
		if(type == EOI) {dbx("parse_e breaking"); break;}
		//if(type == '{') return parse_block();
		if(type == '+' || type == '-') {
			lxr.get();
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
	string sub_name = lxr.get().text;
	auto ufc = usr_funcmap[sub_name];
	return ufc;
}
// FN parse_top .
blang_expr_t BlangParser::parse_top ()
{
	//cout << "parse_top " << lxr.front().type << endl;
	switch(lxr.peek().type ) {
	case SUB: return parse_defsub();
	case EOI: return monostate{};
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

blang_num_t blang_to_num (blang_expr_t val)
{
	if(holds_alternative<monostate>(val)) return 0; // don't use get_if() in this case ∵ an unused var will be generated otherwise
	if(auto v = std::get_if<blang_num_t>(&val)) 	return *v;
	if(auto v = std::get_if<int>(&val)) 	return *v;
	//if(auto v = std::get_if<std::string>(&val)) 	return "\""s + *v + "\""s;
	throw std::logic_error("blang_to_num: Unhandled stringify expression type index " + std::to_string(val.index()));
}


blang_num_t num_eval (blang_expr_t expr) { return blang_to_num(eval(expr)); }
string to_str (blang_expr_t v) { return std::get<string>(v); }
string str_eval (blang_expr_t expr) { return to_str(eval(expr)); }
///////////////////////////////////////////////////////////////////////////////////////////////////

blang_expr_t interpret_string(const string& s)
{
	//cout << "interpret_string called\n";
	Lexer1 lxr(s);
	//cout << "print tokens\n";
	//lxr.print_tokens();
	BlangParser p(lxr);
	blang_expr_t result; // of last expression
	while(!lxr.eof()) {
		blang_expr_t expr{p.parse_top()};
		result = eval(expr); // this evaluated precisely 1 expression
	}
	return result;
}

blang_expr_t interpret_file(const string& path)
{
	//cout << "interpret_string called\n";
	std::filesystem::path pth(path);
	Lexer1 lxr(pth);
	//cout << "print tokens\n";
	//lxr.print_tokens();
	BlangParser p(lxr);
	blang_expr_t result; // of last expression
	while(!lxr.eof()) {
		blang_expr_t expr{p.parse_top()};
		result = eval(expr); // this evaluated precisely 1 expression
	}
	return result;
}

// This is not quite right because it is always expecting another input
blang_expr_t interpret_cin()
{
	blang::Lexer1 lxr(&std::cin);
	BlangParser p(lxr);
	blang_expr_t result; // of last expression
	while(!lxr.eof()) {
		blang_expr_t expr{p.parse_top()};
		result = eval(expr); // this evaluated precisely 1 expression
	}
	return result;

}




} // namespace blang

