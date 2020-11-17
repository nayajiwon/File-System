#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SUFFLE_NUM 10000//값은 마음대로 수정할 수 있음 

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);

int main(int argc, char **argv)
{	
	int fd;

	int *read_order_list;

	int num_of_records;

	struct stat statbuf;

	char buf[SUFFLE_NUM];

	int rand_offset;

	struct timeval start, end;

	long time;

	if((fd = open(argv[1], O_RDWR))<0){
		fprintf(stderr,"open error\n");
	}

	if(stat(argv[1], &statbuf)<0){
		fprintf(stderr,"stat error\n");
	}
	
	num_of_records = statbuf.st_size / 100;

	read_order_list = (int*)malloc(sizeof(int) * num_of_records);
	
	gettimeofday(&start, NULL);

	GenRecordSequence(read_order_list, num_of_records);
	
	for(int i=0; i<num_of_records; i++){
		rand_offset = read_order_list[i] * 100;
	
		if(lseek(fd, rand_offset , SEEK_SET)<0){
			printf("lseek failed\n");
		}
	
		read(fd,buf,100);

	}

	gettimeofday(&end, NULL);
	
	//1sec = 1000000usec
	time = (end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec; 
	
	printf("#records : %d timecost : %ld us", num_of_records, time);
	
	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}
	
	
	for(i=0; i<n; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}
