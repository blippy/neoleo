#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>


using std::ofstream;
using std::cout;
using std::endl;
//using std::out;
using std::string;
using std::vector;



#include "cmd.h"
#include "shell.h"
#include "io-headless.h"

/*
void
exec_cmd(string line, int fildes)
{
#if 0
	// convert from const char* to char* 
	vector<char> v(line.begin(), line.end());
	v.push_back(0);
	char* cmd = &v[0];

	//execute_command(cmd);
#endif
	process_headless_line(line, fildes);
}
*/


std::string
getline_from_fildes(int fildes, bool& eof)
{
	/* TODO Account for use of `#' and line continuations `\'
	 * */

	char ch;
	string line;
	//ofstream ofs;
	//ofs.open("/tmp/oleo-shell", ofstream::out);
	while(true) {
		eof = read(fildes, &ch, 1) == 0;
		if(eof) return line;

		//cout << "getline_from_fildes:ch:" << ch << endl;
		if(ch == '\n') {
			//ofs << "line: " << line << endl;
			//exec_cmd(line, fildes);
			//line = "";
			return line;
		} else {
			line += ch;
		}
	}

	return line;
}


void
run_shell_output_commands(int fildes)
{
	bool eof;
	auto line = getline_from_fildes(fildes, eof);
	if(line.size() >0) process_headless_line(line, fildes); //ofs << "line: " << line << endl;
	//ofs.close();

}

/* see also read_cmds_cmd() for the inspiration of this function.
 * See TR02 for a discussion about this function, and creating
 * commands in general. 
 *
 * Piping taken from:
 * http://www.minek.com/files/unix_examples/execill.html
 */

void
run_shell(char* cmd)
{

	int pid;
	int pc[2]; /* Parent to child pipe */
	int cp[2]; /* Child to parent pipe */
	char ch;
	int incount = 0, outcount = 0;

	/* Make pipes */
	if( pipe(pc) < 0)
	{
		perror("Can't make pipe");
		exit(1);
	}
	if( pipe(cp) < 0)
	{
		perror("Can't make pipe");
		exit(1);
	}


	/* Create a child to run command. */
	switch( pid = fork() )
	{
		case -1:
			perror("Can't fork");
			exit(1);
		case 0:
			/* Child. */
			close(1); /* Close current stdout. */
			(void)dup( cp[1]); /* Make stdout go to write
					end of pipe. */
			close(0); /* Close current stdin. */
			(void)dup( pc[0]); /* Make stdin come from read
					end of pipe. */
			close( pc[1]);
			close( cp[0]);
			//execvp(argv[1], argv + 1, envp);
			//cout << "run_shell:cmd:" << cmd << "." << endl;
			execlp(cmd, cmd, (char *)0);	
			perror("No exec");
			//signal(getppid(), SIGQUIT);
			exit(1);
		default:
			/* Parent. */
			/* Close what we don't need. */
#if 0
			printf("Input to child:\n");
			while(  read(0, &ch, 1) > 0 )
			{
				write(pc[1],&ch, 1);
				write(1, &ch, 1);
				incount ++;
			}
			close(pc[1]);
			printf("\nOutput from child:\n");
			close(cp[1]);
			while( read(cp[0], &ch, 1) == 1)
			{
				write(1, &ch, 1);
				outcount++;
			}
			printf("\n\nTotal characters in: %d\n",incount);
			printf("Total characters out: %d\n", outcount);
			exit(0);
#endif			
			close(pc[1]);
			close(cp[1]);
			run_shell_output_commands(cp[0]);
			// TODO do we need to close(cp[0])?
			int status;
			waitpid(pid, &status, 0);
			break;
	}

}
