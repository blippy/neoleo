#pragma once

#include <string>
#include <vector>

// TODO include power, ege. 12^5

enum LexType { LT_FLT , 
	LT_OPE, // = != <= < >= >  
	LT_OPR, // + -
	LT_OPP, // um ..
	LT_OPT, //  * /
	LT_ID, 
	LT_VAR,
	LT_UNK,
	LT_EOF // end of file
};


typedef struct {
	LexType lextype;
	std::string lexeme;
} lexeme_s;


typedef std::vector<lexeme_s> lexemes;

class lexemes_c {
	public:
		lexemes_c(lexemes lexs);
		void advance();
		std::string curr();
		LexType curr_type();
		void require(std::string s); 
	private:
		lexemes lexs;
		int idx = 0, len;
};

lexemes alt_yylex_a(const std::string& s);
void lex_and_print(std::string s);

