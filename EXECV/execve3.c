#include<stdio.h>
#include<stdlib.h> //for exit(0)
#include<unistd.h> // for fork()
#include <sys/wait.h> // for waitpid()

int main(int argc,char* argv[])
{
  int i,j,pid;
  char* envp[] = {NULL};//making the third argument that execve takes.as it is defined the second and third
  //argument must be a array of strings and must be ended with NULL.

  for(i=1;i<argc;i++)
  {
      pid = fork();
      char* myarr[] = {argv[i],NULL};
      if(pid == 0)
      {
        int status1;
        printf("CHILD PROCESS ID = %d\n",getpid());
        status1 = execve(argv[i],myarr,envp);
        exit(0);
      }

      else  // Main (parent) process
      {

        int status;
        //pid_t waitpid(pid_t pid, int *wstatus, int options);
        int stat = waitpid(pid,&status, 0);  // Parent process will wait for child to terminate.

        if (WIFEXITED(status))  // Verify child process terminated without error.
        { //Returns True if status was returned for a child that terminated normally. In this case, we can
         //execute WEXITSTATUS(status) to fetch the value that the child passed to exit
          printf("THE CHILD PROCESS THAT TERMINATED NORMALLY.EXIT STATUS = %d\n",WEXITSTATUS(status));
          printf("PROCESS THAT TERMINATED WITH ID:%d\n",stat);
        }

        if (WIFSIGNALED(status))
        {//if we want child to be terminated abnormally then in place of execve() we can try
         //didvide by zero.then this part will be executed.
         //when child tries to divide a number by 0 , it will be killed by signal no 8.
        //this signal no. will be shown by the WTERMSIG(ststus).
          printf("THE CHILD PROCESS THAT TERMINATED ABNORMALLY.EXIT STATUS =%d\n",WTERMSIG(status));
        }
      }

  }

  return 0;
}
