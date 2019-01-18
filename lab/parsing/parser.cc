#include <ctype.h>
#include <iostream>
#include <string>

using namespace std;

void tokenize(string str)
{
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
		cout <<  "found number: " << token << "\n";
	} else if (isalpha(ch)) {
		while(isalnum(ch)) { token += ch; ch = cstr[++pos]; }
		cout << "found id: " << token << "\n";
	} else {
		token = ch;
		pos++;
		cout << "found unknown: " << ch << "\n";
	}
	//if(token.size() 
	//pos++;
	goto loop;
finis:
	;
}

int main()
{
	tokenize("12.3+foo(3)");
}
