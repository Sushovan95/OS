#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include <sys/sem.h> /* for semget(2) semop(2) semctl(2) */
#include <errno.h> /* for perror */
#include<unistd.h> // for fork()
#include<sys/wait.h> // for wait()
#include<sys/types.h> // for wait() kill(2)
#include <sys/ipc.h> /* for shmget() shmctl() */
#include <sys/shm.h> /* for shmget() shmctl() */


#define NO_SEM	1

#define P(s) semop(s, &Pop, 1);
#define V(s) semop(s, &Vop, 1);


int semid,status;
struct sembuf Pop;
struct sembuf Vop;

union semun 
	{
		int              val;    /* Value for SETVAL */
		struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
		unsigned short  *array;  /* Array for GETALL, SETALL */
		struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
	} setvalArg;

	setvalArg.val = 1;


	/* struct sembuf has the following fields */
		//	unsigned short sem_num;  /* semaphore number */
        //	short          sem_op;   /* semaphore operation */
        //	short          sem_flg;  /* operation flags */

	Pop.sem_num = 0;
	Pop.sem_op = -1;
	Pop.sem_flg = 0;

	Vop.sem_num = 0;
	Vop.sem_op = 1;
	Vop.sem_flg = 0;

key_t mykey = ftok("/bin/ls", 1);
semid = semget(mykey, NO_SEM, IPC_CREAT | 0777);
status = semctl(semid, 0, SETVAL, setvalArg);

struct Node
{
	int data;
	int next;
};

typedef struct Node node;

int searchnode(int* head_ref,int data)
{
	P(semid);

	int current = *head_ref;

	if(*head_ref == -1)
	{
		V(semid);
		return 0;
	}
	else
	{
		node* new;
		new = (node*) shmat(current, NULL, 0);
	    while(current != -1)
	    {	
	    	if (new->data == data)
	    	{
	    		V(semid);
	    		return 1;
	    	}
	    	current = new->next;
	    	new = (node*) shmat(current, NULL, 0);
		}

		V(semid);
		return 0;
	}



}


void insertfirst(int* head_ref,int data)
{
	P(semid);
  
  int shmid =  shmget(IPC_PRIVATE, sizeof(node), IPC_CREAT | 0777);
  node* pi_child;
  pi_child = (node*) shmat(shmid, NULL, 0);
  
  pi_child->data = data;
  pi_child->next = *head_ref;
  *head_ref = shmid;
  printf("shmid=%d with value = %d\n",shmid,data);
  shmdt(pi_child);

  V(semid);
}

void insertlast(int* head_ref,int data)
{
	P(semid);
	int shmid =  shmget(IPC_PRIVATE, sizeof(struct Node), IPC_CREAT | 0777);
	int last = *head_ref;
	
  	if(*head_ref == -1)
  	{
  		node* pi_child;
  		pi_child =(node*) shmat(shmid, NULL, 0);
  		pi_child->data = data;
  		pi_child->next = *head_ref;
  		*head_ref = shmid;
  		shmdt(pi_child);
  	}
  	else
  	{
  		node* new;
  		new = (node*)shmat(last,NULL,0);
  		
  		while(new->next != -1)
  		{  	
  			last = new->next;
  			new = (node*)shmat(last,NULL,0);		
  			
  		}
  		
  		node* pi_child;
  		pi_child = (node*)shmat(shmid, NULL, 0);

  		new->next = shmid;
  		pi_child->data = data;
  		pi_child->next = -1;
  		
  		shmdt(pi_child);

  	}

  	V(semid);
}

void insertafter(int* head_ref,int data,int value)
{
	P(semid);
	
	int shmid =  shmget(IPC_PRIVATE, sizeof(struct Node), IPC_CREAT | 0777);
	int last = *head_ref;
	if(*head_ref == -1)
  	{
  		printf("THE LIST IS EMPTY\n");
  	}
	else if (searchnode(&last,data))
	{ 
  		node* new;
  		new = (node*)shmat(last,NULL,0);
  		printf("DATA OF NEW ==== %d\n",new->data);
  		while(new->data!= data)
  		{  		
		    last = new->next;
		    new = (node*)shmat(last,NULL,0);
		    printf("DATA OF AFTERADD ==== %d\n",new->data);
  		}

  		node* pi_child;
  		pi_child = (node*)shmat(shmid, NULL, 0);
   		pi_child->data = value;
  		pi_child->next = new->next;
  		new->next = shmid;
  		shmdt(pi_child);

  	}
  else
  {
  	printf("NODE NOT FOUND....CAN NOT BE ADDED\n");
  }

  V(semid);

}

void deletenode(int* head_ref,int data)
{
	P(semid);
	int current = *head_ref;
	if (*head_ref == -1)
	{
		printf("THE LIST IS EMPTY\n");
	}
	else if(searchnode(&current,data))
	{
		node* new;
		node* newnode;
		new = (node*)shmat(current,NULL,0);
		if(new->data == data)
		{
			*head_ref = new->next;
			shmctl(current,IPC_RMID, NULL);
		}
		else
		{
			while(new->data != data)
			{
				newnode = (node*)shmat(current,NULL,0);
				current = new->next;
				new = (node*)shmat(current,NULL,0);
			}

		shmctl(newnode->next, IPC_RMID, NULL);
		newnode->next = new->next;
	}
	}
	else
	{
		printf("NODE NOT FOUND...CAN NOT BE DELETED\n");
	}

	V(semid);
	
}

void display(int* head_ref)
{
	P(semid);
	int current = *head_ref;
	if (*head_ref == -1)
	{
		printf("LIST IS EMPTY\n");
	}
	else
	{	
		node* new;
		new = (node*)shmat(current,NULL,0);
		if (new->next == -1 )
		{
			printf("VALUE = %d\n",new->data);
		}
		else
		{
	
			while(new->next!=-1)
			{
				new = (node*)shmat(current,NULL,0);
				printf("VALUE = %d\n",new->data);
				current = new->next;
			}
		}
	
	}

	V(semid);

}



int main()
{
	int n;
	int i;
	printf("HOW MANY LINKED-LIST YOU WANT TO CREATE\n");
	scanf("%d",&n);
	
	int headarr[n];

	for(i=1;i<=n;i++)
	{
		headarr[i] = -1;

	}

	insertlast(&headarr[1],25);
	insertfirst(&headarr[1],5);
	insertlast(&headarr[2],6);
	insertfirst(&headarr[1],7);
	insertlast(&headarr[2],8);
	insertafter(&headarr[1],7,10);
	insertafter(&headarr[2],8,50);
	deletenode(&headarr[2],25);
	deletenode(&headarr[1],8);
	printf("search for 8 == %d\n",searchnode(&headarr[2],8));
	printf("%d\n",searchnode(&headarr[1],7));
	deletenode(&headarr[1],7);
	printf("%d\n",searchnode(&headarr[1],7));
	display(&headarr[1]);
	printf("VALUES OF LINKED LIST 2\n");
	display(&headarr[2]);

	
	return 0;
}