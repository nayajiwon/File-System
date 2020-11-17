#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#define BUFFER_SIZE 1024

int main(int argc, char **argv){
	int fd1; int fd2;
	int length;
	char c;
	
	if((fd1=open(argv[1], O_RDWR|O_APPEND))<0){
		fprintf(stderr, "error");
		exit(1);
	}

	if((fd2=open(argv[2], O_RDWR))<0){
		fprintf(stderr, "error");	
		exit(1);	
	}
	
	while((read(fd2, &c, 1))>0){ 
		write(fd1, &c, 1);
	}

	close(fd2);
	close(fd1);
	exit(0);
}



	

