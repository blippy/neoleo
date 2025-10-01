#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PARENT_READ read_pipe[0]
#define PARENT_WRITE write_pipe[1]
#define CHILD_WRITE read_pipe[1]
#define CHILD_READ  write_pipe[0]

int read_pipe[2];
int write_pipe[2];

void send(char* str)	
{
	int x = 1;
	write(PARENT_WRITE, str, strlen(str)+1);
	//fsync(pipefd[x]);
}

void recv()
{
	char buf[100];
	read(PARENT_READ, buf, sizeof(buf));
	printf("%s", buf);

}
	
int main(int argc, char * argv[])
{
	pid_t cpid;
	char buf;

	//assert(pipe(fd1) == 0);
	//assert(pipe(fd2) == 0);

	pipe(read_pipe);
	pipe(write_pipe);

	cpid = fork(); // duplicate the current process
	if(cpid == -1) {
		perror("Couldn't fork");
	}
	if (cpid == 0) // if I am the child then
	{
		close(PARENT_READ);
		close(PARENT_WRITE);

		dup2(CHILD_READ, STDIN_FILENO);
		dup2(CHILD_WRITE, STDOUT_FILENO);
		close(CHILD_READ);
		close(CHILD_WRITE);
		execlp("./child.sh", "./child.sh", "foo", "bar", NULL);
		perror("Child went bad. This shouldn't reach here");
		exit(EXIT_SUCCESS);
	}
	else // if I am the parent then
	{
		close(CHILD_READ);
		close(CHILD_WRITE);
		send("first line\n");
		recv();
		send("second line\n");
		recv();
		send("q\n");
		//recv();
		wait(NULL); // wait for the child process to exit before I do the same
		close(PARENT_READ); 
		close(PARENT_WRITE); 
		exit(EXIT_SUCCESS);
	}
	return 0;
}
