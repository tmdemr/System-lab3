#include <sys/types.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() { 
	pid_t pid;
	pid = fork(); 

	if (pid == 0) { 
		execl("/bin/ls", "ls", "-l", (char *) 0); 
		perror("execl failed"); 
	} 

	else if (pid > 0) { 
		wait((int *) 0); printf("ls completed\n"); 
		exit(0); 
	} 

	else 
		perror("fork failed");

	return 0;
}
