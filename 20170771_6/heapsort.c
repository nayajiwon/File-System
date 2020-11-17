#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include "person.h"
int size, base, inputsize; 
int islastNull(char *fname, int size, FILE *fp);

void outputfileinit(FILE *outputfp, FILE *inputfp);

void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp, PAGE_SIZE*pagenum, SEEK_SET);
        fread(pagebuf, PAGE_SIZE, 1, fp);
}

void writePage(FILE *fp, const char *pagebuf, int pagenum)
{

	fseek(fp, PAGE_SIZE*pagenum, SEEK_SET);
        fwrite(pagebuf, PAGE_SIZE, 1, fp);
}

void buildHeap(FILE *inputfp, char **heaparray)
{

	char sn[1024][1024], temp[1024], tempheap[1024], pagebuf[PAGE_SIZE];
	int ind, i=0; 
	int t, c; 
	int tot =0;
	char *ptr1, *ptr2, ptr3; 

	int go = 0; 

	for(i=0; i<1024; i++)
		memset(sn[i], 0, 1024); 

	for(t=1; t<=size/base+1; t++)
	{
		readPage(inputfp, pagebuf, t); 

		for(c=0; c<base; c++)
		{
				
			if(!isdigit(pagebuf[RECORD_SIZE *c]))
				continue;
				
			ptr3 = pagebuf[RECORD_SIZE * (c+1)];
			ptr1 = pagebuf + RECORD_SIZE * (c+1);
			*ptr1 = '\0';

			ptr2 = pagebuf+RECORD_SIZE*c;

			strcpy(heaparray[tot], ptr2);

			
			pagebuf[RECORD_SIZE * (c+1)] = ptr3;
			
	
			ind = 0; 
			for(i=0; i<size; i++)
			{
				if(!isdigit(heaparray[i][0]))
				{
					break; 
				}

				ptr1 = heaparray[i]; 
				while(*ptr1 != '#'){
					ptr1++; 
				}

				*ptr2 = *ptr1; 
				*ptr1 = '\0';
			

				strcpy(sn[i], heaparray[i]); 
				*ptr1 = *ptr2; 
				++ind; 
		

			}

			inputsize = ind; 

			for(i=0; i<ind; i++){
				int child = i;
	
				do{
					int root = (child-1)/2;

					if(strcmp(sn[child], sn[root])<0){
			
						strcpy(temp, sn[child]);
						strcpy(tempheap, heaparray[child]);

						strcpy(sn[child], sn[root]); 
						strcpy(heaparray[child], heaparray[root]);
		
						strcpy(sn[root], temp); 
						strcpy(heaparray[root], tempheap);

					}
					child = root;
				
				}while(child!=0);	//최상단 노드까지 점검

			}
			tot++; 
			
		}
	}

/*
	printf("*********~~~~~\n");
	for(int g=0; g<size; g++)
		printf("%s\n", heaparray[g]);
*/

}


//void heapmaking(char **heaparray, char sn[1024][1024], int ind){
void heapmaking(char **heaparray, int index){


	char temp[1024], tempheap[1024]; 
	int i; 
	char *ptr1, ptr2; 
	char sn[1024][1024], pagebuf[PAGE_SIZE];

	for(i=0; i<size; i++)
	{
		if(!isdigit(heaparray[i][0]))
		{
			break; 
		}

		ptr1 = heaparray[i]; 
		
		while(*ptr1 != '#'){
			ptr1++; 
		}
		ptr2 = *ptr1; 
		*ptr1 = '\0';
		strcpy(sn[i], heaparray[i]); 
		*ptr1 = ptr2; 
	}



	for(i=0; i<index; i++){
	
		int child = i;
		do{
			int root = (child-1)/2;
			
			if(strcmp(sn[child], sn[root])<0){
				strcpy(temp, sn[child]);
				strcpy(tempheap, heaparray[child]);
				strcpy(sn[child], sn[root]); 
				strcpy(heaparray[child], heaparray[root]);
				strcpy(sn[root], temp); 
				strcpy(heaparray[root], tempheap);
			}
			child = root;
		}while(child!=0);	//최상단 노드까지 점검
	}


}
void makeSortedFile(FILE *outputfp, char **heaparray)
{

	char sn[1024][1024], pagebuf[PAGE_SIZE];
	char temp[1024], buf[1024]; 
	char *ptr1, ptr2; 
	int i,d,ind =0;
	int indsize; 

	int flag = 1; 


	indsize = inputsize; 
	int st = 0; 
	for(i = 0; i<inputsize/base+1; i++)
	{

		for(d = 0; d<base; d++)
		{
	
			if(indsize == 0){
				break;
			}

			heapmaking(heaparray, indsize);
			strcpy(temp, heaparray[0]);

			strcpy(pagebuf + RECORD_SIZE * d, heaparray[0]);
			
			strcpy(heaparray[0], heaparray[indsize-1]);
			strcpy(heaparray[indsize-1], temp);
			--indsize;
			
		}

		if(!isdigit(pagebuf[0]))
			break;

	
		
		for(int h = PAGE_SIZE-1; h>=0; h--)
		{
			if(pagebuf[h] != (char)0xff)
				pagebuf[h] = (char)0xff; 
			else 
				break;
		}

		pagebuf[PAGE_SIZE] = '\0'; 

		writePage(outputfp, pagebuf, i);


		memset(pagebuf, 0, PAGE_SIZE);
		
	}
}	



int islastNull(char *fname, int size, FILE *fp){
	char pagebuf[PAGE_SIZE]; 
	struct stat statbuf; 
	int base, go; 

	base = PAGE_SIZE/RECORD_SIZE; 

	stat(fname, &statbuf);
//	printf("사이즈: %ld\n", statbuf.st_size);
	
	size = statbuf.st_size;
	go = size; 

	size -= PAGE_SIZE; 
	size /= PAGE_SIZE; 

	go -= PAGE_SIZE; 
	int t = 0;  
	for(int i=0; i<PAGE_SIZE; i++)
	{
		if(!isdigit(pagebuf[go+t*RECORD_SIZE]))
		{
			break; 
		}
		else 
			t++; 

	}


	return size*base;
}

void outputfileinit(FILE *outputfp, FILE *inputfp){

	int f, s, t, l; 
	char buf[PAGE_SIZE - 4*4]; 
	char pagebuf[PAGE_SIZE];

	readPage(inputfp, pagebuf, 0);

        memcpy(&f, pagebuf,sizeof(int));
        memcpy(&s, pagebuf+sizeof(int),  sizeof(int));
        memcpy(&t, pagebuf+sizeof(int)*2,  sizeof(int));
        memcpy(&l, pagebuf+sizeof(int)*3,  sizeof(int));
        memcpy(buf, pagebuf+sizeof(int)*4,  PAGE_SIZE - sizeof(int)*4 );


	memset(pagebuf, 0, PAGE_SIZE);
	
        memcpy(pagebuf,&f, sizeof(int));
        memcpy(pagebuf+sizeof(int), &s,  sizeof(int));
       
	memcpy(pagebuf+sizeof(int)*2, &t, sizeof(int));
        memcpy(pagebuf+sizeof(int)*3, &l, sizeof(int));
	memcpy(pagebuf + sizeof(int)*4, buf, PAGE_SIZE - sizeof(int) * 4); 


	writePage(outputfp, pagebuf, 0);
/*	
	readPage(outputfp, pagebuf, 0);
        memcpy(&f, pagebuf,sizeof(int));
        memcpy(&s, pagebuf+sizeof(int),  sizeof(int));
        memcpy(&t, pagebuf+sizeof(int)*2,  sizeof(int));
        memcpy(&l, pagebuf+sizeof(int)*3,  sizeof(int));
        memcpy(buf, pagebuf+sizeof(int)*4,  PAGE_SIZE - sizeof(int)*4 );

	//strcat(buf, "\0");
	printf("-- %ld\n", PAGE_SIZE - sizeof(int) * 4);

	printf("\n	1. %d\n", f);
	printf("	2. %d\n", s);
	printf("	3. %d\n", t);
	printf("	4. %d\n", l);
*/
}

int main(int argc, char *argv[])
{
	FILE *inputfp;	
	FILE *outputfp;	
	int i,p, ind;  
	char fname[1048]; 
	char pagebuf[PAGE_SIZE];
	char **arr;
	char *ptr1, *ptr2; 
	

	base = PAGE_SIZE/RECORD_SIZE; 


	strcpy(fname, argv[2]);

	if((inputfp = fopen(fname, "r+")) == NULL){
		fprintf(stderr,"file open error\n");
		exit(1);
	}

	size = islastNull(fname, size, inputfp);

	arr = (char**)malloc(sizeof(char*) * size);

	for(i=0; i<size; i++)
	{
		arr[i] = (char*)malloc(sizeof(char) * (RECORD_SIZE+2)); 
		memset(arr[i], 0, RECORD_SIZE+1); 
	}
	
	buildHeap(inputfp, arr);

	strcpy(fname, argv[3]);
	
	if(access(fname, F_OK)==0){
		remove(fname);	
	}

	outputfp = fopen(fname, "a+"); 

	
	outputfileinit(outputfp, inputfp);
	
	makeSortedFile(outputfp, arr);
	
	fclose(inputfp);	
	fclose(outputfp);	

	
	return 1;
}





















