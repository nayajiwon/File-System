#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
int main(int argc, char *argv[]){
	int fd;
	char c;
	int set;	
	int byte; 
	int num=0;

	set = atoi(argv[2]);
	byte = atoi(argv[3]);
	
	if((fd=open(argv[1], O_RDWR))<0){
		fprintf(stderr, "open error\n");
		exit(1);
	}

	
	if(lseek(fd, set, SEEK_SET)<0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	while(read(fd,&c,1)>0){	
		write(1,&c,1);			
		++num; 
		if(num>=byte) break;	
	}

	close(fd);
	exit(0);
}


