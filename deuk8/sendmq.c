#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 16 
#define QKEY (key_t)0111

struct msgq_data { 
	long type; 
	char text[BUFSIZE]; 
} ; struct msgq_data send_data = {1, "Hello, world"};

int main() { 
	int qid, len; 
	char buf[BUFSIZE];

	if ((qid = msgget(QKEY, IPC_CREAT | 0666)) == -1) { 
		perror ("msgget failed"); 
		exit (1); 
	}

	if (msgsnd(qid,&send_data,strlen(send_data.text),0) == -1) { 
		perror ("msgsnd failed");
		 exit (1); 
	} 

	return 0;
}
