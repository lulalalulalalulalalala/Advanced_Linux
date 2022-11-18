#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<sys/mman.h>
#include<pthread.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<signal.h>
using namespace std;

int info=1;

pthread_mutex_t lock;
pthread_rwlock_t rwlock;

pthread_cond_t daytime;
pthread_cond_t night;
pthread_cond_t end;


char buf1[1024];
char buf2[1024];
int fd;
int fd2;
FILE * fp;

void * THREAD_A(void*arg)
{
	pthread_detach(pthread_self());  
	while(1)
	{
		pthread_mutex_lock(&lock);     
		if(info!=1)
			pthread_cond_wait(&night,&lock);   
		if(NULL==fgets(buf1, 1024, fp))   
		{
			cout<<"over"<<endl;
			exit(0);
		}
		cout<<"buf1 = "<< buf1 <<endl;

		info=0;
		pthread_mutex_unlock(&lock);   
		pthread_cond_signal(&daytime);    
	}
	pthread_exit(NULL);
}

void * THREAD_B(void*arg)
{
	pthread_detach(pthread_self());
	while(1)
	{
		pthread_mutex_lock(&lock);
		if(info!=0)
			pthread_cond_wait(&daytime,&lock);
		info=2;
		char char1[7]="E CamX";
		char char2[13]="E CHIUSECASE";
		if(strstr(buf1,char1)==NULL&&strstr(buf1,char2)==NULL)    
		{
			memset(buf1,0,sizeof(buf1));     
		}
		else
		{
			strcpy(buf2,buf1);       
			cout<<"strcpy buf2 = "<<buf2 <<endl;
		}
		memset(buf1,0,sizeof(buf1));
		pthread_mutex_unlock(&lock);
		pthread_cond_signal(&end);
	}
	pthread_exit(NULL);
}
void * THREAD_C(void*arg)
{
	pthread_detach(pthread_self());
	while(1)
	{
		pthread_rwlock_rdlock(&rwlock);
		if(info!=2)
			pthread_cond_wait(&end,&lock);
		info=1;
		write(fd2,buf2,strlen(buf2));
		memset(buf2,0,sizeof(buf2));
		pthread_rwlock_unlock(&rwlock);
		pthread_cond_signal(&night);
	}
}

int main(void)
{
	fp=fopen("ERROR.log","r");              
	fd2=open("Result.log",O_RDWR|O_CREAT,0664);   
	
	pthread_mutex_init(&lock,NULL);           
	pthread_rwlock_init(&rwlock,NULL);
	pthread_cond_init(&daytime,NULL);
	pthread_cond_init(&night,NULL);
	pthread_cond_init(&end,NULL);

	pthread_t tids[3];
	pthread_create(&tids[0],NULL,THREAD_A,NULL);
	pthread_create(&tids[1],NULL,THREAD_B,NULL);
	pthread_create(&tids[2],NULL,THREAD_C,NULL);
	
	while(1)
		sleep(1);

	pthread_mutex_destroy(&lock);
	pthread_rwlock_destroy(&rwlock);
	pthread_cond_destroy(&daytime);
	pthread_cond_destroy(&night);
	pthread_cond_destroy(&end);
	
	return 0;
}
