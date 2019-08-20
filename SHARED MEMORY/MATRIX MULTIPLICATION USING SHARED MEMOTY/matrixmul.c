#include<stdio.h>
#include<stdlib.h>
#include <errno.h> /* for perror */
#include<unistd.h> // for fork()
#include<sys/wait.h> // for wait()
#include<sys/types.h> // for wait() kill(2)
#include <sys/ipc.h> /* for shmget() shmctl() */
#include <sys/shm.h> /* for shmget() shmctl() */


int main()
{
  int r1,c1,r2,c2,r3,c3;
  int i1,j1;
  int pid;

  printf("ENTER THE NO. OF ROW OF FIRST MATRIX\n");
  scanf("%d",&r1);

  printf("ENTER THE NO. OF COLUMN OF FIRST MATRIX\n");
  scanf("%d",&c1);

  int arr1[r1][c1];
  printf("ENTER THE VALUES IN THE MATRIX:\n");

  for(i1=1;i1<=r1;i1++)
  {
    for(j1=1;j1<=c1;j1++)
    {
       scanf("%d",&arr1[i1][j1]);
    }
  }

  i1 = 1;
  j1 = 1;

  printf("FIRST MATRIX:\n");

  for(i1=1;i1<=r1;i1++)
  {
    for(j1=1;j1<=c1;j1++)
    {
       printf("%d\t",arr1[i1][j1]);
    }
    printf("\n");
  }

  i1 = 1;
  j1 = 1;
  r2 = c1;

  printf("ENTER THE NO. OF COLUMNS OF SECOND MATRIX\n");
  scanf("%d",&c2);

  int arr2[r2][c2];
  printf("ENTER THE VALUES IN THE MATRIX:\n");

  for(i1=1;i1<=r2;i1++)
  {
    for(j1=1;j1<=c2;j1++)
    {
       scanf("%d",&arr2[i1][j1]);
    }
  }

  i1 = 1;
  j1 = 1;

  printf("SECOND MATRIX:\n");

  for(i1=1;i1<=r2;i1++)
  {
    for(j1=1;j1<=c2;j1++)
    {
       printf("%d\t",arr2[i1][j1]);
    }
    printf("\n");
  }

  i1 = 1;
  j1 = 1;

  r3 = r1;
  c3 = c2;

  int shmid;
  printf("%ld\n",sizeof(int)*r3*c3);
  shmid =  shmget(IPC_PRIVATE, sizeof(int)*r3*c3, IPC_CREAT | 0777);
  if (shmid == -1)
  { /* shmget() failed() */
          perror("shmget() failed: ");
          exit (1);
  }
  else
  {
    printf("shmget() returns shmid = %d.\n", shmid);
  }

  int sum = 0;
  int arr3[r3][c3];
  int k = 1;

  for(i1=1;i1<=r3;i1++)
  {
    for(j1=1;j1<=c3;j1++)
    {

      pid = fork(); // creating child process
      if(pid == 0)
      {
        int *pi_child;
        // void *shmat(int shmid, const void *shmaddr, int shmflg);
        pi_child = shmat(shmid, NULL, 0);
        if (pi_child == (void *)-1)
        { /* shmat fails */
                perror("shmat() failed at child: ");
                exit (1);
        }

      printf("CHILD PROCESS ID=%d\n",getpid());
      for(k=1;k<=c1;k++)
      {
        sum = sum + arr1[i1][k]*arr2[k][j1];
      }

      //arr3[i1-1][j1-1] = sum;
      printf("CALCULATED VALUE:%d\n",sum);
      *(pi_child+(i1-1)*c3+(j1-1)) = sum;
      sum = 0;
      shmdt(pi_child);

      exit(0);
    }
    else
    {
      wait(NULL);
    }

  }
}

  /*main process*/
  i1=0;
  j1=0;
  int *ppi_child;
  ppi_child = shmat(shmid,NULL,0);
  if (ppi_child == (void *)-1)
  { /* shmat fails */
          perror("shmat() failed at child: ");
          exit (1);
  }

  printf("RESULTANT MATRIX:\n");

  for(i1=0;i1<r3;i1++)
  {
    for(j1=0;j1<c3;j1++)
    {
      printf("%d\t",*(ppi_child+i1*c3+j1));
    }
    printf("\n");
  }

  shmdt(ppi_child);
//  shmctl(shmid,IPC_RMID,NULL);
  i1=0;
  j1=0;
  int status;
  // int shmctl(int shmid, int cmd, struct shmid_ds *buf);
  status = shmctl(shmid, IPC_RMID, NULL); /* IPC_RMID is the command for destroying the shared memory*/
  if (status == 0)
  {
          fprintf(stderr, "Shared memory has been removed with id = %d.\n", shmid);
  } else if (status == -1)
  {
          fprintf(stderr, "Cannot remove shared memory with id = %d.\n", shmid);
  } else
  {
          fprintf(stderr, "shmctl() returned wrong value while removing shared memory with id = %d.\n", shmid);
  }
  return 0;

}
