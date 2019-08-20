#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>


struct Node
{
		int data;
		struct Node* next;
		
};

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

void insertfirst(struct Node** head_ref,int value)
	{
		struct Node* new = (struct Node*) malloc(sizeof(struct Node));
		new->data = value;
		
		new->next = *head_ref;
		*head_ref = new;
		printf("%d => NODE IS ADDED AT THE BEGINNING OF THE LIST\n",value);
	}
	
	
void insertlast(struct Node** head_ref,int value)
{
	struct Node* new = (struct Node*) malloc(sizeof(struct Node));
	new->data = value;
	struct Node* last = *head_ref;
	if(*head_ref == NULL)
	{
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
}

void insertmid(struct Node** head_ref,int data,int value)
{
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
	

}

void displaylist(struct Node** head_ref)
{
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
}

void deleteNode(struct Node** head_ref,int data)
{
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
	
	while(current->data!=data)
	{
		prev = current;
		current = current->next;
	}

	prev->next = current->next;
	printf("%d => THE NODE IS DELETED\n",current->data);
	free(current);	

	}
	else
	{
		printf("NODE NOT FOUND\n");
	}

}
	
int main()
{
	struct Node* head = NULL;
	insertlast(&head,4);
	insertfirst(&head,5);
	insertfirst(&head,6);
	insertlast(&head,7);
	insertlast(&head,8);
	insertmid(&head,5,10);
	displaylist(&head);
	deleteNode(&head,10);
	insertmid(&head,50,20);
	deleteNode(&head,50);
	deleteNode(&head,6);
	printf("MODIFIED LIST\n");
	displaylist(&head);
	
	return 0;
	
}
