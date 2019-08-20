#include"header.h"
void *runner( void *param);

int main()
{
	int n;
	int status;
	printf("ENTER THE NO OF LINKEDLIST\n");
	scanf("%d",&n);
	struct Node* header[n];
	pthread_mutex_t mutex_array[n];
	pthread_t tid1,tid2,tid3;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int i;



	for (i = 0; i < n;i++)
	{
		header[i] = NULL;
		pthread_mutex_init(mutex_array + i, NULL);
	}



	struct insert* data1;
	data1 = (struct insert *) malloc(sizeof(struct insert));
	data1->value = 50;
	data1->head_ref = &header[1];
	data1->p_mutex = mutex_array+1;
	//pthread_mutex_init(data1->p_mutex,NULL);
	status = pthread_create(&tid1, &attr, runner, data1);


	struct insert* data2;
	data2 = (struct insert *) malloc(sizeof(struct insert));
	data2->value = 80;
	data2->head_ref = &header[1];
	data2->p_mutex = mutex_array+1;
	//pthread_mutex_init(data2->p_mutex,NULL);
	status = pthread_create(&tid2, &attr, runner, data2);


	struct insert* data3;
	data3 = (struct insert *) malloc(sizeof(struct insert));
	data3->value = 100;
	data3->head_ref = &header[2];
	data3->p_mutex = mutex_array+2;
	//pthread_mutex_init(data3->p_mutex,NULL);
	status = pthread_create(&tid3, &attr, runner, data3);

	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	pthread_join(tid3,NULL);

	return 0;

}

int r(int n)
{
	
}

void *runner(void *param)
{

	struct insert *data; /* the structure that holds our data */
    data = (struct insert *)param; /* the structure that holds our data */
    int value = data->value;

    insertfirst(data->head_ref,value,data->p_mutex);
    displaylist(data->head_ref,data->p_mutex);


    free(param);
}
