#include <iostream>
#include <map>
#include <regex>
#include <string>

#include "alt-lex.h"

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::string_view;
using namespace std::string_view_literals;

const static map<LexType, string_view> typenames = {
	{LT_FLT, "FLT"sv},
	{LT_ASS, "ASS"sv},
	{LT_OPE, "OPE"sv},
	{LT_OPR,  "OPR"sv},
	{LT_OPP, "OPP"sv},
	{LT_OPT, "OPT"sv},
	{LT_LRB, "LRB"sv},
	{LT_ID,  "ID"sv},
	{LT_REM, "REM"sv},
	{LT_STR, "STR"sv},
	{LT_VAR, "VAR"sv},
	{LT_UNK, "UNK"sv},
	{LT_EOF, "EOF"sv}
};

std::string_view lextypename(LexType ltype)
{
	return typenames.at(ltype);
}



/*
lexemes_c::lexemes_c(lexemes lexs) : lexs(lexs)
{
	len = lexs.size();
}

void lexemes_c::advance() { idx++;}

std::string lexemes_c::curr() { return idx<len? lexs[idx].value : "" ;}

LexType lexemes_c::curr_type() { return idx<len? lexs[idx].type : LT_EOF ;}

void lexemes_c::require(std::string s) { 
	if(this->curr() != s) 
		throw std::runtime_error("#PARSE_ERR: Expecting " + s + ", got " + this->curr()); 
}
*/


//bool lexemes_c::empty() {return idx >= len; }

//token lexemes_c::curr_lex() { return lexs[idx]; }

std::string curr(tokens& tokes) {return tokes.empty() ? "" : tokes.front().value; }
token take(tokens& tokes) { token toke = tokes.front(); tokes.pop_front(); return toke; }


void advance(tokens& tokes) { tokes.pop_front();}

/*
token take(tokens& tokes) 
{ 
	token toke = tokes.curr_lex();
	tokes.advance();
	return toke;
	//token toke = tokes.front(); tokes.pop_front(); return toke; 
}
*/

void require(tokens& tokes, std::string required) 
{ 
	auto toke = take(tokes);
	string found = toke.value; 
	if(found != required)
		throw std::runtime_error("#PARSE_ERR: Required:" + required + ",found:" + found);
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




tokens alt_yylex_a(const std::string& s)
{
	tokens lexes;
	typedef std::vector<Re> Res;
	static Res regexes = { 
		Re(":=", LT_ASS, "ass"),
		Re("[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?", LexType::LT_FLT, "float"),
		Re("=|!=|<=?|>=?", LT_OPE, "ope"),
		Re("\\+|\\-", LT_OPR, "opr"),
		Re("\\*|/", LT_OPT, "opt"),
		// Re("[Rr][0-9]+[Cc][0-9]", "rc"),
		Re("\\?[a-zA=Z]+", LT_VAR, "var"),
		Re("[a-zA-z]+", LexType::LT_ID, "id"),
		Re("\"(?:[^\"\\\\]|\\\\.)*\"", LT_STR, "str"),
		Re("'(?:[^'\\\\]|\\\\.)*'", LT_STR, "str"),
		Re("#.*\\n", LT_REM, "rem"),
		Re("\\(", LT_LRB, "lrb"),
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
					if(areg.lextype != LT_REM)
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
	tokens lexes = alt_yylex_a(s);
	for(const auto& l:lexes)
	       	cout << "match:" << type_map[l.type] << ":" << l.value << "\n";
	cout << endl;
}

