#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int main()
{
  FILE *fp;
  char buff[1024];

  fp = fopen("Write.txt","w");//file is opened by the process P1(i.e PARENT PROCESS)
  int pid;
  printf("WRITE SOMETHING IN THE FILE\n");
  fgets(buff,1024,stdin);

  pid = fork();//creating child process(i.e P2)

  if(pid==0)
  {
    //Line is being written by the child
    fputs("LINE WRITTEN BY THE CHILD \n",fp);
    fputs(buff,fp);
    //fclose(fp);
    exit(0);
   
  }
  else
  {
    //wait(NULL);
    //Line is being written by the Parent
    fputs("LINE WRITTEN BY THE PARENT\n",fp);
    fputs(buff,fp);
    fclose(fp);//closing the file
  }
  //fclose(fp);//closing the file

  return 0;
}
