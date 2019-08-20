#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
  FILE *fp;
  char buff[100];

  fp = fopen("Read.txt","r");//file is opened by the process P1(i.e PARENT PROCESS)
  int pid;
  pid = fork();//creating child process(i.e P2)

  //fgets(buff,1024,(FILE*)fp);//reading lines into buff


  if(pid==0)
  {
    
    //Line is being read by the child
    printf("LINE READ BY THE CHILD:\n");
    fgets(buff,100,(FILE*)fp);//reading lines into buff
    printf("%s\n",buff);
    fclose(fp);
    printf("CHILD read\n");
    exit(0);

  }

  else
  {
    
    wait(NULL);
    //Line is being read by the Parent
    printf("LINE READ BY THE PARENT:\n");
    fgets(buff,100,(FILE*)fp);//reading lines into buff
    printf("%s\n",buff);
    //fclose(fp);//closing the file
    
  }
  //fclose(fp);//closing the file

  return 0;
}
