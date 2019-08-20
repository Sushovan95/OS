#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
#include<unistd.h>
struct Node
{
	int data;
	struct Node* next;
	
};


struct insert
{
	int value;
	pthread_mutex_t *p_mutex; 
	struct Node** head_ref;

};

bool searchNode(struct Node** head_ref,int data);
void insertfirst(struct Node** head_ref,int value,pthread_mutex_t *p_mutex);
void insertlast(struct Node** head_ref,int value,pthread_mutex_t* p_mutex);
void insertmid(struct Node** head_ref,int data,int value,pthread_mutex_t* p_mutex);
void deleteNode(struct Node** head_ref,int data,pthread_mutex_t* p_mutex);
void displaylist(struct Node** head_ref,pthread_mutex_t* p_mutex);



