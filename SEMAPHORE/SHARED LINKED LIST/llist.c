#include"sharedll.h"

#define NO_SEM	1

#define P(s) semop(s, &Pop, 1);
#define V(s) semop(s, &Vop, 1);

struct sembuf Pop;
struct sembuf Vop;

int searchnode(int* head_ref,int data)
{

	int current = *head_ref;

	if(*head_ref == -1)
	{

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
	    		return 1;
	    	}
	    	current = new->next;
	    	new = (node*) shmat(current, NULL, 0);
		}

		return 0;
	}

}


void insertfirst(int* head_ref,int data,key_t mykey)
{

  Pop.sem_num = 0;
	Pop.sem_op = -1;
	Pop.sem_flg = 0;

	Vop.sem_num = 0;
	Vop.sem_op = 1;
	Vop.sem_flg = 0;

	int semid = semget(mykey, NO_SEM, IPC_CREAT | 0777);

    P(semid);

    int shmid = shmget(IPC_PRIVATE, sizeof(node), IPC_CREAT | 0777); //creating shared memory for each node

  node* pi_child; //pointer to struct Node
  pi_child = (node*) shmat(shmid, NULL, 0);

  pi_child->data = data;
  pi_child->next = *head_ref;
  *head_ref = shmid;
  printf("shmid=%d with value = %d\n",shmid,data);

  shmdt(pi_child);

  V(semid);
}

void insertlast(int* head_ref,int data,key_t mykey)
{
	Pop.sem_num = 0;
	Pop.sem_op = -1;
	Pop.sem_flg = 0;

	Vop.sem_num = 0;
	Vop.sem_op = 1;
	Vop.sem_flg = 0;

	int semid = semget(mykey, NO_SEM, IPC_CREAT | 0777);

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

void insertafter(int* head_ref,int data,int value,key_t mykey)
{
	Pop.sem_num = 0;
	Pop.sem_op = -1;
	Pop.sem_flg = 0;

	Vop.sem_num = 0;
	Vop.sem_op = 1;
	Vop.sem_flg = 0;

	int semid = semget(mykey, NO_SEM, IPC_CREAT | 0777);

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

void deletenode(int* head_ref,int data,key_t mykey)
{

	Pop.sem_num = 0;
	Pop.sem_op = -1;
	Pop.sem_flg = 0;

	Vop.sem_num = 0;
	Vop.sem_op = 1;
	Vop.sem_flg = 0;

	int semid = semget(mykey,NO_SEM, IPC_CREAT | 0777);

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

void display(int* head_ref,key_t mykey)
{

	Pop.sem_num = 0;
	Pop.sem_op = -1;
	Pop.sem_flg = 0;

	Vop.sem_num = 0;
	Vop.sem_op = 1;
	Vop.sem_flg = 0;

	int semid = semget(mykey, NO_SEM, IPC_CREAT | 0777);

	P(semid);

	int current = *head_ref;
	if (*head_ref == -1)
	{
		printf("LIST IS EMPTY\n");
	}
	else
	{
		printf("VALUES OF LINKED-LIST:\n");
		node* new;
		new = (node*)shmat(current,NULL,0);

		while(current!=-1)
			{
				new = (node*)shmat(current,NULL,0);
				printf("VALUE = %d\n",new->data);
				current = new->next;
			}

	}

	V(semid);

}
