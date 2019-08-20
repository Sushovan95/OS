#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/types.h>

int main()
	{
        int i,pid;
		int proc;
		printf("ENTER THE NUMBER OF PROCESSES\n");
		scanf("%d",&proc);
    for (i=0;i<proc;i++)
    {
        pid = fork();//creating child process
        if (pid<0)
          {
            return -1;
          }
        else if (pid == 0)
            {
              int id = getpid();//returns the process ID
              printf("CHILD PROCESS:%d ID = %d\n",(i+1),id);
              exit(1);
            }
        else
            {
			  int p = getpid();
			  printf("PARENT PROCESS ID = %d\n",p);
              wait(NULL);
            }
    }

    		return 0;

	}
