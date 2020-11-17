#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv){
	int fd1;
	int offset; 	
			
	if((fd1=open(argv[1], O_RDWR))<0){
		fprintf(stderr, "error");
		exit(1);
	}
	
	offset = atoi(argv[2]);
	if(lseek(fd1, offset, SEEK_SET)<0){
		exit(1);
	}
	
	write(fd1, argv[3], strlen(argv[3]));

	close(fd1);	
	exit(0);
}

	




	
