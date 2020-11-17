// 주의사항
// 1. sectormap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. sectormap.h에 정의되어 있지 않을 경우 본인이 이 파일에서 만들어서 사용하면 됨
// 3. 필요한 data structure가 필요하면 이 파일에서 정의해서 쓰기 바람(sectormap.h에 추가하면 안됨)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "sectormap.h"
// 필요한 경우 헤더 파일을 추가하시오.


typedef struct _G_bl
{
        int B_pbn; //몇번째 블럭의 가비지블럭인가  
        char P_offs[PAGE_SIZE];
        struct _G_bl *next;
}G_bl;
void dd_read();
void dd_write();
void dd_erase();

void makeGarbage_block(int block_num, int page_offset);
void addPsn_inBlock(int blockNum, int pageNum);
int isBlockExist(int blockNum);
void garbageCollection();
void removeN(int key);
int isfull();

void ftl_open();
void ftl_read(int lsn, char *sectorbuf);
void ftl_write(int lsn, char *sectorbuf);
void ftl_print();

int OFFS;
int Free_pbn; 
int amTable[DATAPAGES_PER_DEVICE];
int empty[BLOCKS_PER_DEVICE*PAGES_PER_BLOCK];
SpareData spareInfo[BLOCKS_PER_DEVICE * PAGES_PER_BLOCK];

G_bl *List;

//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.
//
void ftl_open()
{
	for(int i=0; i<DATAPAGES_PER_DEVICE; i++)
                amTable[i] = -1; 
	
	Free_pbn = BLOCKS_PER_DEVICE-1; 
	OFFS = 0; 
	
	List = (G_bl*)malloc(sizeof(G_bl));
        List->B_pbn = -1;
        List->next = NULL;

        OFFS = 0;


	//
	// address mapping table 초기화
	// free block's pbn 초기화
    	// address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일
	
	return;
}

//
// 이 함수를 호출하기 전에 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 한다.
// 즉, 이 함수에서 메모리를 할당받으면 안된다.
//
void ftl_read(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	char sec[SECTOR_SIZE];
	int p;
        int i = 0;	
	
	p = amTable[lsn];
	
	if(p == -1){
		printf("읽을 페이지의 내용이  없습니다.\n");
		return; 
	}
	
	dd_read(p, pagebuf);
	
	for(i=0; i<SECTOR_SIZE; i++)
		sec[i] = pagebuf[i];
	
	sec[i] = '\0';
	
	strcpy(sectorbuf, sec);

	return;
}


void ftl_write(int lsn, char *sectorbuf)
{
	char sparebuf[SPARE_SIZE];
	char pagebuf[PAGE_SIZE];
	char ItoC[10];
	int sec_len;
	int lsn_len;
	int check_pbn;
	char temp[SPARE_SIZE];	
	int blockNum;
	
	memset(pagebuf, 0, PAGE_SIZE);//null로 초기화 
	memset(sparebuf, 0, SPARE_SIZE);
	

	sprintf(ItoC, "%d", lsn); //char 형으로 변환 
	lsn_len = strlen(ItoC);

	memcpy(pagebuf, sectorbuf, SECTOR_SIZE);

	memcpy(sparebuf,ItoC, lsn_len);
	memset(temp, 'u', SPARE_SIZE-lsn_len);
	temp[SPARE_SIZE - lsn_len] = '\0';

	strcat(sparebuf, temp);
	


	strcat(pagebuf, sparebuf);

	if(OFFS >= BLOCKS_PER_DEVICE*PAGES_PER_BLOCK){
		OFFS = 0; 
	}
	
	
	if(isfull()){
		garbageCollection();
	}
	
	
	//while로 빈 페이지를 찾을 때 까지 돌려줘야함 
//	while(empty[OFFS] == 1){
//		++OFFS;
//	}

	if(amTable[lsn] == -1){
		dd_write(OFFS, pagebuf);
	}
	else if(amTable[lsn] >= 0){
		
		blockNum = amTable[lsn]/4;

		if(!isBlockExist(blockNum)){
                        makeGarbage_block(blockNum, amTable[lsn]%4);
                }
		else{

                        addPsn_inBlock(blockNum, amTable[lsn]%4);

                        G_bl *cur = List;
                        while(cur != NULL){
                                //if(cur->B_pbn == blockNum)
				cur = cur->next;
                        }
			
                }
		spareInfo[OFFS].is_invalid = 1;


		dd_write(OFFS, pagebuf);
	}
	amTable[lsn] = OFFS;
	spareInfo[OFFS].lpn = lsn; 
	empty[OFFS] = 1; 
	++OFFS;


	return;
}

void ftl_print()
{
        printf("lpn ppn\n");
        for(int i=0; i<DATAPAGES_PER_DEVICE; i++){
                printf("%d %d\n", i, amTable[i]);
        }

        printf("free block's pbn=%d\n", Free_pbn);
        return;
}

int isfull(){
        int cnt = 0;
        for(int i=0; i< BLOCKS_PER_DEVICE * PAGES_PER_BLOCK; i++){
                if(empty[i] == 1){//이미 사용했다면 
                        ++cnt;
                }
        }

	if(cnt == DATAPAGES_PER_DEVICE)
		return 1;

        return 0;
}



int isBlockExist(int blockNum){
        G_bl* cur = List;
        while(cur != NULL){
                if(cur->B_pbn ==blockNum){
                        return 1;
                }
                cur = cur->next;
        }

        return 0;
}


void makeGarbage_block(int block_num, int page_offset ){
        char psnC[10];
        sprintf(psnC,"%d",page_offset);

        G_bl *garbageBl;
        garbageBl = (G_bl*)malloc(sizeof(G_bl));

        garbageBl->B_pbn = block_num;
        strcat(garbageBl->P_offs, psnC);
        strcat(garbageBl->P_offs, ",");

        garbageBl->next = NULL;

        if(List == NULL)
                List = garbageBl;
        else{
                garbageBl->next = List;
                List = garbageBl;
        }

        return;
}

void addPsn_inBlock(int blockNum, int pageNum){
        char pbnC[100];

        sprintf(pbnC, "%d", pageNum);
	
	G_bl* cur = List;

        if(cur == NULL)
                return;

        while(cur!=NULL){
                if(cur->B_pbn == blockNum){
                        strcat(cur->P_offs, pbnC);
                        strcat(cur->P_offs,",");
                }
                cur = cur->next;
        }

        return;

}

void garbageCollection(){
	char pagebuf[PAGE_SIZE];
	char *ptr;
        int garbagePage[PAGES_PER_BLOCK] = {1, 1, 1, 1};
        int cnt = 0;
	int gc;

        if(List == NULL)
                return;
 	
    	G_bl *cur = List;
	G_bl *after; 

	 
	while(cur->next != NULL){
         
                                 
		if(cur->next->B_pbn == -1){

			break;

		}

		cur = cur->next;
	}

	after = List;


	ptr = strtok(cur->P_offs, ",");
	gc = cur->B_pbn;

        while(ptr != NULL){
                for(int i=0; i<PAGES_PER_BLOCK; i++){
                        if(atoi(ptr) == i)
                                garbagePage[i] = -1;
                }
                ptr = strtok(NULL, ",");
        }

       	for(int i=0; i<PAGES_PER_BLOCK; i++){      
		memset(pagebuf, 0, PAGE_SIZE); //맞을까.. ?
		if(garbagePage[i] > 0){//쓸모있는 페이지라는 소리
			dd_read(cur->B_pbn*4+i, pagebuf);
			dd_write(Free_pbn*4+i, pagebuf);
			
			//초기화.
			amTable[spareInfo[cur->B_pbn*4+i].lpn] = Free_pbn*4+i;
			empty[Free_pbn*4+i] = 1;
		}		
			
	}

	 dd_erase(cur->B_pbn);

	 for(int i=0; i<4; i++){
			spareInfo[cur->B_pbn*4+i].is_invalid = 0; 
	 		empty[cur->B_pbn*4+i] = 0;
	 }

	//free

	 removeN(cur->B_pbn);
	 Free_pbn = gc;

	 //free하는 함수 만들어야 

}
/*
void traverse(){
        //printf("traverse\n");
        G_bl* cur = List;
        while(cur != NULL){
                cur = cur->next;
        }
        //printf("\n");
}
*/
void removeN(int key){
	
	if (List == NULL) {
		return;
	}

	if (List->B_pbn == key) {
		G_bl * cur = List;
		List = List->next;
		free(cur);
		return;
	}
	else {
		G_bl * cur = List->next;
		G_bl * prev = List;
		while (cur != NULL && cur->B_pbn != key) {
			prev = cur;
			cur = cur->next;
		}

		if (cur == NULL) return;

		prev->next = cur->next;
		free(cur);
		return;
	}
}
