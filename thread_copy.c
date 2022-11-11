#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pthread.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<string.h>
#include<sys/fcntl.h>

#define THREAD_NO 10
char* ptr = NULL;
int blocksize;
//---------------------------------------------------------------
int create_map(void)
{
	int fd = open("flag.png",O_RDONLY);
	int Fsize = lseek(fd,0,SEEK_END);
	ptr = mmap(NULL,Fsize,PROT_READ,MAP_PRIVATE,fd,0);
	return Fsize;
}
//---------------------------------------------------------------
void* thread_copy(void * arg)
{
	int i = *(int*)arg;
	int POS = i*blocksize;
	printf("copy thread 0x%x,i = %d,POS = %d,blocksize = %d\n",(unsigned int)pthread_self(),i,POS,blocksize);
	//char buf[blocksize];
	//bzero(buf,sizeof(buf));
	//snprintf(buf,blocksize+1,"%s",ptr+POS);
	//打开目标文件，不存在创建
	int dest_fd = open("copy.png",O_RDWR|O_CREAT,0664);
	lseek(dest_fd,POS,SEEK_SET);
	//写入拷贝数据
	//write(dest_fd,buf,strlen(buf));
	write(dest_fd,POS+ptr,blocksize);
	//for(double j = 1;j<=10;j++){
	//	printf("%.2f%% ",j*(i+1)*(size/blocksize));
	//}
	//printf("\n");
	close(dest_fd);
	pthread_exit(NULL);
		
}
//-----------------------------------------------------------
int thread_cur(const char* File)
{
	int fd = open(File,O_RDONLY);
	int Fsize = lseek(fd,0,SEEK_END);
	
	if(Fsize% THREAD_NO == 0)
		return Fsize / THREAD_NO;
	else
		return Fsize/THREAD_NO +1;

}
//----------------------------------------------------------
int thread_create(int Fsize)
{
	pthread_t tids[THREAD_NO];
	int i = 0;
	for(i = 0;i<THREAD_NO;i++){
		pthread_create(&tids[i],NULL,thread_copy,(void*)&i);
        int rfd = open("copy.png",O_RDONLY);
        int size = lseek(rfd,0,SEEK_END);
		double d = size*1.0/Fsize*100;
		
    	printf("%.2f%% copied ",d);
        close(rfd);
	
        usleep(10000);
          	
	}
	while(i--){
		pthread_join(tids[i],NULL);
	}
	return 0;
}
//-------------------------------------------------------
int main(void)
{
	int Fsize;
	//将拷贝数据加载到映射内存
	Fsize = create_map();
	//获取拷贝大小
	blocksize = thread_cur("flag.png");
    //创建多线程
	thread_create(Fsize);
	//process_bar("copy.png",Fsize);//进度条
	munmap(ptr,Fsize);
	printf("thread copy its done..\n");
	return 0;
}







