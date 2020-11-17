#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
// 필요한 경우 헤더파일을 추가한다
#include "flash.h"
#include "fdevicedriver.c"
FILE *flashfp;	// fdevicedriver.c에서 사용
struct exising{
	int sector_size;
	int spare_size;
};

int check_exist(char *argv);
int freeblock();

int main(int argc, char *argv[])
{	
	char sparebuf[BLOCK_SIZE];
	char sectorbuf[SECTOR_SIZE];
	char pagebuf[PAGE_SIZE];
	char *blockbuf;

	if(strcmp(argv[1],"c"))
	{
		flashfp = fopen(argv[2], "r+");//파일 열고
	}

	if(!strcmp(argv[1], "c"))
	{	
		flashfp = fopen(argv[2], "w+");
		for(int i=0; i<atoi(argv[3]); i++)
			dd_erase(i);

	}

	else if(!strcmp(argv[1], "w"))
	{	
		int length;
		int exist_size; 
		int f_block; 
		int num;

		f_block = check_exist(argv[3]); 		
		
		length = SECTOR_SIZE - strlen(argv[4]);

		memcpy(pagebuf,argv[4],sizeof(char)*strlen(argv[4]));		
		
		memset(pagebuf+strlen(argv[4]), (char)0xFF, length);

		memcpy(pagebuf+SECTOR_SIZE,argv[5],sizeof(char)*strlen(argv[5]));		
	
		length = SPARE_SIZE - strlen(argv[5]);
		
		memset(pagebuf+SECTOR_SIZE+strlen(argv[5]), (char)0xFF, length);
		
		dd_write(atoi(argv[3]), pagebuf);

		//프리블럭의 페이지를 복사 한 뒤에, 프리블럭을 지움 
		if(f_block != -1)
		{
			num = atoi(argv[3])/4; //페이지가 속해있는 블럭의 첫 페이지 
			
			for(int i=0; i<4; i++){
				
				if(i == atoi(argv[3])%4) continue;
				dd_read(f_block*4+i, pagebuf);
				dd_write(num*4+i,pagebuf);				
			
			}

			dd_erase(f_block);
		}	

	}
	else if(!strcmp(argv[1], "r"))
	{
		int flag=0;
		
		dd_read(atoi(argv[3]), pagebuf); 

		for(int i=0; i<SECTOR_SIZE; i++)
		{
			if(pagebuf[i]==(char)0xFF){
				continue;
			}
			
			flag = 1;
			printf("%c", pagebuf[i]);
		}
			
		if(flag==1)
			printf(" ");	
		
		for(int p=SECTOR_SIZE; p<PAGE_SIZE; p++)
		{
			if(pagebuf[p]==(char)0xFF)
				continue;
			
			printf("%c", pagebuf[p]);
			
		}

		
	}
	else if(!strcmp(argv[1], "e"))
	{
		dd_erase(atoi(argv[3]));
	}
	return 0; 
}

int check_exist(char *argv){
	int freebl;
	char temp[BLOCK_SIZE];
	int i;
	int num;

	dd_read(atoi(argv), temp); //argv페이지 확인

	for(i=0; i<PAGE_SIZE; i++)
	{
		if(temp[i] != (char)0xFF)//이미 파일이 존재했을 경우 
			break;	
	}

	if(i==PAGE_SIZE)
		return -1; 
	

	if(i!=PAGE_SIZE)//프리블록에 복사하고 
	{
		num = atoi(argv)/4; //num번 블럭의 페이지를 free block으로 복사 
		
		freebl = freeblock();
		
		for(int i=0; i<4; i++){
			
			if(i==atoi(argv)%4) 
				continue;
			
			dd_read(num*4+i,temp);
			dd_write(freebl*4+i, temp);//page 단위로 보냄 
		}

		dd_erase(num);//block 단위로 erase before write 
	}

	return freebl; 

}


/*프리 블럭을 찾는 함수 */
int freeblock(){
	//bs = blocksize 
	char temp[BLOCK_SIZE];
	int i;
	int flag=0;
	int block_cnt;

	fseek(flashfp, 0, SEEK_END);
	block_cnt = ftell(flashfp)/BLOCK_SIZE;
	
	for(i=0; i<block_cnt; i++)
	{
		fseek(flashfp, BLOCK_SIZE*i, SEEK_SET);
		fread(temp, BLOCK_SIZE, 1, flashfp);
	
		for(int p=0; p<BLOCK_SIZE; p++)
		{
			if(temp[p] != (char)0xFF){
				flag = 0;
				break;
			}

			flag = 1;
		}
		if(flag == 1) 
			break;
	}
	//프리 블록이 없다면 
	if(flag == 0){
		
		printf("You can't overwrite since there are no freeblocks"); 
		return -1;
	
	}
	
	//프리 블록 번호를 리턴 
	return i; 
}


