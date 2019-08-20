#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h> /* for fork() */
#include<sys/wait.h> /* for wait() */
#include<sys/types.h> /* for wait() kill(2)*/
#include<sys/ipc.h> /* for shmget() shmctl() */
#include<sys/shm.h> /* for shmget() shmctl() */
#include<signal.h> /* for signal(2) kill(2) */
#include<errno.h> /* for perror */

#define min_number 510515001
#define max_number 510515020
int shmid;
int stop = 0;
typedef void (*sighandler_t)(int);

// A linked list node
struct Node
{
  int data;
  int processid;
  struct Node *next;
};

struct Node* head;
// This function prints contents of linked list starting from head
void insertlast(struct Node** head_ref, int new_data,int processid)
{

    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
    struct Node* last = *head_ref;
    new_node->data  = new_data;//This new node is going to be the last node, so make next  of it as NULL
    new_node->processid = processid;
    new_node->next = NULL;//If the Linked List is empty, then make the new node as head
    if (*head_ref == NULL)
    {
       *head_ref = new_node;
       return;
    }
    // 5. Else traverse till the last node
    while (last->next != NULL)
        last = last->next;// Change the next of last node
    last->next = new_node;
    new_node->next = NULL;
    return;
}

// Checks whether the value x is present in linked list
bool search(struct Node* head, int x)
{
    struct Node* current = head;// Initialize current
    while (current != NULL)
    {
        if (current->data == x)
            return true;
        current = current->next;
    }
    return false;
}

void display(struct Node *node)
{
  if(node == NULL)
  {
    printf("NOBODY ATTENDED THE CLASS....!!\n");
  }
  while (node != NULL)
  {
     printf("ENROLLMENT ID: %d WITH PROCESS-ID = %d\n",node->data,node->processid);
     node = node->next;
  }
}
void releaseSHM(int signum)
{
  stop = 1;
  printf("------------------CLASS IS OVER--------------------\n");
  printf("\nLIST OF STUDENTS WHO ATTENDED THE CLASS:\n");
  display(head);
  int status;
  // int shmctl(int shmid, int cmd, struct shmid_ds *buf);
  status = shmctl(shmid, IPC_RMID, NULL); /* IPC_RMID is the command for destroying the shared memory*/
  if (status == 0)
  {
      fprintf(stderr, "Shared memory has been removed with id = %d.\n", shmid);
  }
  else if (status == -1)
  {
      fprintf(stderr, "Cannot remove shared memory with id = %d.\n", shmid);
  }
  else
  {
      fprintf(stderr, "shmctl() returned wrong value while removing shared memory with id = %d.\n", shmid);
  }
}


int main()
{

  sighandler_t shandler;  // sighandler_t signal(int signum, sighandler_t handler);
  shandler = signal(SIGINT, releaseSHM); /* should we call this seperately in parent and child process */

  head = NULL;
  key_t key = ftok("/bin/ls",1);
  if ( key>0 )
    {
       perror("ftok"); /*Displays the error message*/
       /*Error handling. Return with some error code*/
    }

  else /* This is not needed, just for success message*/
    {
       printf("ftok success\n");
    }

  shmid =  shmget(key , sizeof(int), IPC_CREAT | 0777);
  if (shmid == -1)
  { /* shmget() failed() */
          perror("shmget() failed: ");
          exit (1);
  }
  else
  {
    printf("shmget() returns shmid = %d.\n", shmid);
  }

  int *ppi_child;
  ppi_child = shmat(shmid, NULL, 0);// void *shmat(int shmid, const void *shmaddr, int shmflg);
  *ppi_child = -1;
  shmdt(ppi_child);//detaching the main program from the shared memory

  int *pi_child;
  pi_child = shmat(shmid, NULL, 0);// void *shmat(int shmid, const void *shmaddr, int shmflg);
  if (pi_child == (void *)-1)
  { /* shmat fails */
          perror("shmat() failed at child: ");
          exit (1);
  }

while(!stop)
{

   if(*pi_child!= -1)
   {
     printf("VALUE OF STUDENT ID = %d\n",*pi_child);
     if(*pi_child>=min_number && *pi_child<=max_number)
     {
      if (search(head,*pi_child))
      {
        printf("DON'T RESPOND TWICE..!\n");
        *pi_child = -1;
      }
      else
      {
        struct shmid_ds strc,*buf;
        buf = &strc;
        shmctl(shmid,IPC_STAT,buf);
        int processid = buf->shm_lpid;
        insertlast(&head,*pi_child,processid);
        *pi_child = -1;
      }
   }
   
   else
   {
      printf("YOU ARE NOT FROM THIS CLASS.I CAN'T GIVE YOUR ATTENDENCE\n");
      *pi_child = -1;
   }

   }
   else
   {
      printf("I AM WAITING\n");
   }

    sleep(1);

 }

   return 0;

}
