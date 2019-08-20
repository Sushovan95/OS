/*Using this execute.c program we will call the functions which are to be used to operate on 
linkedlists.Here the header sharedll.h contains the definition of those functions 
*/
#include"sharedll.h" //header file that contains all the functions for linkedlist
#define NO_SEM 1

int main()
{
	char yn;
	printf("DO YOU WANT TO CREATE LINKEDLISTS OR OPERATE ON EXISTINGS..IF NOT INITIALIZED THEN CREATE FIRST = Y / N\n");
	scanf(" %c",&yn);

	if (yn == 'y') //to initiallize the no of linkedlist user want to create
	{

		union semun 
		{
			int              val;    /* Value for SETVAL */
			struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
			unsigned short  *array;  /* Array for GETALL, SETALL */
			struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
		} setvalArg;

		setvalArg.val = 1;


		int n1;
		int t;
		printf("HOW MANY LINKED-LIST YOU WANT TO CREATE\n");
		scanf("%d",&n1);
		
		key_t arr[n1];
		int semid[n1];

		for (t = 0; t < n1; t++)
		{
			arr[t] = ftok("/bin/ls",t);  //generating keys for each semaphore
			
		}

		key_t my_key = ftok("/bin/ls",100);//key to generate shared memory where array of linkedlist
		//headers will be stored

		if (my_key == -1) 
		{
			perror("ftok() failed");
			exit(1);
		}


		for ( t = 0; t < n1; t++)
		{
			semid[t] = semget(arr[t], NO_SEM, IPC_CREAT | 0777);//generating semaphore corresponding to each linkedlist  
			semctl(semid[t], 0, SETVAL, setvalArg);	//and initially set the semaphore value to 1 as setvalArg.val = 1
		}

		
		int shm_id =  shmget(my_key,(n1+1)*sizeof(int), IPC_CREAT | 0777);//creating shared memory to store the array of headers
		int* p_tr;
		p_tr = shmat(shm_id,NULL,0);

		*p_tr = n1;//no of linked lists stored here

		for(t=1;t<=n1;t++)//storing the headers of linkedlists and initially all are -1
		{
			*(p_tr+t) = -1;

		}
		

		shmdt(p_tr);
		
	}

	else
	{	
		int i;

		key_t mykey = ftok("/bin/ls",100);//generating the same key so that we can get access to the same memory 
		//location where array of headers are stored
		if (mykey == -1) 
		{
			perror("ftok() failed");
			exit(1);
		}
		
		int shmid = shmget(mykey,sizeof(int), IPC_CREAT | 0777);
		int* ptr;
		ptr = shmat(shmid,NULL,0);

		int n = *ptr;
		key_t arr[n];
		int headar[n];
		


		for (i = 0; i < n; i++)
		{
			arr[i] = ftok("/bin/ls",i); //generating same keys which are used for initializing the semaphores
		}
		//choices to work on the linkedlists
		printf("------------------------------------------------------------------\n");
		printf("1.searchnode(head,value)\n");
		printf("2.insertfirst(head,value)\n");
		printf("3.insertlast(head,value)\n");
		printf("4.insertafter(head,data,value)\n");
		printf("5.deletenode(head,value)\n");
		printf("6.display(head)\n");
		printf("7.EXIT\n");
		printf("8.FOR DESTROYING LINKEDLISTS AND IPC-TOOLS\n");
		printf("-------------------------------------------------------------------\n");

		int x;
		printf("ENTER YOUR CHOICE:\n");
		scanf("%d",&x);
		switch(x)
		{
			case 1: printf("CHOOSE THE LINKEDLIST NO.\n");
					int llno1;
					scanf("%d",&llno1);
					printf("ENTER THE DATA\n");
					int data1;
					scanf("%d",&data1);
					int stat = searchnode((ptr+llno1),data1);
					if (stat == 0)
					{
						printf("THE NODE DOES NOT EXIST\n");
					}
					else
					{
						printf("THE NODE EXISTS\n");
					}
					break;

			case 2: printf("CHOOSE THE LINKEDLIST NO.\n");
					int llno2;
					scanf("%d",&llno2);
					printf("ENTER THE DATA\n");
					int data2;
					scanf("%d",&data2);
					insertfirst((ptr+llno2),data2,arr[llno2]);
					break;

			case 3: printf("CHOOSE THE LINKEDLIST NO.\n");
					int llno3;
					scanf("%d",&llno3);
					printf("ENTER THE DATA\n");
					int data3;
					scanf("%d",&data3);
					insertlast((ptr+llno3),data3,arr[llno3]);
					break;

			case 4: printf("CHOOSE THE LINKEDLIST NO.\n");
					int llno4,value;
					scanf("%d",&llno4);
					printf("ENTER THE VALUE AFTER WHICH YOU WANT TO INSERT:\n");
					scanf("%d",&value);
					printf("ENTER THE DATA YOU WANT TO STORE\n");
					int data4;
					scanf("%d",&data4);
					insertafter((ptr+llno4),value,data4,arr[llno4]);				
					break;

			case 5: printf("CHOOSE THE LINKEDLIST NO.\n");
					int llno5;
					scanf("%d",&llno5);
					printf("ENTER THE DATA\n");
					int data5;
					scanf("%d",&data5);
					deletenode((ptr+llno5),data5,arr[llno5]);
					break;

			case 6: printf("CHOOSE THE LINKEDLIST NO.\n");
					int llno6;
					scanf("%d",&llno6);
					display((ptr+llno6),arr[llno6]);				
					break;

			case 7:
					exit(0);

			case 8:
					printf("DO YOU WANT TO DELETE ANY LINKEDLIST = Y / N \n");
					char input;
					char choice;
					scanf(" %c",&choice);

					if (choice == 'y')
					{
						printf("ARRAY OF HEADERS BEFORE REMOVING \n");
						for (i = 0; i < n; i++)
						{
							headar[i] = *(ptr+i+1);
							printf("%d\n",headar[i]);
						}

						printf("ENTER THE LINKEDLIST NO. YOU WANT TO DELETE :\n");
						int lno;
						scanf("%d",&lno);

						int id1 = *(ptr+lno);
						int temp1 = id1;
						node* newptr1;
						newptr1 = (node*)shmat(id1,NULL,0);
						while(temp1 != -1)
						{
							newptr1 = (node*)shmat(temp1,NULL,0);
							temp1 = newptr1->next;

							shmctl(temp1,IPC_RMID, NULL);
						}

						shmctl(id1,IPC_RMID, NULL);
						*(ptr+lno) = -1;

						printf("ARRAY OF HEADERS AFTER REMOVING \n");
						for (i = 0; i < n; i++)
						{
							headar[i] = *(ptr+i+1);
							printf("%d\n",headar[i]);
						}

					}

					else if(choice == 'n')
					{

						for (i = 0; i < n; i++)
						{
							headar[i] = *(ptr+i+1);
							printf("%d\n",headar[i]);
						}
	
						printf("DO YOU WANT TO DESTROY ALL THE IPC IPC-TOOLS : (Y / N) \n");
						scanf(" %c",&input);
						
						if (input == 'y')//for removing each linked list;
						{
							for (i = 0; i < n; i++)
							{
							  if (arr[i] != -1)
							  {
								int id = headar[i];
								int temp = id;
								node* newptr;
								newptr = (node*)shmat(id,NULL,0);
								while(temp != -1)
								{
									newptr = (node*)shmat(temp,NULL,0);
									temp = newptr->next;

									shmctl(temp,IPC_RMID, NULL);
								}

								shmctl(id,IPC_RMID, NULL);
							  }
							
							}

							shmctl(shmid,IPC_RMID, NULL); //for removing the array of headers

							for (i = 0; i < n; i++)// for removing semaphores
							{
								int semmid = semget(arr[i],NO_SEM,IPC_CREAT | 0777);
								semctl(semmid,0,IPC_RMID, NULL);

							}
						
						}

						else if ( input == 'n')
						{
							exit(0);
						}

					}

					break;

			default:
					printf("ENTER PROPER CHOICE...!\n");
					break;

		}

	}

	return 0;

}