#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
	int fd1; 
	int fd2;
	ssize_t nread;
	char buf[BUFFER_SIZE];

	if((fd1 = open(argv[1],O_RDWR))<0) {
		fprintf(stderr, "fd1 open failed");
		exit(1);	
	}

	if((fd2 = creat(argv[2],0644))<0){	
		fprintf(stderr,"fd2 open failed");
	 	exit(1);
	}

	while((nread = read(fd1,buf,100))>0){
		if(write(fd2, buf, nread)<nread){
			exit(1);
		}				
	}
	
	close(fd1);
	close(fd2);
        exit(0);
}

