#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#define BUFFER_SIZE 1024
	char buf1[BUFFER_SIZE];
	//char buf2[BUFFER_SIZE];
	
int main(int argc, char **argv){
	
	int fd;
	int offset;
	char c;
	int i=0;
	

	offset = atoi(argv[2]);

 	if((fd=open(argv[1], O_RDWR))<0){
		fprintf(stderr, "error while opening\n");
		exit(1);
	}
	
	if(lseek(fd, offset, 0)<0){
		fprintf(stderr, " error while lseek\n");
		exit(1);
	}

	while(read(fd, &c, 1)>0){
		buf1[i]=c; ++i;
	}
		
	if((offset = lseek(fd, offset, SEEK_SET))<0){
		fprintf(stderr, " error while lseek\n");
		exit(1);
	}

	if(write(fd, argv[3], strlen(argv[3]))!=strlen(argv[3])){
		exit(1);
	}
	
	if(write(fd, buf1, strlen(buf1))!=strlen(buf1)){
		exit(1);
	}

	close(fd);
	exit(0);

	
}
