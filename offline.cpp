#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <deque>


#define BOOTH_SERVE_TIME 5 	//second

#define STD_DEP 0
#define STD_WTH 1
#define TCR 2

using namespace std;

sem_t 	bankEmpty,   	// 	total empty slots in bank
		queueXFull,		//	# of people in queue X
		queueYFull,		//	# of people in queue Y
		queueZFull;		//	# of people in queue Z
		
pthread_mutex_t xLock,
				yLock,
				zLock,
				exitLock;
		

deque<int> 	loungeX,
			loungeY, 
			loungeZ;


int teacherCount = 0, stdCount = 0;

int getSem(sem_t* arg){
	int u;
	sem_getvalue(arg, &u);
	return u;
}

void display(){

//	pthread_mutex_lock(&xLock);
//	pthread_mutex_lock(&yLock);
//	pthread_mutex_lock(&zLock);
	
	for(int i=0;i<100;++i) printf("\n");
	
	deque<int>::iterator p;
	printf(" X %d #" ,getSem(&queueXFull));
	int i;
	for(p=loungeX.begin(),i=0; p != loungeX.end(); ++p,++i)
	{
		printf(" %s [T%d]", i==5?"\t\t":"", *p);
	}
	printf("\n");
	printf(" Y %d #", getSem(&queueYFull));
	for(p=loungeY.begin(), i=0; p != loungeY.end(); ++p, ++i)
	{
		printf(" %s [S%d]", i==5?"\t\t":"", *p);
	}
	printf("\n");
	printf(" Z %d #", getSem(&queueZFull));
	for(p=loungeZ.begin(), i=0; p != loungeZ.end(); ++p, ++i)
	{
		printf(" %s [S%d]", i==5?"\t\t":"", *p);
	}
	printf("\n");
	
//	pthread_mutex_unlock(&xLock);
//	pthread_mutex_unlock(&yLock);
//	pthread_mutex_unlock(&zLock);
	
	
	
}

void * customerProducer(void* arg){
	
	while(1){
	
		usleep(1000* ( 100 + (rand()%3)*100 )); // 1000-3000 ms
		
		sem_wait(&bankEmpty);
		
		int r = rand()%3;
		
//		printf("%d\n", r);
		
		if(r==STD_DEP){
		
			++stdCount;
//			printf("Student %d entered for deposit\n", stdCount);

			pthread_mutex_lock(&yLock);
			loungeY.push_back(stdCount);
			pthread_mutex_unlock(&yLock);
			
			sem_post(&queueYFull);
			
			display();
			
			
		}
		else if(r==STD_WTH){
		
			++stdCount;
//			printf("Student %d entered for withdrawal\n", stdCount);
			pthread_mutex_lock(&zLock);
			loungeZ.push_back(stdCount);
			pthread_mutex_unlock(&zLock);
			
			sem_post(&queueZFull);
			
			display();
		}
		

		else{
		++teacherCount;

			pthread_mutex_lock(&xLock);
			loungeX.push_back(teacherCount);
			pthread_mutex_unlock(&xLock);
			
			sem_post(&queueXFull);
			
			display();
		}
		
		
	}
	
	
}


void * boothFunctionX(void * arg){
	
	while(1){
	
		sem_wait(&queueXFull);
		sleep(BOOTH_SERVE_TIME);
		
		int teacherId = loungeX.front();
		pthread_mutex_lock(&xLock);
			loungeX.pop_front();
			pthread_mutex_unlock(&xLock);
		
		display();
//		printf("Booth X serving teacher %d\n", teacherId);
		
//		printf("Booth X ended serving teacher %d\n", teacherId);
		
		pthread_mutex_lock(&exitLock);
		sem_post(&bankEmpty);
//		printf("Teacher %d left the bank\n", teacherId);
		pthread_mutex_unlock(&exitLock);
	}
}


void * boothFunctionY(void * arg){
	
	while(1){
	
		sem_wait(&queueYFull);
		sleep(BOOTH_SERVE_TIME);
		
		int studentId = loungeY.front();
		pthread_mutex_lock(&yLock);
			loungeY.pop_front();
			pthread_mutex_unlock(&yLock);
		display();
	
		pthread_mutex_lock(&exitLock);
		sem_post(&bankEmpty);
		pthread_mutex_unlock(&exitLock);
	}
}

void * boothFunctionZ(void * arg){
	
	while(1){
	
		sem_wait(&queueZFull);
		sleep(BOOTH_SERVE_TIME);
		
		int studentId = loungeZ.front();
		pthread_mutex_lock(&zLock);
			loungeZ.pop_front();
			pthread_mutex_unlock(&zLock);
		display();
		
		pthread_mutex_lock(&exitLock);
		sem_post(&bankEmpty);
		pthread_mutex_unlock(&exitLock);
	}
}


void init()
{
	sem_init(&bankEmpty,0,20);
	
	sem_init(&queueXFull,0,0);
	sem_init(&queueYFull,0,0);
	sem_init(&queueZFull,0,0);
	
	pthread_mutex_init(&exitLock,0);
	
	pthread_mutex_init(&xLock,0);
	pthread_mutex_init(&yLock,0);
	pthread_mutex_init(&zLock,0);
	
	srand(time(NULL));

}



int main(){
//	freopen ("myfile.txt","w",stdout);
	
	pthread_t producerThread, xThread, yThread, zThread;
	
	init();
	pthread_create(&producerThread,NULL,customerProducer,NULL);
	
	
	
	pthread_create(&xThread,NULL,boothFunctionX, NULL);
	pthread_create(&yThread,NULL,boothFunctionY, NULL);
	pthread_create(&zThread,NULL,boothFunctionZ, NULL);
	
	while(1);
	return 0;
	
}


