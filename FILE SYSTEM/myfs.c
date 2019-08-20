/*
This program is to create my own file system.There are mkfs,use,ls,cp,rm,mv commands which are being used for creating filsystem,mount a file
in a specified path,showing the contents of the file in the mounted location,copy one file to mounted file or another file ,remove the file 
from the system or from the path, and renaming a file repectively.
in my file system superblock is given more priority as it tells the current status of the file to the user. i.e. if user wants to copy some 
file to another file then user first know that the file he/she wants to copying,whether the file size can be accomodated inside that file.
Not only this user also must know whether that much data blocks are free or not.Now if user has to check all such things it will be 
complicated for users.Therefore to make it simpler I designed the superblock in a such way from that users do not have to bother all such 
things.
initially we copied a file(os1) inside an another file (say os2).now when another user will try to copy another file(say os3) in os2 then 
program first checks the superblock values of the os2 which is updated after copying the os1 file.now next inode index ,free datablocks,
next inode start location, next datablock start location are updated accordingly.
these elements of the superblock indicate while copying from which location the datablocks can be occupied and for that copied file where the
inode will be located that is also given.now we are maintaining a array of inodelocations.By travesing this array we can find each inode location
each of the array element denotes address the of the each inode where we read the inode structure and will come to know the file that has been
copied in that file.
Here we make an assumption that a inode is dedicated for a particular file that is copied in that file(os2) and a file is 
copied in contiguous manner.
And I have kept a inode separately just after the superblock which is called as root inode that contains the info about the owner file here (os2)
now if we remove a file which had been copied to the os2 file and after that we are again try to copy a file inside os2.Now to copy that file by
reading the status of the superblock may not be enough as because the deleted file might had been located in the middle. so while deleting that 
file I am not deleting the inode for that file instead of that I am updating that inode status as empty and keeping the size that means if user
 wants then user can copy a file on that empty location and after copying that empty inode will be modified according to the copied file.
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>//open()
#include<unistd.h> //ftruncate()
#include<sys/types.h>//ftruncate()

#define RED "\x1b[31m"
#define RESET "\x1b[0m"


#define INODE_COUNT 5
#define DBLOCK_COUNT 5

#define MAX 1024

mode_t mode = S_IRUSR | S_IWUSR; //third parameter of the open() sys call.S_IRUSR and S_IWUSR for use has read and write permission respectively
char buff[MAX];
char tempbuff[MAX];
int mntindex = 0;
int icnt = 0;

struct superblock //structure of the superblock
{
	unsigned long int filesize;
	unsigned int block_size; //size of each block
	unsigned int next_inode_start_index;
	unsigned int next_inode_start_loc;
	unsigned long int data_block_start_loc;
	unsigned int free_data_block_count;
	unsigned int inode_startloc[10];

};

struct iNode //structure of each inode
{
	char type;//d for directory and f for file type
	char name[10];
	unsigned long int size;//size of file
	unsigned long int data_block_start_loc;
	unsigned int data_block_count;//no of data blocks pointed by the inode

};

int iNodestloc = sizeof(struct superblock)+sizeof(struct iNode);//first inode start location

struct mount //mount structure 
{
	char mntpart[10];
	char osfilename[10];
	unsigned long int osfsize;

};

struct mount mnt[MAX]; // array of mount structure

void mkfs(char buff1[]) //function for creating file
{

	struct superblock sb;
	int i = 0;
	int j;
	char* bfmkfs1 = buff1;
	char* bfmkfs[MAX];
	char* token;


	while((token = strtok_r(bfmkfs1," ",&bfmkfs1)))
	{
		bfmkfs[i] = token;
		i++;

	}


	if (i!=3)
	{
		printf("GIVE INSTRUCTION PROPERLY\n");
		return;
	}


	char* filename = bfmkfs[0];
	int blocksize = atoi(bfmkfs[1]);
	int totalsize = atoi(bfmkfs[2]);


	int sizem = 1024*1024*totalsize;
	int fd = open(filename,O_CREAT | O_RDWR,mode);


	//intial superblock contents
		sb.filesize = sizem;
		sb.block_size = blocksize;
		sb.next_inode_start_index = 0; //index of the next inode which we get reading the superblock
		sb.next_inode_start_loc = iNodestloc; //next inode will written in that location
		sb.data_block_start_loc = blocksize + INODE_COUNT*blocksize; //next datablock start location
		sb.free_data_block_count = (sb.filesize - sb.data_block_start_loc)/sb.block_size; //initial free data block

		for (j = 0; j < MAX; j++)
		{
			sb.inode_startloc[j] = 0; //inode start location array
		}

		int sz = write(fd,&sb,sizeof(struct superblock));
		if (sz != -1)
		{
			printf("FILE HAS BEEN UPDATED\n");
		}

		struct iNode root;
		//root inode
		root.type ='r'; //
		strcpy(root.name,filename);
		root.size = sizem;
		root.data_block_count = DBLOCK_COUNT;

		write(fd,&root,sizeof(root));

		int status = ftruncate(fd,sizem); //creating file of given size

		if (status == 0)
		{
			printf("FILE IS CREATED SUCCESFULLY\n");
		}
		else if (status == -1)
		{
			printf("FILE CANNOT BE CREATED\n");
		}

		close(fd);

}

void use(char buff2[]) //function to mount a osfile in a path
{
	char* token;
	char* bfuse[MAX];
	int i = 0;

	while(token = strtok_r(buff2," ",&buff2))
	{
		bfuse[i] = token;
		i++;
	}

	if (i!= 3)
	{
		printf("GIVE INSTRUCTION PROPERLY\n");
		return;
	}

	char* filename = bfuse[0]; //os file name which will be mounted
	char* mountpath = bfuse[2]; //at which path file will be mounted

	if (mountpath[1]!=':')
	{
		printf("GIVE MOUNT PATH NAME PROPERLY\n");
		return;
	}

	int fd = open(filename,O_RDONLY); //opening the osfile 

	if (fd == -1)
	{
		printf("%s FILE DOES NOT EXIST...CAN'T BE MOUNTED ON SPECIFIED PATH\n",filename);
		return;
	}

	struct superblock sb;
	read(fd,&sb,sizeof(struct superblock));// reading the superblock structure from the file


	//filling the stucture array mount 
	strncpy(mnt[mntindex].mntpart,mountpath,2); 
	strcpy(mnt[mntindex].osfilename,filename);
	mnt[mntindex].osfsize = sb.filesize;
	mntindex++;
	close(fd);

}

void ls(char buff3[])
{
	char* token;
	int i = 0;
	int j = 0;
	int flag = 0;

	if(buff3[1]!=':')
	{
		printf("GIVE INSTRUCTION PROPERLY\n");
		return;
	}

	for ( j = 0; j < mntindex; j++)
	{

		if (strncmp(mnt[j].mntpart,buff3,2) == 0)
		{
			flag++;
			i = j;
			break;
		}
	}

	if (flag == 0)
	{
		printf("MOUNT PATH DOES NOT EXIST\n");
		return;
	}


	void* fp; //void pointer
	fp = malloc(mnt[i].osfsize); //allocating of size,the file which will be mounted
	int fd = open(mnt[i].osfilename,O_RDONLY);//open the file from the mount stucture

	if (fd == -1)
	{
		printf("FILE DOES NOT EXIST\n");
		return;
	}

	read(fd,fp,mnt[i].osfsize); //reading the file in the address where fp pointing
	struct superblock sb;
	memcpy(&sb,fp,sizeof(sb));
	printf("******************MOUNT INFO**************************\n");

	printf("mount part = %s\n",mnt[i].mntpart);
	printf("mount osfilename = %s\n",mnt[i].osfilename);
	printf("mount osfile size = %ld\n",mnt[i].osfsize);

	printf("**********************SUPERBLOCK CONTENTS**************\n");

	printf("blocksize = %d\n",sb.block_size);
	printf("filesize = %ld\n",sb.filesize);
	printf("next_inode_start_index = %d\n",sb.next_inode_start_index);
	printf("next_inode_start_loc = %d\n",sb.next_inode_start_loc);
	printf("datablock start loc = %ld\n",sb.data_block_start_loc);
	printf("free datablocks =  %d\n",sb.free_data_block_count);

	struct iNode nd;

	memcpy(&nd,fp+sizeof(struct superblock),sizeof(struct iNode));

	printf("********************************ROOT INODE CONTENTS********************** \n");

	printf("inode type = %c\n",nd.type);
	printf("osfilename = %s\n",nd.name);
	printf("osfile size(bytes) = %ld\n",nd.size);
	printf("no of datablocks = %d\n",nd.data_block_count);

	for ( j = 0; j < sb.next_inode_start_index; j++)
		{
			if (sb.inode_startloc[j]!=0)
			{

				printf("*******************INODE = %d*************************\n",j+1);
				printf("inode start location = %d\n",sb.inode_startloc[j]);
				memcpy(&nd,fp+sb.inode_startloc[j],sizeof(nd));
				printf("inode type = %c\n",nd.type);
				printf("osfilename = %s\n",nd.name);
				printf("osfile size(bytes) = %ld\n",nd.size);
				printf("datablock start loc = %ld\n",nd.data_block_start_loc);
				printf("no of datablocks = %d\n",nd.data_block_count);

			}

		}


	close(fd);

}

void copy(char buff4[])  //performing the copy operation
{
	char* token;
	char* bfcopy[MAX];
	int i = 0;
	int j,k;
	int frmindex,toindex;
	int flag1 = 0;
	int flag2 = 0;

	char buffrm[10];
	char bufto[10];

	char srcfilename[10];
	char destfilename[10];
	char temp[10];

	while(token = strtok_r(buff4," ",&buff4))
	{
		bfcopy[i] = token;
		printf("bfcopy = %s\n",bfcopy[i]);
		i++;
	}

	if (i!= 2)
	{
		printf("GIVE INSTRUCTION PROPERLY\n");
		return;
	}

	char* from = bfcopy[0];
	char* to = bfcopy[1];

   //****************************************************************************************************************************

   if (from[1] != ':' && to[1] == ':')  //cp os1 C:testfile1
	{
		printf("COPY FROM OS FILE TO MOUNTED PATH\n");

		memset(buffrm,0,sizeof(buffrm));
		memset(bufto,0,sizeof(bufto));
		memset(srcfilename,0,sizeof(srcfilename));
		memset(destfilename,0,sizeof(destfilename));

		strcpy(buffrm,bfcopy[0]);//os file name
		strncpy(bufto,bfcopy[1],2);//mount path name
		strcpy(destfilename,bfcopy[1]+2);//os file name where data will be copied

		int len3 = strlen(destfilename);

		len3 = strlen(bufto);
		printf("bufto = %s with length = %d\n",bufto,len3);

		char* m = destfilename;
		m[strlen(m)-1] = 0;

		len3 = strlen(destfilename);
		printf("destfilename = %s with length = %d\n",destfilename,len3);

		for ( j = 0; j < mntindex; j++)
		{
			if (strncmp(mnt[j].mntpart,bufto,2) == 0)
			{
				flag2++;
				toindex = j;
				break;
			}
		}

		if (flag2 == 0)
		{
			printf("%s PATH DOES NOT EXIST\n",bufto);
			return;
		}

		struct superblock sb1,sb2,sb3;
		struct iNode nd,nd1;
		int fd1 = open(buffrm,O_RDONLY);
		read(fd1,&sb1,sizeof(sb1));
		printf("filesize = %ld\n",sb1.filesize);

		if (mnt[toindex].osfsize > sb1.filesize)
		{
			int fd = open(destfilename,O_RDWR);
			if (fd == -1)
			{

				fd1 = open(buffrm,O_RDONLY);
				read(fd1,&sb1,sizeof(sb1));
				printf("filesize = %ld\n",sb1.filesize);

				void* fp1;
				long int filesize = sb1.filesize;
				fp1 = malloc(sb1.filesize);

				read(fd1,fp1,sb1.filesize);
				memcpy(&sb3,fp1,sizeof(sb1));

				void* fp2;
				fp2 = malloc(mnt[toindex].osfsize);
				int fd2 = open(mnt[toindex].osfilename,O_RDONLY);
				read(fd2,fp2,mnt[toindex].osfsize);
				memcpy(&sb2,fp2,sizeof(sb2));

				int m = sb2.block_size;
				long int n = sb1.filesize;

				printf("n = %ld\n",n);
				printf("m = %d\n",m);

				int r = (n/m);
				printf("required no of blocks = %d\n",r);

				int a = (sb2.filesize - sb2.data_block_start_loc)/sb2.block_size;

				printf("available no of blocks = %d\n",a);

				if (a>r)
				{

					memcpy(fp2+sb2.data_block_start_loc,fp1,filesize);
					memcpy(&sb3,fp2+sb2.data_block_start_loc,sizeof(sb3));

					
					memcpy(&nd,fp2+sb2.data_block_start_loc+sizeof(struct superblock),sizeof(nd));
					memcpy(&sb3,fp2,sizeof(sb3));
					int siz = sb3.next_inode_start_loc;
					sb3.inode_startloc[sb3.next_inode_start_index] = sb3.next_inode_start_loc;
					sb3.next_inode_start_loc = sb3.next_inode_start_loc+sizeof(struct iNode);
					sb3.next_inode_start_index = sb3.next_inode_start_index+1;
					sb3.data_block_start_loc = r*sb3.block_size + sb3.data_block_start_loc;
					sb3.free_data_block_count = (sb3.filesize - sb3.data_block_start_loc)/sb3.block_size;

					nd.type = 'f';
					strcpy(nd.name,buffrm);
					nd.size = filesize;
					nd.data_block_start_loc = sb2.data_block_start_loc;
					nd.data_block_count = r;

					memcpy(&nd1,fp2+sizeof(struct superblock),sizeof(nd1));
					strcpy(nd1.name,destfilename);

					memcpy(fp2+sizeof(struct superblock),&nd1,sizeof(nd1));
					memcpy(fp2,&sb3,sizeof(sb3));
					memcpy(fp2+siz,&nd,sizeof(nd));

					close(fd1);
					close(fd2);

					int fd3 = open(destfilename,O_CREAT | O_WRONLY,mode);
					if (fd3 == -1)
					{
						printf("CANNOT OPEN FILE\n");
						return;
					}
					write(fd3,fp2,mnt[toindex].osfsize);
					ftruncate(fd3,mnt[toindex].osfsize);

					read(fd3,fp2,mnt[toindex].osfsize);

					memcpy(&sb2,fp2,sizeof(sb1));

					strcpy(mnt[toindex].osfilename,destfilename);

				}

				else
				{
					printf("CANNOT COPY....FILE SIZE EXCEED\n");
					return;
				}

		}

		else
		{

				void* fp1;
				long int filesize = sb1.filesize;
				fp1 = malloc(sb1.filesize);

				read(fd1,fp1,sb1.filesize);
				memcpy(&sb3,fp1,sizeof(sb1));

				void* fp2;
				fp2 = malloc(mnt[toindex].osfsize);
				read(fd,fp2,mnt[toindex].osfsize);
				memcpy(&sb2,fp2,sizeof(sb2));

				int m = sb2.block_size;
				long int n = sb1.filesize;

				printf("n = %ld\n",n);
				printf("m = %d\n",m);

				int r = (n/m);
				printf("required no of blocks = %d\n",r);

				int a = (sb2.filesize - sb2.data_block_start_loc)/sb2.block_size;

				printf("available no of blocks = %d\n",a);

				for (j = 0; j < sb2.next_inode_start_index; j++)
				{
					printf("COPYING INTERNAL\n");
					printf(" inode start location = %d\n",sb2.inode_startloc[j]);
					memcpy(&nd,fp2+sb2.inode_startloc[j],sizeof(nd));
					if(nd.type == 'e' && r<nd.data_block_count)
					{
						memcpy(fp2+nd.data_block_start_loc,fp1,filesize);
						strcpy(nd.name,buffrm);
						nd.type = 'f';
						nd.size = sb1.filesize;
						nd.data_block_count = nd.size/sb2.block_size;
						memcpy(fp2+sb2.inode_startloc[j],&nd,sizeof(nd));

						int rem = remove(destfilename); //delete the existing file

						if (rem == 0)
						{
							printf("file removed SUCCESFULLY\n");
						}

						int fd3 = open(destfilename,O_CREAT | O_WRONLY,mode); // creating new one where new updated data will be stored
						if (fd3 == -1)
						{
							printf("CANNOT OPEN FILE\n");
							return;
						}

						write(fd3,fp2,mnt[toindex].osfsize);
						ftruncate(fd3,mnt[toindex].osfsize);

						strcpy(mnt[toindex].osfilename,destfilename);

						return;						
					}
				}

				if (a>r)
				{

					memcpy(fp2+sb2.data_block_start_loc,fp1,filesize);
					memcpy(&sb3,fp2+sb2.data_block_start_loc,sizeof(sb3));

					struct iNode nd;
					memcpy(&nd,fp2+sb2.data_block_start_loc+sizeof(struct superblock),sizeof(nd));
					memcpy(&sb3,fp2,sizeof(sb3));
					int siz = sb3.next_inode_start_loc;
					sb3.inode_startloc[sb3.next_inode_start_index] = sb3.next_inode_start_loc;
					sb3.next_inode_start_loc = sb3.next_inode_start_loc+sizeof(struct iNode);
					sb3.next_inode_start_index = sb3.next_inode_start_index+1;
					sb3.data_block_start_loc = r*sb3.block_size + sb3.data_block_start_loc;
					sb3.free_data_block_count = (sb3.filesize - sb3.data_block_start_loc)/sb3.block_size;

					nd.type = 'f';
					strcpy(nd.name,buffrm);
					nd.size = filesize;
					nd.data_block_start_loc = sb2.data_block_start_loc;
					nd.data_block_count = r;

					memcpy(fp2,&sb3,sizeof(sb3));
					memcpy(fp2+siz,&nd,sizeof(nd));

					close(fd1);

					int rem = remove(destfilename); //delete the existing file

					if (rem == 0)
					{
						printf("file removed SUCCESFULLY\n");
					}

					int fd3 = open(destfilename,O_CREAT | O_WRONLY,mode); // creating new one where new updated data will be stored
					if (fd3 == -1)
					{
						printf("CANNOT OPEN FILE\n");
						return;
					}

					write(fd3,fp2,mnt[toindex].osfsize);
					ftruncate(fd3,mnt[toindex].osfsize);

					strcpy(mnt[toindex].osfilename,destfilename);

					//read(fd3,fp2,mnt[toindex].osfsize);
					//read(fd,fp2,mnt[toindex].osfsize);
					//memcpy(&sb2,fp2,sizeof(sb1));

				}

			}

		}

	}


	if (from[1] == ':' && to[1] == ':') //from mount path to mount path =>cp C:test D:testfile
	{

		printf("COPY FROM MOUNTED PATH TO MOUNTED PATH\n");

		memset(buffrm,0,sizeof(buffrm));
		memset(bufto,0,sizeof(bufto));
		memset(srcfilename,0,sizeof(srcfilename));
		memset(destfilename,0,sizeof(destfilename));

		strncpy(buffrm,bfcopy[0],2);
		strncpy(bufto,bfcopy[1],2);

		strcpy(srcfilename,bfcopy[0]+2);
		strcpy(destfilename,bfcopy[1]+2);

		char* m = destfilename;
		m[strlen(m)-1] = 0;

		int l11 = strlen(buffrm);
		int l2 = strlen(bufto);


		printf("PATH from = %s length  bfrm = %d\n",buffrm,l11);
		printf("PATH to =  %s length  bfto = %d\n",bufto,l2);

		l11 = strlen(srcfilename);
		l2 = strlen(destfilename);

		printf("src PATH from = %s length of srcfilename = %d\n",srcfilename,l11);
		printf("dest PATH to =  %s length of destfilename = %d\n",destfilename,l2);


		for ( j = 0; j < mntindex; j++)
		{

			int le = strlen(mnt[j].mntpart);
			printf("src PATH from = %s length = %d\n",mnt[j].mntpart,le);

			if (strncmp(mnt[j].mntpart,buffrm,2) == 0)
			{
				flag1++;
				frmindex = j;
				break;
			}
		}

		for ( j = 0; j < mntindex; j++)
		{
			if (strncmp(mnt[j].mntpart,bufto,2) == 0)
			{
				flag2++;
				toindex = j;
				break;
			}
		}

		if (flag1 == 0)
		{
			printf("%s PATH DOES NOT EXIST\n",buffrm);
			return;
		}

		if (flag2 == 0)
		{
			printf("%s PATH DOES NOT EXIST\n",bufto);
			return;
		}

		if (strncmp(mnt[frmindex].osfilename,srcfilename,3) != 0)
		{
			printf("%s FILE DOES NOT EXIST ON SPECIFIED PATH %s\n",srcfilename,buffrm);
			return;
		}

		if (strcmp(buffrm,bufto) == 0)
		{
			printf("COPY OPERATION IN SAME MOUNTED PATH\n");
			struct superblock sb1,sb2,sb;
			struct iNode nd,nd1;
			int fd1 = open(srcfilename,O_RDONLY);
			if (fd1 == -1)
			{
			 	printf("CANNOT OPEN FILE\n");
			 	return;
			}
			read(fd1,&sb1,sizeof(sb1));

			void* fp;
			fp = malloc(sb1.filesize);
			fd1 = open(srcfilename,O_RDONLY);
			read(fd1,fp,sb1.filesize);

			memcpy(&sb,fp,sizeof(sb));
			memcpy(&nd,fp+sizeof(struct superblock),sizeof(nd));

			long int sizem = sb1.filesize*2;
			void* fp2;
			fp2 = malloc(sizem);

			sb.filesize = sizem;
			sb.next_inode_start_index = 0;
			sb.next_inode_start_loc = iNodestloc;
			sb.data_block_start_loc = sb.block_size + INODE_COUNT*sb.block_size;
			sb.free_data_block_count = (sb.filesize - sb.data_block_start_loc)/sb.block_size;

			strcpy(nd.name,destfilename);
			nd.size = sizem;

			nd1.type = 'f';
			strcpy(nd1.name,srcfilename);
			nd1.size = sb1.filesize;
			nd1.data_block_start_loc = sb.data_block_start_loc;
			nd1.data_block_count = (nd1.size/sb.block_size);

			memcpy(fp2+sb.next_inode_start_loc,&nd1,sizeof(nd1));

			sb.inode_startloc[sb.next_inode_start_index] = sb.next_inode_start_loc;

			sb.next_inode_start_index = sb.next_inode_start_index + 1;
			sb.next_inode_start_loc = sb.next_inode_start_loc + sizeof(struct iNode);

			memcpy(fp2,&sb,sizeof(sb));
			memcpy(fp2+sizeof(struct superblock),&nd,sizeof(nd));


				int fd2 = open(destfilename,O_CREAT | O_WRONLY,mode);
				if (fd2 == -1)
				{
					printf("CANNOT CREATE NEW OSFILE\n");
					return;
				}
				write(fd2,fp2,sizem);

				strcpy(mnt[frmindex].osfilename,destfilename);


		}

		struct superblock sb1,sb2,sb3;
		int fd1 = open(srcfilename,O_RDONLY);
		read(fd1,&sb1,sizeof(sb1));
		printf("filesize = %ld\n",sb1.filesize);

		if (mnt[toindex].osfsize > sb1.filesize)
		{
			int fd = open(destfilename,O_RDWR);
			if (fd == -1)
			{
				fd1 = open(buffrm,O_RDONLY);
				read(fd1,&sb1,sizeof(sb1));
				printf("filesize = %ld\n",sb1.filesize);

				void* fp1;
				long int filesize = sb1.filesize;
				fp1 = malloc(sb1.filesize);

				read(fd1,fp1,sb1.filesize);
				memcpy(&sb3,fp1,sizeof(sb1));

				void* fp2;
				fp2 = malloc(mnt[toindex].osfsize);
				int fd2 = open(mnt[toindex].osfilename,O_RDONLY);
				read(fd2,fp2,mnt[toindex].osfsize);
				memcpy(&sb2,fp2,sizeof(sb2));

				int m = sb2.block_size;
				long int n = sb1.filesize;

				printf("n = %ld\n",n);
				printf("m = %d\n",m);

				int r = (n/m);
				printf("required no of blocks = %d\n",r);

				int a = (sb2.filesize - sb2.data_block_start_loc)/sb2.block_size;

				printf("available no of blocks = %d\n",a);

				if (a>r)
				{

					memcpy(fp2+sb2.data_block_start_loc,fp1,filesize);
					memcpy(&sb3,fp2+sb2.data_block_start_loc,sizeof(sb3));

					struct iNode nd,nd1;
					memcpy(&nd,fp2+sb2.data_block_start_loc+sizeof(struct superblock),sizeof(nd));
					memcpy(&sb3,fp2,sizeof(sb3));
					int siz = sb3.next_inode_start_loc;
					sb3.inode_startloc[sb3.next_inode_start_index] = sb3.next_inode_start_loc;
					sb3.next_inode_start_loc = sb3.next_inode_start_loc+sizeof(struct iNode);
					sb3.next_inode_start_index = sb3.next_inode_start_index+1;
					sb3.data_block_start_loc = r*sb3.block_size + sb3.data_block_start_loc;
					sb3.free_data_block_count = (sb3.filesize - sb3.data_block_start_loc)/sb3.block_size;

					nd.type = 'f';
					strcpy(nd.name,srcfilename);
					nd.size = filesize;
					nd.data_block_start_loc = sb2.data_block_start_loc;
					nd.data_block_count = r;

					memcpy(&nd1,fp2+sizeof(struct superblock),sizeof(nd1));
					strcpy(nd1.name,destfilename);


					memcpy(fp2,&sb3,sizeof(sb3));
					memcpy(fp2+sizeof(struct superblock),&nd1,sizeof(nd1));
					memcpy(fp2+siz,&nd,sizeof(nd));

					close(fd1);
					close(fd2);

					int fd3 = open(destfilename,O_CREAT | O_WRONLY,mode);
					if (fd3 == -1)
					{
						printf("CANNOT OPEN FILE\n");
						return;
					}
					write(fd3,fp2,mnt[toindex].osfsize);
					ftruncate(fd3,mnt[toindex].osfsize);

					read(fd3,fp2,mnt[toindex].osfsize);

					memcpy(&sb2,fp2,sizeof(sb1));

					strcpy(mnt[toindex].osfilename,destfilename);

				}

				else
				{
					printf("CANNOT COPY....FILE SIZE EXCEED\n");
					return;
				}


			}
			else
			{

				void* fp1;
				long int filesize = sb1.filesize;
				fp1 = malloc(sb1.filesize);

				read(fd1,fp1,sb1.filesize);
				memcpy(&sb3,fp1,sizeof(sb1));

				void* fp2;
				fp2 = malloc(mnt[toindex].osfsize);
				read(fd,fp2,mnt[toindex].osfsize);
				memcpy(&sb2,fp2,sizeof(sb2));

				int m = sb2.block_size;
				long int n = sb1.filesize;

				printf("n = %ld\n",n);
				printf("m = %d\n",m);

				int r = (n/m);
				printf("required no of blocks = %d\n",r);

				int a = (sb2.filesize - sb2.data_block_start_loc)/sb2.block_size;

				printf("available no of blocks = %d\n",a);

				if (a>r)
				{

					memcpy(fp2+sb2.data_block_start_loc,fp1,filesize);
					memcpy(&sb3,fp2+sb2.data_block_start_loc,sizeof(sb3));

					struct iNode nd;
					memcpy(&nd,fp2+sb2.data_block_start_loc+sizeof(struct superblock),sizeof(nd));
					memcpy(&sb3,fp2,sizeof(sb3));
					int siz = sb3.next_inode_start_loc;
					sb3.inode_startloc[sb3.next_inode_start_index] = sb3.next_inode_start_loc;
					sb3.next_inode_start_loc = sb3.next_inode_start_loc+sizeof(struct iNode);
					sb3.next_inode_start_index = sb3.next_inode_start_index+1;
					sb3.data_block_start_loc = r*sb3.block_size + sb3.data_block_start_loc;
					sb3.free_data_block_count = (sb3.filesize - sb3.data_block_start_loc)/sb3.block_size;

					nd.type = 'f';
					strcpy(nd.name,srcfilename);
					nd.size = filesize;
					nd.data_block_start_loc = sb2.data_block_start_loc;
					nd.data_block_count = r;

					memcpy(fp2,&sb3,sizeof(sb3));
					memcpy(fp2+siz,&nd,sizeof(nd));

					close(fd1);

					int rem = remove(destfilename);

					if (rem == 0)
					{
						printf("file removed SUCCESFULLY\n");
					}

					int fd3 = open(destfilename,O_CREAT | O_WRONLY,mode);
					if (fd3 == -1)
					{
						printf("CANNOT OPEN FILE\n");
						return;
					}
					write(fd3,fp2,mnt[toindex].osfsize);
					ftruncate(fd3,mnt[toindex].osfsize);

					strcpy(mnt[toindex].osfilename,destfilename);

					read(fd3,fp2,mnt[toindex].osfsize);

					//read(fd,fp2,mnt[toindex].osfsize);

					memcpy(&sb2,fp2,sizeof(sb1));

				}

			}
		}

	}


	else if (from[1] == ':' && to[1] != ':') // from mount to osfile => cp C:test1 os2
	{
		printf("COPY FROM MOUNT PATH TO FILE\n");

		memset(buffrm,0,sizeof(buffrm));
		memset(bufto,0,sizeof(bufto));
		memset(srcfilename,0,sizeof(srcfilename));
		memset(destfilename,0,sizeof(destfilename));

		strncpy(buffrm,bfcopy[0],2); //mount path name
		strcpy(srcfilename,bfcopy[0]+2); //os file name from data will be copied
		strcpy(destfilename,bfcopy[1]); //os file name

		char* m = destfilename;
		m[strlen(m)-1] = 0;

		for ( j = 0; j < mntindex; j++)
		{

			if (strncmp(mnt[j].mntpart,buffrm,2) == 0)
			{
				flag1++;
				frmindex = j;
				break;
			}
		}

		if (flag1 == 0)
		{
			printf("%s PATH DOES NOT EXIST\n",buffrm);
			return;
		}


		if (strncmp(mnt[frmindex].osfilename,srcfilename,3) != 0)
		{
			printf("%s FILE DOES NOT EXIST ON SPECIFIED PATH %s\n",srcfilename,buffrm);
			return;
		}

		struct superblock sb1,sb2,sb3;
		int fd1 = open(srcfilename,O_RDONLY);

		if (fd1 == -1)
		{
			printf("FILE DOES NOT EXIST\n");
		}

		printf("filesize1 = %ld\n",mnt[frmindex].osfsize);

		int fd2 = open(destfilename,O_RDONLY);
		if (fd2 == -1)
			{

				printf("COPY TO NEW OSFILE\n");
				fd1 = open(srcfilename,O_RDONLY);
				void* fp;

				struct superblock sb;
				struct iNode nd;
				fp = malloc(mnt[frmindex].osfsize);
				read(fd1,fp,mnt[frmindex].osfsize);

				memcpy(&sb,fp,sizeof(sb));
				memcpy(&nd,fp+sizeof(struct superblock),sizeof(nd));

				int n;
				printf("ENTER THE SIZE OF THE FILE (MB)\n");
				scanf("%d",&n);
				long int sizem = 1024*1024*n;

				sb.filesize = sizem;
				sb.data_block_start_loc = sb.block_size + INODE_COUNT*sb.block_size;
				sb.free_data_block_count = (sb.filesize - sb.data_block_start_loc)/sb.block_size;

				strcpy(nd.name,destfilename);
				nd.size = sizem;

				memcpy(fp,&sb,sizeof(sb));
				memcpy(fp+sizeof(struct superblock),&nd,sizeof(nd));

				int fd = open(destfilename,O_CREAT | O_WRONLY,mode);
				if (fd == -1)
				{
					printf("CANNOT CREATE NEW OSFILE\n");
					return;
				}
				write(fd,fp,mnt[frmindex].osfsize);
				ftruncate(fd,sizem);

			}
		else
			{

			printf("COPY TO EXISTING OSFILE\n");
			read(fd2,&sb2,sizeof(sb2));

			close(fd1);
			close(fd2);

			if (sb2.filesize > mnt[frmindex].osfsize)
			{

				void* fp1;
				long int filesize = mnt[frmindex].osfsize;
				fp1 = malloc(filesize);

				fd1 = open(srcfilename,O_RDONLY);
				read(fd1,fp1,filesize);
				memcpy(&sb1,fp1,sizeof(sb1));

				void* fp2;
				fp2 = malloc(sb2.filesize);

				fd2 = open(destfilename,O_RDONLY);
				read(fd2,fp2,sb2.filesize);
				memcpy(&sb2,fp2,sizeof(sb2));

				int m = sb2.block_size;
				long int n = sb1.filesize;

				printf("n = %ld\n",n);
				printf("m = %d\n",m);

				int r = (n/m);
				printf("required no of blocks = %d\n",r);

				int a = (sb2.filesize - sb2.data_block_start_loc)/sb2.block_size;

				printf("available no of blocks = %d\n",a);

				if (a>r)
				{

					memcpy(fp2+sb2.data_block_start_loc,fp1,filesize);
					memcpy(&sb3,fp2+sb2.data_block_start_loc,sizeof(sb3));

					struct iNode nd;
					memcpy(&nd,fp2+sb2.data_block_start_loc+sizeof(struct superblock),sizeof(nd));
					memcpy(&sb3,fp2,sizeof(sb3));
					int siz = sb3.next_inode_start_loc;
					sb3.inode_startloc[sb3.next_inode_start_index] = sb3.next_inode_start_loc;
					sb3.next_inode_start_loc = sb3.next_inode_start_loc+sizeof(struct iNode);
					sb3.next_inode_start_index = sb3.next_inode_start_index+1;
					sb3.data_block_start_loc = r*sb3.block_size + sb3.data_block_start_loc;
					sb3.free_data_block_count = (sb3.filesize - sb3.data_block_start_loc)/sb3.block_size;

					nd.type = 'f';
					strcpy(nd.name,srcfilename);
					nd.size = filesize;
					nd.data_block_start_loc = sb2.data_block_start_loc;
					nd.data_block_count = r;

					memcpy(fp2,&sb3,sizeof(sb3));
					memcpy(fp2+siz,&nd,sizeof(nd));

					close(fd1);
					close(fd2);

					int status = remove(destfilename);
					if (status == 0)
					{
						printf("removed SUCCESFULLY\n");
					}

					int fd3 = open(destfilename,O_CREAT | O_WRONLY,mode);
					if (fd3 == -1)
					{
						printf("CANNOT OPEN FILE\n");
						return;
					}

					write(fd3,fp2,sb2.filesize);
					ftruncate(fd3,sb2.filesize);
					read(fd3,fp2,sb2.filesize);

					memcpy(&sb2,fp2,sizeof(sb2));

				}

				else
				{
					printf("CANNOT COPY....FILE SIZE EXCEED\n");
					return;
				}
			}
		}
	}

}
void remv(char buff4[])
{

	char* token;
	char* bfrmv1 = buff4;
	char* bfrmv[MAX];
	int i = 0;
	int j;

	while((token = strtok_r(bfrmv1," ",&bfrmv1)))
	{
		bfrmv[i] = token;
		i++;
	}

	if (i != 1)
	{
		printf("GIVE INSTRUCTION PROPERLY\n");
		return;
	}

	char path[10];
	char filename[10];

	strncpy(path,bfrmv[0],2);
	strcpy(filename,bfrmv[0]+2);

	char *m = filename;
	m[strlen(m)-1] = 0;

	int len = strlen(filename);
	printf("filename = %s with len = %d\n",filename,len);
	 len = strlen(path);
	printf("path = %s with len = %d\n",path,len);

	int toindex;
	int flag = 0;

	for ( j = 0; j < mntindex; j++)
	{

			if (strncmp(mnt[j].mntpart,path,2) == 0)
			{
				flag++;
				toindex = j;
				break;
			}

	}

	if (flag == 0)
	{
		printf("PATH DOES NOT EXIST\n");
		return;
	}


	if (strcmp(mnt[toindex].osfilename,filename) == 0)
	{
		int status = remove(filename);
		if (status == 0)
		{
			printf("FILE REMOVED SUCCESFULLY\n");
			return;
		}


	}
	else
	{
		printf("else part\n");
		struct superblock sb,sb1,sb2;
		struct iNode nd;
		printf("osfilename = %s\n",mnt[toindex].osfilename);
		int fd = open(mnt[toindex].osfilename,O_RDONLY);
		if (fd == 0)
		{
			printf("CANNOT OPEN FILE\n");
			return;
		}

		void* fp;
		printf("size of fp = %ld\n",mnt[toindex].osfsize);
		fp = malloc(mnt[toindex].osfsize);
		read(fd,fp,mnt[toindex].osfsize);
		memcpy(&sb,fp,sizeof(sb));
		int flag1 = 0;

		long int datablockstartloc;
		int inodloc;
		int index;
		long int size;

		for ( j = 0; j < sb.next_inode_start_index; j++)
		{

			printf("inode start location = %d\n",sb.inode_startloc[j]);
			memcpy(&nd,fp+sb.inode_startloc[j],sizeof(nd));
			printf("filename = %s\n",nd.name);
			if (strcmp(nd.name,filename) == 0)
			{
				flag1++;
				index = j;
				inodloc = sb.inode_startloc[j];
				datablockstartloc = nd.data_block_start_loc;
				size = nd.size;
				break;
			}
		}

		if (flag1 == 0)
		{
			printf("FILE DOES NOT EXIST IN THE MOUNTED PATH FILE\n");
			return;
		}

		memcpy(&nd,fp+inodloc,sizeof(nd));
		nd.type = 'e';
		strcpy(nd.name,"empty");

		memcpy(fp+inodloc,&nd,sizeof(nd));

		memset(fp+datablockstartloc,0,size);

		int stat = remove(mnt[toindex].osfilename);
		{
			printf("remove SUCCESFULLY\n");
		}

		int fd1 = open(mnt[toindex].osfilename,O_CREAT | O_WRONLY,mode);
		if (fd1 == -1)
		{
			printf("CANNOT OPEN FILE\n");
		}

		write(fd1,fp,mnt[toindex].osfsize);

	}

}

void renamefile(char buff5[])
{
	char* token;
	char* bfrenam1 = buff5;
	char* bfrenam[MAX];
	int i = 0;
	int j;
	int flag1 = 0;
	int frmindex;

	char buffrm[10];
	char bufto[10];

	char srcfilename[10];
	char destfilename[10];

	memset(buffrm,0,sizeof(buffrm));
	memset(bufto,0,sizeof(bufto));
	memset(srcfilename,0,sizeof(srcfilename));
	memset(destfilename,0,sizeof(destfilename));


	while((token = strtok_r(bfrenam1," ",&bfrenam1)))
	{
		bfrenam[i] = token;
		i++;
	}

	if (i != 2)
	{
		printf("GIVE INSTRUCTION PROPERLY\n");
		return;
	}

	strncpy(buffrm,bfrenam[0],2);//path name D:
	strncpy(bufto,bfrenam[1],2);//path name must be same as buffrm

	strcpy(srcfilename,bfrenam[0]+2);//old file name
	strcpy(destfilename,bfrenam[1]+2);//new file name

	char* m = destfilename;
	m[strlen(m)-1] = 0;


		int l11 = strlen(buffrm);
		int l2 = strlen(bufto);


		printf("PATH from = %s length  bfrm = %d\n",buffrm,l11);
		printf("PATH to =  %s length  bfto = %d\n",bufto,l2);

		l11 = strlen(srcfilename);
		l2 = strlen(destfilename);

		printf("src PATH from = %s length of srcfilename = %d\n",srcfilename,l11);
		printf("dest PATH to =  %s length of destfilename = %d\n",destfilename,l2);

	if (strcmp(buffrm,bufto) != 0)
	{
		printf("PATH ARE NOT SAME .....CANNOT RENAME\n");
		return;
	}

	for ( j = 0; j < mntindex; j++)
		{

			if (strncmp(mnt[j].mntpart,buffrm,2) == 0)
			{
				flag1++;
				frmindex = j;
				break;
			}
		}

		if (flag1 == 0)
		{
			printf("%s PATH DOES NOT EXIST\n",buffrm);
			return;
		}


		if (strncmp(mnt[frmindex].osfilename,srcfilename,3) != 0)
		{
			printf("%s FILE DOES NOT EXIST ON SPECIFIED PATH %s\n",srcfilename,buffrm);
			return;
		}


		int stat = rename(srcfilename,destfilename);
		strcpy(mnt[frmindex].osfilename,destfilename);
}



int main()
{

	int i;
	while(1)
	{
		printf(RED"MYFS>"RESET);
		fgets(buff,MAX,stdin);

		int len = strlen(buff);

		if(strncmp(buff,"exit",4) == 0) //exiting from the filesystem
		{
			exit(0);
		}

		else if (strncmp(buff,"mkfs",4) == 0) //creating a file
		{

			strcpy(tempbuff,buff+5);
			mkfs(tempbuff);
		}

		else if(strncmp(buff,"use",3) == 0) //mount a file in a specific path
		{
			strcpy(tempbuff,buff+4);

			use(tempbuff);
		}

		else if (strncmp(buff,"ls",2) == 0) //to see the contents of mounted path
		{

			strcpy(tempbuff,buff+3);

			ls(tempbuff);
		}

		else if (strncmp(buff,"cp",2) == 0) // for prforming copy operation
		{

			strcpy(tempbuff,buff+3);
			copy(tempbuff);
		}

		else if (strncmp(buff,"rm",2) == 0) //for removing a file
		{
			strcpy(tempbuff,buff+3);
			remv(tempbuff);

		}

		else if (strncmp(buff,"mv",2) == 0) // for renaming file
		{
			strcpy(tempbuff,buff+3);
			renamefile(tempbuff);
		}

	}
	
	return 0;
}
