#include <stdio.h>
#include <unistd.h> /* for fork() */
#include <sys/wait.h> /* for wait() */
#include <sys/types.h> /* for wait() kill(2)*/
#include <sys/ipc.h> /* for shmget() shmctl() */
#include <sys/shm.h> /* for shmget() shmctl() */
#include <signal.h> /* for signal(2) kill(2) */
#include <errno.h> /* for perror */
#include <stdlib.h>
#include <time.h>

int main()
{
//cerating a common key wwhich can be shared b/w student.c and teacher.c
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
//Creating shared memory for both the process
  int shmid =  shmget(key , sizeof(int), IPC_CREAT | 0777);
  if (shmid == -1)
  { /* shmget() failed() */
          perror("shmget() failed: ");
          exit (1);
  }
  else
  {
    printf("shmget() returns shmid = %d.\n", shmid);
  }
  //main program accessing to the shared memory to set the variable value  = -1
  int *ppi_child;
  ppi_child = shmat(shmid, NULL, 0);// void *shmat(int shmid, const void *shmaddr, int shmflg);
  *ppi_child = -1;
  shmdt(ppi_child);//detaching the main program from the shared memory


  int roll_no;
  printf("ENTER YOUR ENROLLMENT NUMBER\n");
  scanf("%d",&roll_no);

      int *pi_child;// void *shmat(int shmid, const void *shmaddr, int shmflg);
      pi_child = shmat(shmid, NULL, 0);//child process accessing to the shared memory
      if (pi_child == (void *)-1)
      {
            /* shmat fails */
            perror("shmat() failed at child: ");
            exit (1);
      }

      if (*pi_child == -1)
      {
        *pi_child = roll_no;
         printf("VALUE OF ROLL NO = %d\n",*pi_child);
      }

      shmdt(pi_child);//detaching the child process from the shared memory

return 0;
}
