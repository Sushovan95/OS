#include<stdio.h>
#include<stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include<sys/types.h>

int factor(int );
int main(int argc,char *argv[])
	{
    int i,pid;

    for (i=0;i<argc;i++)
    {
        pid = fork(); // creating child process
        if (pid<0)
          {
            return -1;
          }
        else if (pid == 0)
            {
              int id = getpid();//returns the process ID
			  int num = atoi(argv[i+1]);//atoi function converts in integer
			  int fact = factor(num);
              printf("CHILD PROCESS:%d ID = %d ",(i+1),id);
			  printf("FACTORIAL OF %d = %d\n",num,fact);
              exit(1);
            }
        else
            {
            	//parent process
              wait(NULL);//This function blocks the calling process until one of its child processes exits
            }
    }
   
   /* for (i=0;i<argc;i++)
    {
    	wait(NULL);
    }*/



    		return 0;

	}

int factor(int p)
	{
		int sum = 1;
		int k;
		if(p==0)
			{
				return 1;
			}
		else if(p<0)
			{
				return -1;
			}
		else
		 {
			for(k=1;k<=p;k++)
			{
				sum = sum * k;
			}
			return sum;
		}
	}
