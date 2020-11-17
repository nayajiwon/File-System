#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#define BUFFER_SIZE 1024
char buf[BUFFER_SIZE];
char size[BUFFER_SIZE];
int main(int argc, char *argv[]){
	int fd;
	int offset1;
	int offset2;
	char c;
	int i;
	int length;
	int sizeofFile;
	
	if((fd=open(argv[1], O_RDWR))<0){
		exit(1);
	}

	
	while(read(fd,size,BUFFER_SIZE)>0){
	}

	sizeofFile = strlen(size); 

	if((offset1 = lseek(fd,atoi(argv[2])+atoi(argv[3]), SEEK_SET))<0){
		exit(1);
	}

	while(read(fd,buf,BUFFER_SIZE)>0){
	}
	
	if((offset2 = lseek(fd, atoi(argv[2]), SEEK_SET))<0){
		exit(1);
	}
	
	write(fd,buf,strlen(buf));
	if(ftruncate(fd,sizeofFile-atoi(argv[3]))<0){
		exit(1);
	}
	
	close(fd);
	exit(0);

}
	
	
