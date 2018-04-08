#pragma once

#include <vector>


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

typedef struct {
	LexType lextype;
	std::string lexeme;
} lexeme_s;

using token = lexeme_s;


typedef std::vector<lexeme_s> lexemes;

class lexemes_c {
	public:
		lexemes_c(lexemes lexs);
		void advance() { idx++;}
		std::string curr() { return idx<len? lexs[idx].lexeme : "" ;}
		LexType curr_type() { return idx<len? lexs[idx].lextype : LT_EOF ;}
		void require(std::string s) { 
			if(this->curr() != s) 
				throw std::runtime_error("#PARSE_ERR: Expecting " + s + ", got " + this->curr()); 
		}
		bool empty() {return idx >= len; }
		token curr_lex() { return lexs[idx]; }
	private:
		lexemes lexs;
		int idx = 0, len;
};

typedef lexemes_c tokens;

std::string_view lextypename(LexType ltype);
std::string curr(tokens& tokes);
void require(tokens& tokes, std::string required);
token take(tokens& tokes);
lexemes alt_yylex_a(const std::string& s);
void lex_and_print(std::string s);
