#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <fstream>
//#include <iostream>
#include <string>
#include <vector>


using std::ofstream;
using std::endl;
//using std::out;
using std::string;
using std::vector;


extern "C"
{

#include "cmd.h"
#include "shell.h"

void
exec_cmd(string line)
{
	/* convert from const char* to char* */
	vector<char> v(line.begin(), line.end());
	v.push_back(0);
	char* cmd = &v[0];

	execute_command(cmd);
}

void
run_shell_output_commands(int fildes)
{
	/* TODO Account for use of `#' and line continuations `\'
	 * */

	int ch;
	string line;
	//ofstream ofs;
	//ofs.open("/tmp/oleo-shell", ofstream::out);
	while( read(fildes, &ch, 1) == 1)
	{
		if(ch == '\n') {
			//ofs << "line: " << line << endl;
			exec_cmd(line);
			line = "";
		} else {
			line += ch;
		}
	}
	if(line.size() >0) exec_cmd(line); //ofs << "line: " << line << endl;
	//ofs.close();

}


} // extern "C"
