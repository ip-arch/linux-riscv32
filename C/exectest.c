#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	pid_t pid;
	pid = fork();
	if(pid > 0) { wait(NULL); return 0;}
	else if(pid == 0)
	{
		char *args[] = {"/bin/ls", NULL};
		execv("/bin/ls", args);
		// ここに来たらexec()は失敗
		printf("RETURN from execv() \n");
		_exit(0);
	}
	else return 1; // fork() 失敗
}
