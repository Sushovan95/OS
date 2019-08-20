#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h> //pipe(),execvp(),execve(),fork()
#include<fcntl.h> //open()
#include<sys/stat.h> //open()
#include<sys/types.h> //open(),wait(),waitpid()
#include<sys/wait.h> //wait(),waitpid()

#define COLOR "\x1b[36m" //blue color used in "MYSHELL" prompt 
#define RED "\x1b[31m" //Red color
#define RESET "\x1b[0m" //reset the color to normal

#define SIZE 1024

char temp[SIZE];
char buffer[SIZE];
pid_t pid;
int stat2; //exit staus of child process which is executing execve(); stat2=0 for successful execution and 1 unsuccessful execution
int stat1 = 1;
int value = 0;

void externalcmd(char[]);
void prewd();
void changedir(char[]);
void clearscrn();

int main()
{
	printf(RED"*****************ALL COMMANDS MUST BE SPACE SEPARATED*******************\n"RESET);
	
	while(1)
	{	
		value = 0;
		printf(COLOR"MYSH>"RESET);
		fgets(buffer,SIZE,stdin);
		
		char* m = buffer;
		m[strlen(m)-1] = 0;
		
		int track = 0;
		char symbol;
		int c = 0;
		char buffer2[SIZE];
		strcpy(buffer2,buffer);
		char* bfr = buffer;
		char* token2;
		char* tok2;
		while(token2 = strtok_r(bfr," ",&bfr))
		{
			if (strcmp(token2,";") == 0 || strcmp(token2,"&&") == 0 || strcmp(token2,"||") == 0) //checking for connector
			{
				tok2 = (char*) malloc(strlen(token2)); 
				strcpy(tok2,token2); // storing the connector present in the string
				track = 1;
				break; //and breaking from the loop
			}
		}


		if (track == 0) // track == 0 implies no connector present in the input
		{
			char buffer3[SIZE];
			strcpy(buffer3,buffer2);
			char* bfr3 = buffer3;
			char* token3;
			char* tok3[5];
			int p = 0;
			int flag = 0;
			char* ttok;

			
			while(token3 = strtok_r(bfr3," ",&bfr3)) //tokenizing wrt space(" ")
			{
				if((strcmp(token3,"<") == 0 || strcmp(token3,"2>") == 0 || strcmp(token3,">") == 0 || strcmp(token3,"|") == 0)) //checking
					//whether there is any I/O redirection or pipe("|") present or not
				{
					ttok = (char*) malloc(strlen(token3));
					strcpy(ttok,token3); // storing that I/O redirc. or pipe symbol in the ttok
					flag = 1;
					break; //if we get the I/O redirec or pipe then breaking from the loop 
				}
			}
			
			if (flag == 1) // flag == 1 implies that I/O redirection or pipe is present in the input
			{
				
				if(strcmp(ttok,"2>") == 0) // for std error store in a file
				{
					char buffer4[SIZE];
					strcpy(buffer4,buffer2);
					char* bfr4 = buffer4;
					char* token4;
					char* tok6[5];
					int s = 0;

					while(token4 = strtok_r(bfr4,"2>",&bfr4)) //splitting the string wrt "2>"
					{
						tok6[s] = malloc(strlen(token4));
						strcpy(tok6[s],token4);
						s++;
					}

					char* m = tok6[0];
					m[strlen(m)-1] = 0;

					char tempo[10];
					strcpy(tempo,tok6[1]);
					memset(tok6[1],0,sizeof(tok6[1]));
					strcpy(tok6[1],tempo+1);

					int fd3;
					int pid = fork(); //creating the child process

					if (pid == 0)
					{
						fd3 = open(tok6[1],O_CREAT|O_WRONLY|O_APPEND, 0777);//creating the file in write only mode
						if (fd3 == -1)
						{
							return 0;
						}

						dup2(fd3,STDERR_FILENO);// error message will be printed in the file

						char* token1;
						int v = 0;
						char* bufls4[SIZE];
						while(token1 = strtok_r(tok6[0]," ",&tok6[0])) //splitting the file wrt " "
						{
							bufls4[v] = (char*) malloc(strlen(token1));
							strcpy(bufls4[v],token1);
							v++;
						}

						bufls4[v] = NULL;
					
						int f = execvp(bufls4[0], bufls4); //executes all internal and external commands
						if(f == -1)
						{
							fprintf(stderr,"WRONG COMMAND\n"); //if command is not executed then 
							//this message will be printed inside the file 
							exit(1);
						}
					}

					else
					{
						int statt2;
						waitpid(pid,&statt2,0); //statt2 will be storing the exit status of the child process with value 0 or 1
					}
				}

				else if (strcmp(ttok,"<") == 0) //command will get the input from the file i.e input I/O redirection
				{

					char buffer4[SIZE];
					strcpy(buffer4,buffer2);
					char* bfr4 = buffer4;
					char* token4;
					char* tok5[5];
					int r = 0;

					while(token4 = strtok_r(bfr4,"<",&bfr4)) //splitting the string into 2 halves by "<"
					{
						tok5[r] = malloc(strlen(token4));
						strcpy(tok5[r],token4);
						r++;
					}

					char* m = tok5[0];
					m[strlen(m)-1] = 0;
					
			
					char tempo[10];
					strcpy(tempo,tok5[1]);
					memset(tok5[1],0,sizeof(tok5[1]));
					strcpy(tok5[1],tempo+1);

					int fd2;
					int pid = fork(); //creating the child process

					if (pid == 0)
					{
						fd2 = open(tok5[1],O_RDONLY);//opening the file in read only mode
						if (fd2 == -1)
						{
							return 0;
						}

						dup2(fd2,STDIN_FILENO); //input of the command will be read from the file whose file
						//descriptor is fd2 instead of standard input

						char* token1;
						int v = 0;

						char* bufls3[SIZE];
						while(token1 = strtok_r(tok5[0]," ",&tok5[0]))
						{
							bufls3[v] = (char*) malloc(strlen(token1));
							strcpy(bufls3[v],token1);
							v++;
						}

						bufls3[v] = NULL;
					
						int f = execvp(bufls3[0], bufls3);
						if(f == -1)
						{
							exit(1); //for unsuccessful execution child preocess exits with value 1
						}
					}

					else
					{
						int statt1;
						waitpid(pid,&statt1,0); //parent is waiting
					}
						
				}

				else if (strcmp(ttok,">") == 0) // output of a command will be printed in a file
				{//output I/O redirection

					char buffer4[SIZE];
					strcpy(buffer4,buffer2);
					char* bfr4 = buffer4;
					char* token4;
					char* tok4[5];
					int q = 0;

					while(token4 = strtok_r(bfr4,">",&bfr4)) //splitting the string by ">"
					{
						tok4[q] = malloc(strlen(token4));
						strcpy(tok4[q],token4);
						q++;
					}

					char* m = tok4[0];
					m[strlen(m)-1] = 0;
				
			
					char tempo[10];
					strcpy(tempo,tok4[1]);
					memset(tok4[1],0,sizeof(tok4[1]));
					strcpy(tok4[1],tempo+1);


					int fd1;
					int pid = fork(); //creating child process
					if (pid == 0)
					{
						fd1 = open(tok4[1],O_CREAT|O_WRONLY|O_APPEND, 0777);//creating the file with write only mode
						if (fd1 == -1)
						{
							return 0;
						}

						dup2(fd1,STDOUT_FILENO); //output of the command will be printed in the file whose file 
						//descriptor is fd1


						char* token1;
						int v = 0;
						char* bufls2[SIZE];
						while(token1 = strtok_r(tok4[0]," ",&tok4[0]))
						{
							bufls2[v] = (char*) malloc(strlen(token1));
							strcpy(bufls2[v],token1);
							v++;
						}

						bufls2[v] = NULL;
					
						int f = execvp(bufls2[0], bufls2);
						if(f == -1)
						{
							exit(1);
						}
					}

					else
					{
						int statt;
						waitpid(pid,&statt,0);
					}
					
				}

				else if (strcmp(ttok,"|") == 0) // for pipe()
				{
					char buffer4[SIZE];
					strcpy(buffer4,buffer2);
					char* bfr4 = buffer4;
					char* token4;
					char* tok3[5];
					int p = 0;

					while(token4 = strtok_r(bfr4,"|",&bfr4))
					{
						tok3[p] = malloc(strlen(token4));
						strcpy(tok3[p],token4);
						p++;
					}

					char* m = tok3[0];
					m[strlen(m)-1] = 0; //eleminating the " " from the last position of first string

					char tempo[10];
					strcpy(tempo,tok3[1]);
					memset(tok3[1],0,sizeof(tok3[1]));
					strcpy(tok3[1],tempo+1); //removing the " " from the first position of the second string


					for (int i = 0; i < p; i++)
					{
						int pfd[2];
						pipe(pfd); //pipe establishes uni-directional communication b/w two process
						//pfd[0] for read and pfd[1] for write

						int pid;
						pid = fork(); //creating child process

						if(pid == 0)
						{
							if(i == 0)
							{
								dup2(pfd[1], STDOUT_FILENO); //output of the first command will be written
								//
							}
							else if(i == 1)
							{
								dup2(pfd[0], STDIN_FILENO);
							}

							char* token1;
							int v = 0;
							char* bufls[SIZE];
							while(token1 = strtok_r(tok3[i]," ",&tok3[i]))
							{
								bufls[v] = (char*) malloc(strlen(token1));
								strcpy(bufls[v],token1);
								v++;
							}

							bufls[v] = NULL;

							int f = execvp(bufls[0], bufls);
							if(f == -1)
							{
								exit(1);
							}

						}
						else
						{
							int status;
							waitpid(pid, &status, 0);
						}
					}
		
				}
		}

		else if(flag == 0) // if any pipe or I/O redirection is not present in the string
			{

				if(strncmp(buffer2,"exit",4) == 0) //exit from myshell
				{
					exit(0);
				}
				else if (strcmp(buffer2,"pwd") == 0) //to know the present working directory
				{
					prewd();
				}
				else if (strncmp(buffer2,"cd",2) == 0) //to change the directory
				{
					strcpy(temp,buffer2+3);
					changedir(temp);
				}
				else if (strncmp(buffer2,"clear",5) == 0) // for clear the screen
				{
					clearscrn();
				}
				else // for all external commands
				{
					strcpy(temp,buffer2);
					externalcmd(temp);
				}
		    }
		}

		else if (track == 1) //condintion for when connector is present
		{
			char* token;
			char* buff = buffer2;
			char* strbuf[SIZE];
			int i = 0;
			int j1 = 0;

			if (strcmp(tok2,";") == 0)
			{
				while(token = strtok_r(buff,";",&buff))
				{
					strbuf[i] = (char*) malloc(strlen(token));
					strcpy(strbuf[i],token);
					int lenn = strlen(strbuf[i]);
					i++;
				}

				char* m = strbuf[0];
				m[strlen(m)-1] = 0;
				int lenn = strlen(strbuf[0]);
		
				char tempo[10];
				strcpy(tempo,strbuf[1]);
				memset(strbuf[1],0,sizeof(strbuf[1]));
				strcpy(strbuf[1],tempo+1);
	
			}

			else if (strcmp(tok2,"||") == 0) //checking the connector is "||" or not
			{
							
				while(token = strtok_r(buff,"||",&buff)) //splitting the string by "||"
				{
					strbuf[i] = (char*) malloc(strlen(token));
					strcpy(strbuf[i],token);
					int lenn = strlen(strbuf[i]);	
					i++;
				}

				char* m = strbuf[0];
				m[strlen(m)-1] = 0;
				int lenn = strlen(strbuf[0]);
		
				char tempo[10];
				strcpy(tempo,strbuf[1]);
				memset(strbuf[1],0,sizeof(strbuf[1]));
				strcpy(strbuf[1],tempo+1);

			}

			else if(strcmp(tok2,"&&") == 0) // checking whether the connector is "&&"
			{
				
				while(token = strtok_r(buff,"&&",&buff)) //splitting the string w.r.t "&&" 
				{
					strbuf[i] = (char*) malloc(strlen(token));
					strcpy(strbuf[i],token);
					int lenn = strlen(strbuf[i]);
					i++;
				}

				char* m = strbuf[0];
				m[strlen(m)-1] = 0;
				int lenn = strlen(strbuf[0]);
		
				char tempo[10];
				strcpy(tempo,strbuf[1]);
				memset(strbuf[1],0,sizeof(strbuf[1]));
				strcpy(strbuf[1],tempo+1);

			}

			while(j1<i)
				{
					
					int lenn = strlen(strbuf[j1]);
					strcpy(buffer,strbuf[j1]);

					if(strncmp(buffer,"exit",4) == 0) // exit from myshell
					{
						exit(0);
					}
					else if (strcmp(buffer,"pwd") == 0) // to know the present working directory
					{
						prewd();
					}
					else if (strncmp(buffer,"cd",2) == 0) // to change the directory
					{
						strcpy(temp,buffer+3);
						changedir(temp);
					}
					else if (strncmp(buffer,"clear",5) == 0) //clear the screen
					{
						clearscrn();
					}
					else // for all external commands
					{
						strcpy(temp,buffer);
						externalcmd(temp);
					}

					memset(buffer,0,sizeof(buffer));
					memset(temp,0,sizeof(temp));

					if(strcmp(tok2,";") == 0)
					{
						j1++;
					}
					
					else if(strcmp(tok2,"||") == 0)
					{
						//printf("%d\n",WEXITSTATUS(stat));
						if (WEXITSTATUS(stat2) == 0 || value == 1) // if first command(internal or external)
							//executed successfully then break from the while loop as connector is "||"
						{
							break;
						}
						else
							j1++;
						
					}
					
					else if(strcmp(tok2,"&&") == 0) 
					{
						if (WEXITSTATUS(stat2) == 0 || value == 1)//if first command(internal or external)
							//executed successfully then only it will be incremented and allow the next command for
						//execution
						{
							j1++;
							value = 0; //making the vlaue = 0 as it keeps track of execution of internal command

						}
						else
							break;
								
					}
					else
						break;
			}	
		
		}		
	
	}
	
	return 0;
}

void clearscrn() //clear the screen internal command
{
	system("clear");
	value = 1; //true;
}

void changedir(char buff[])//internal command //funtion to change the directory (internal command) i.e cd /path
{
	int len = strlen(buff);
	//printf("%s of length = %d\n",buff,len);
	if (len == 0)
	{
		char home[50];
		strcpy(home,"/home/sushovan");
		int stat = chdir(home);
		if (stat == 0)
		{
			printf("SUCCESS\n");
			value = 1;
		}
		else if (stat == -1)
		{
			printf("CANNOT CHANGE DIRECTORY\n");
			value = 0;
		}
	}
	else if (strncmp(buff,"..",2) == 0) // perfoming cd .. operaton same as actual shell
	{
		char direc1[SIZE];
		char catstr[SIZE];
		getcwd(direc1,sizeof(direc1));
		char* tok[SIZE];
		int i = 0; 
		char* pw = direc1;
		char* token;
		while(token = strtok_r(pw,"/",&pw))
		{
			tok[i] = (char*) malloc(strlen(token));
			strcpy(tok[i],token);
			i++;
		}

		memset(catstr,0,sizeof(catstr));

		for (int j = 0; j < i-1; j++)
		{
			strcat(catstr,"/");
			strcat(catstr,tok[j]);
		}

		//catstr[len] = '/';
		len = strlen(catstr);
		//printf("%s of length = %d\n",catstr,len);
		int stat = chdir(catstr);
		memset(catstr,0,sizeof(catstr));

		if (stat == 0)
		{
			printf("SUCCESS\n");
			value = 1;
		}
		else if (stat == -1)
		{
			printf("CANNOT CHANGE DIRECTORY\n");
			value = 0;
		}
		
	}

	else
	{
		int stat = chdir(buff);
		if (stat == 0)
		{
				printf("SUCCESS\n");
				value = 1;
		}
		else if (stat == -1)
		{
				printf("CANNOT CHANGE DIRECTORY\n");
				value = 0;
		}
	}
	
}

void prewd() //function to know present working directory(internal command)
{
	char direc[1024];
	getcwd(direc,sizeof(direc));
	value = 1;
	printf("%s\n",direc);
}

void externalcmd(char buf[]) //for all external commands like ls,ps with switches
{
	char* token1;
	int i = 0;
	int len = strlen(buf);
	if (len == 0)
	{
		return;
	}
	char* bufls[SIZE];
	while(token1 = strtok_r(buf," ",&buf))
	{
		bufls[i] = (char*) malloc(strlen(token1));
		strcpy(bufls[i],token1);
		i++;
	}

	//printf("EXTERNAL COMMAND\n");
	char myar[SIZE];
	strcpy(myar,"/bin/");
	strcat(myar,bufls[0]);

	char* str[SIZE];
	str[0] = myar;

	for (int j = 1; j < i; j++)
	{
		str[j] = bufls[j];
	}

	str[i] = NULL;

	pid = fork();
	if (pid == 0)
	{
		
		int status = execve(myar,str,NULL);
		if (status == -1)
		{
			char myar1[SIZE];
			strcpy(myar1,"/usr/bin/");
			strcat(myar1,bufls[0]);

			char* str1[SIZE];
			str1[0] = myar1;

			for (int k = 1; k < i; k++)
			{
				str1[k] = bufls[k];
			}

			str1[i] = NULL;
			status = execve(myar1,str1,NULL);
			if (status == -1)
			{
				exit(1);
			}		
		}	
	}
	else
	{
		waitpid(pid,&stat2,0);
	}	
}

