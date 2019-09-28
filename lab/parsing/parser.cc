#include <ctype.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef pair<int, string> token_t;
typedef vector<token_t> tokens_t;

enum Tokens { UNK, NUMBER, ID };

tokens_t tokenise(string str)
{
	tokens_t tokens;

	auto found = [&tokens](auto toketype, auto token) { tokens.push_back(make_pair(toketype, token)); };
	cout << "Parsing: " << str << "\n";
	const char* cstr = str.c_str();
	int pos = 0;
	auto it = str.begin();
loop:
	string token;
	char ch = cstr[pos];
	if(ch == 0) {
		goto finis;
	} else if ( isdigit(ch)) {
		while(isdigit(ch) || ch == '.' ) { token += ch; ch = cstr[++pos]; }
		found(NUMBER, token);
		//tokens.push_back(make_pair(NUMBER, token));
		//cout <<  "found number: " << token << "\n";
	} else if (isalpha(ch)) {
		while(isalnum(ch)) { token += ch; ch = cstr[++pos]; }
		found(ID, token);
		cout << "found id: " << token << "\n";
	} else {
		token = ch;
		pos++;
		found(UNK, token);
		//cout << "found unknown: " << ch << "\n";
	}
	//if(token.size() 
	//pos++;
	goto loop;
finis:
	return tokens;
}

// a little test to show us that we can get recursive definitions
class FunCall;
class FunCall { public: vector <FunCall> funcs; };

int main()
{
	tokens_t tokes{tokenise("12.3+3*4")};
	for(auto& t:tokes) {
		cout << "Found: " << t.first << " " << t.second << "\n";
	}
}
