#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<semaphore.h>


//semaphore to control sleep and wake up
sem_t empty;
sem_t full;



void init_semaphore()
{
	sem_init(&empty,0,100);
	sem_init(&full,0,0);
}


void * producerFunc(void* as){
	int i = 0;
	while(1){
		
		i++;
		printf("13 x %d = %d\n", i, 13*i);
		usleep(1000*50);
	}
	
}

int main(){
	
	pthread_t th1;
	
	pthread_create(&th1,NULL,producerFunc,NULL);
	
	while(1);
	return 0;
	
}


