/*this main.c will store the array of heads in the shared memory which to be used in multiple linkedlist operations.
user give the number of linkedlists to be created atmost and based on that all the heads will be stored at shared memory 
and their initial values will be -1.These heads will be shared by the each and every process which want to operate on 
Linkedlists.

This main.c will also set the semaphore values which will be used for mutual exclusion.
*/

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/sem.h> /* for semget(2) semop(2) semctl(2) */
#include<errno.h> /* for perror */
#include<unistd.h> // for fork()
#include<sys/wait.h> // for wait()
#include<sys/types.h> // for wait() kill(2)
#include<sys/ipc.h> /* for shmget() shmctl() */
#include<sys/shm.h> /* for shmget() shmctl() */
#define NO_SEM	1
#define MAX 1024

int main()
{
	
	union semun 
	{
		int              val;    /* Value for SETVAL */
		struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
		unsigned short  *array;  /* Array for GETALL, SETALL */
		struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
	} setvalArg;

	setvalArg.val = 1;


	int n;
	int i;
	int status1;
	printf("HOW MANY LINKED-LIST YOU WANT TO CREATE\n");
	scanf("%d",&n);
	key_t arr[n];
	int semid[n];

	for (i = 0; i < n; i++)
	{
		arr[i] = ftok("/bin/ls",i);
		
	}

	key_t mykey = ftok("/bin/ls",100);

	if (mykey == -1) 
	{
		perror("ftok() failed");
		exit(1);
	}


	for ( i = 0; i < n; ++i)
	{
		semid[i] = semget(arr[i], NO_SEM, IPC_CREAT | 0777);
		semctl(semid[i], 0, SETVAL, setvalArg);	
	}

	
	int shmid =  shmget(mykey,(n+1)*sizeof(int), IPC_CREAT | 0777);
	int* ptr;
	ptr = shmat(shmid,NULL,0);

	*ptr = n;

	for(i=1;i<=n;i++)
	{
		*(ptr+i) = -1;

	}
	

	shmdt(ptr);
	return 0;
}