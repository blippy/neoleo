#pragma once

#include <deque>
//#include <vector>


// TODO include power, ege. 12^5

enum LexType { LT_FLT , 
	LT_ASS,
	LT_OPE, // = != <= < >= >  
	LT_OPR, // + -
	LT_OPP, // um ..
	LT_OPT, //  * /
	LT_LRB, // left round bracket
	LT_ID, 
	LT_REM,
	LT_STR,
	LT_VAR,
	LT_UNK,
	LT_EOF // end of file
};

typedef struct token {	LexType type;	std::string value;} token;

//using token = lexeme_s;


//typedef std::vector<lexeme_s> lexemes;
typedef std::deque<token> tokens;

/*
class lexemes_c {
	public:
		lexemes_c(lexemes lexs);
		void advance();
		std::string curr();
		LexType curr_type();
		void require(std::string s);
		bool empty();
		token curr_lex();
	private:
		lexemes lexs;
		int idx = 0, len;
};
*/

//typedef lexemes_c tokens;

std::string_view lextypename(LexType ltype);
std::string curr(tokens& tokes);
void require(tokens& tokes, std::string required);
token take(tokens& tokes);
void advance(tokens& tokes);
tokens alt_yylex_a(const std::string& s);
void lex_and_print(std::string s);
