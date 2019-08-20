#include"header.h"
bool searchNode(struct Node** head_ref,int data)
{
	
	struct Node* current = *head_ref;
	if (current == NULL)
	{
		
		return false;
	}
	else
	{
		while(current!=NULL)
		{
			if (current->data == data)
			{
				
				return true;
			}
			current = current->next;
		}
		
		return false;
	}

	

}

void insertfirst(struct Node** head_ref,int value,pthread_mutex_t *p_mutex)
{
	pthread_mutex_lock(p_mutex);
	struct Node* new = (struct Node*) malloc(sizeof(struct Node));
	new->data = value;
	new->next = *head_ref;
	*head_ref = new;
	printf("%d => NODE IS ADDED AT THE BEGINNING OF THE LIST\n",value);
	pthread_mutex_unlock(p_mutex);
}

void insertlast(struct Node** head_ref,int value,pthread_mutex_t* p_mutex)
{
	pthread_mutex_lock(p_mutex);
	struct Node* new = (struct Node*) malloc(sizeof(struct Node));
	new->data = value;
	struct Node* last = *head_ref;
	if(*head_ref == NULL)
	{	
		new->next = *head_ref;
		*head_ref = new;
		printf("%d => THE NODE IS ADDED AS HEAD\n",value);
	}
	else
	{
		while(last->next!=NULL)
		{
			last = last->next;
		}
		
		last->next = new;
		new->next = NULL;	
		printf("%d => NODE IS ADDED AT THE LAST OF THE LIST\n",value);
	}

    pthread_mutex_unlock(p_mutex);
}

void insertmid(struct Node** head_ref,int data,int value,pthread_mutex_t* p_mutex)
{
	pthread_mutex_lock(p_mutex);
	struct Node* current = *head_ref;
	struct Node* new = (struct Node*)malloc(sizeof(struct Node));
	new->data = value;

	if(*head_ref == NULL)
	{
		*head_ref = new;
		printf("%d => THE NODE IS ADDED AS HEAD\n",value);
	}
	
	else if(searchNode(&current,data))
	{
		
		printf("NODE IS FOUND\n");
		while(current->data!=data)
		{
			current = current->next;
		}

		new->next = current->next;
		current->next = new;
		printf(" %d => THE NODE IS ADDED AFTER %d\n",value,data);
	}

	else
	{
		printf("THE NODE CAN NOT BE ADDED\n");

	}
	
	pthread_mutex_unlock(p_mutex);

}

void deleteNode(struct Node** head_ref,int data,pthread_mutex_t* p_mutex)
{
	pthread_mutex_lock(p_mutex);
	struct Node* current = *head_ref;
	struct Node* prev;

	if (current == NULL)
	{
		printf("LIST IS EMPTY\n");
		return;
	}
	else if(searchNode(&current,data))
	{
		
	
		printf("NODE FOUND\n");

		if (current->data == data)
		{
			printf("%d => THE NODE IS DELETED\n",current->data);
			*head_ref = current->next;
			free(current);
		}
		else
		{

		while(current->data!=data)
		{
			prev = current;
			current = current->next;
		}

		prev->next = current->next;
		printf("%d => THE NODE IS DELETED\n",current->data);
		free(current);	

		}
	}

	else
	{
		printf("NODE NOT FOUND\n");
	}

  	pthread_mutex_unlock(p_mutex);
}

void displaylist(struct Node** head_ref,pthread_mutex_t* p_mutex)
{
	pthread_mutex_lock(p_mutex);
	struct Node* current = *head_ref;
	if (current == NULL)
	{
		printf("LIST IS EMPTY\n");
		return;
	}
	while(current!=NULL)
	{
		printf("VALUE=%d\n",current->data);
		current = current->next;
	}

	pthread_mutex_unlock(p_mutex);
}
