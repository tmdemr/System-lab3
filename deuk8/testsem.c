#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/sem.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#define SEMKEY (key_t)0111 

void testsem(int semid); 
void p(int semid); 
void v(int semid);

int main() {
	int semid, i; 
	union semun { 
		int value; 
		struct semid_ds *buf; 
		unsigned short int *array; 
	} arg;

	if ((semid = semget(SEMKEY, 1, IPC_CREAT | 0666)) == -1) { 
		perror ("semget failed"); 
		exit (1); 
	} 
	arg.value = 1; 

	if (semctl(semid, 0, SETVAL, arg) == -1) { 
		perror ("semctl failed"); 
		exit (1); 
	} 

	for (i = 0; i < 3; i++) { 
		if (!fork()) testsem(semid); 
	} 

	sleep(10);

	if (semctl(semid, 0, IPC_RMID, arg) == -1) { 
		perror ("semctl failed"); 
		exit (1); 
	} 

	return 0;
}

void testsem (int semid) { 
	srand((unsigned int) getpid()); 
	p(semid); 
	printf("process %d : semaphore in use\n", getpid()); 
	sleep(rand()%5); 
	printf("process %d : putting semaphore\n", getpid()); 
	v(semid);
	exit(0); 
}

void p (int semid) { 
	struct sembuf pbuf; pbuf.sem_num = 0; 
	pbuf.sem_op = -1; pbuf.sem_flg = SEM_UNDO;

	if (semop (semid, &pbuf, 1) == -1) { 
		perror ("semop failed"); 
		exit (1);
	}
}

void v (int semid) { 
	struct sembuf vbuf; vbuf.sem_num = 0; 
	vbuf.sem_op = 1; 
	vbuf.sem_flg = SEM_UNDO; 

	if (semop (semid, &vbuf, 1) == -1) { 
		perror ("semop failed"); 
		exit (1); 
	}
 }
