#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include <errno.h> ///* for perror
#include<unistd.h> // for fork()
#include<sys/wait.h> // for wait()
#include<sys/types.h> // for wait() kill(2)
#include <sys/ipc.h> // for shmget() shmctl() 
#include <sys/shm.h> // for shmget() shmctl()
#include <sys/sem.h> //for semget()

struct Node
{
	int data; //value that we want to keep in a node
	int next; //this will contain the shmid of the next node
};

typedef struct Node node;
int searchnode(int* head_ref,int data);
void insertfirst(int* head_ref,int data,key_t mykey);
void insertlast(int* head_ref,int data,key_t mykey);
void insertafter(int* head_ref,int data,int value,key_t mykey);
void deletenode(int* head_ref,int data,key_t mykey);
void display(int* head_ref,key_t mykey);
