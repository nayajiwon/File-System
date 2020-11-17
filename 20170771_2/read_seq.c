#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<time.h>
#define BUFFER_SIZE 120

int main(int argc, char *argv[]){
	
	int fd;
	struct stat statbuf;
	struct timeval start, end;
	long time;
	int recordSize;
	char buf[BUFFER_SIZE];	


	if((fd=open(argv[1], O_RDWR))<0){
		fprintf(stderr, "open error");
	}

	
	if(stat(argv[1], &statbuf)<0){
		fprintf(stderr, "statbuf error\n");
	}

	recordSize = statbuf.st_size/100;
	//명세1. 레코드의 개수를 간단히 계산하고 	
	
	gettimeofday(&start, NULL);
	
	//명세2. 그 개수를 순차적 레코드 읽기에 사용한다 
	for(int i=0; i<recordSize; i++){ 
		read(fd,buf,100);
	}

	gettimeofday(&end, NULL);
			
	//1sec = 1000000usec
	time = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);

	printf("#records : %d timecost : %ld us", recordSize, time);

	return 0;
}
