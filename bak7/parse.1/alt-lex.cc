#include <iostream>
#include <map>
#include <regex>
#include <stdexcept>
//#include <string>
//#include <vector>

#include "alt-lex.h"

using std::cout;
using std::endl;
using std::map;
using std::vector;



void lexemes_c::advance()
{ 
	idx++;
}


std::string lexemes_c::curr()
{ 
	return idx<len? lexs[idx].lexeme : "" ;
}


LexType lexemes_c::curr_type()
{ 
	return idx<len? lexs[idx].lextype : LT_EOF ;
}


void lexemes_c::require(std::string s) 
{ 
	if(this->curr() != s) 
		throw std::runtime_error("#PARSE_ERR: Expecting " + s + ", got " + this->curr()); 
}

lexemes_c::lexemes_c(lexemes lexs) : lexs(lexs)
{
	len = lexs.size();
}




static map<LexType, std::string> type_map;

class Re {

	public:
		Re(const std::string& str, LexType lextype, const std::string& name);
		LexType lextype;
		std::regex re;
		std::string name;
};

Re::Re(const std::string& str, LexType lextype, const std::string& name) : lextype(lextype), name(name)
{
	re = std::regex( "^(" + str + ")" );
	type_map[lextype] = name;
}




lexemes alt_yylex_a(const std::string& s)
{
	lexemes lexes;
	typedef std::vector<Re> Res;
	static Res regexes = { 
		Re("[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?", LexType::LT_FLT, "float"),
		Re("=|!=|<=?|>=?", LT_OPE, "ope"),
		Re("\\+|\\-", LT_OPR, "opr"),
		Re("\\*|/", LT_OPT, "opt"),
		// Re("[Rr][0-9]+[Cc][0-9]", "rc"),
		Re("\\?[a-zA=Z]+", LT_VAR, "var"),
		Re("[a-zA-z]+", LexType::LT_ID, "word"),
		Re(".", LexType::LT_UNK, "unknown")

	};


	std::smatch m;

	auto i0 = s.begin();
	auto iX = s.end();
	while(i0 < iX) {
		while(i0<iX && std::isspace(*i0)) i0++; // eat white
		if(i0 != iX) {
			std::string s2(i0, iX);
			//int matched_size = 0;
			std::string matched_str;
			for(const auto& areg: regexes) {
				if(std::regex_search(s2, m, areg.re)) {
					matched_str = m[0];
					lexes.push_back({areg.lextype, matched_str});
					break;
				}
			}
			i0 += matched_str.size();
		}
	}	

	return lexes;
}

void lex_and_print(std::string s)
{
	cout << "lex_and_print:input:`" << s << "'\n";	
	lexemes lexes = alt_yylex_a(s);
	for(const auto& l:lexes)
	       	cout << "match:" << type_map[l.lextype] << ":" << l.lexeme << "\n";
	cout << endl;
}

// lexical analysis
