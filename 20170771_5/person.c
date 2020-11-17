#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "person.h"

int rcnt; 
int flag; 
int total;

void readPage(FILE *fp, char *pagebuf, int pagenum);
void writePage(FILE *fp, const char *pagebuf, int pagenum);
void pack(char *recordbuf, const Person *p);
void unpack(const char *recordbuf, Person *p);
void insert(FILE *fp, const Person *p);
void delete(FILE *fp, const char *sn);
int strtokRecordbuf(char *pagebuf, int num, char recordbuf[RECORD_SIZE]);
void makeHeader(FILE *fp, char *pagebuf);
void makeheaderInit(FILE *fp);


void readPage(FILE *fp, char *pagebuf, int pagenum)
{
  	int ret;

        fseek(fp, PAGE_SIZE*pagenum, SEEK_SET);
        ret = fread(pagebuf, PAGE_SIZE, 1, fp);

}

void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	int ret;
        fseek(fp, PAGE_SIZE*pagenum, SEEK_SET);
        ret = fwrite(pagebuf, PAGE_SIZE, 1, fp);

}


void pack(char *recordbuf, const Person *p)
{
  	 int len;

        strcpy(recordbuf, p->sn);

        strcat(recordbuf, "#");
        strcat(recordbuf, p->name);
        strcat(recordbuf, "#");
        strcat(recordbuf, p->age);
        strcat(recordbuf, "#");
        strcat(recordbuf, p->addr);
        strcat(recordbuf, "#");
        strcat(recordbuf, p->phone);
        strcat(recordbuf, "#");
        strcat(recordbuf, p->email);
        strcat(recordbuf, "#");


        len = strlen(recordbuf);

        memset(recordbuf+len, (char)0xff, RECORD_SIZE - len);
        recordbuf[RECORD_SIZE] = '\0';



}

void unpack(const char *recordbuf, Person *p)
{
	int i=0;
        char temp[RECORD_SIZE];
        strcpy(temp, recordbuf);

        char *ptr = strtok(temp, "#");

        while(ptr!=NULL && i<6){
    
                if(i==0)
                        strcpy(p->sn, ptr);
                else if(i==1)
                        strcpy(p->name, ptr);
    
                else if(i==2)
                        strcpy(p->age, ptr);
    
                else if(i==3)
                        strcpy(p->addr, ptr);
                else if(i==4)
                        strcpy(p->phone, ptr);
                else if(i==5)
                        strcpy(p->email, ptr);
    
                ptr = strtok(NULL, "#");
                ++i;
        }

}
void insert(FILE *fp, const Person *p)
{		
	int d;
	int gg; 
	int hh;
	int vv;

	int ggg;

	int head1;
	int head2;
	int head3;
	int head4;
        

	int prevpgn;
	int prevrcn;


	int deletepgn; 
	int deletercn; 
	char recordbuf[RECORD_SIZE];
        char pagebuf[PAGE_SIZE];
        char headerbuf[PAGE_SIZE];
        char header[PAGE_SIZE];

	pack(recordbuf, p);
	strcat(recordbuf, "\0");

 	readPage(fp, pagebuf, 0); 

	memcpy(&d, pagebuf, sizeof(int));
        memcpy(&gg, pagebuf+sizeof(int),  sizeof(int));
        memcpy(&hh, pagebuf+sizeof(int)*2,  sizeof(int));
        memcpy(&vv, pagebuf+sizeof(int)*3,  sizeof(int));

 	
	readPage(fp, header, 0); 
	memcpy(&head1, header, sizeof(int));
        memcpy(&head2, header+sizeof(int),  sizeof(int));
        memcpy(&head3, header+sizeof(int)*2,  sizeof(int));
        memcpy(&head4, header+sizeof(int)*3,  sizeof(int));


	if(hh == -1 && vv == -1){
		if(flag == 1){ 
 			readPage(fp, pagebuf, 1); 
 			memcpy(pagebuf, recordbuf, RECORD_SIZE); 
 			writePage(fp, pagebuf, 1);

			head1 = head1+1;
        		memcpy(header, &head1, sizeof(int));

			ggg = head2+1; 
        		memcpy(header+sizeof(int), &ggg, sizeof(int));
 			writePage(fp, header, 0); 

		}
		else{

			ggg = head2+1; 
        		memcpy(header+sizeof(int), &ggg, sizeof(int));
			
			if(rcnt == total) {
				
 				readPage(fp, pagebuf, d);  
 				memcpy(pagebuf, recordbuf, RECORD_SIZE); 
				writePage(fp, pagebuf, d);
				head1 = head1+1;
        			memcpy(header, &head1, sizeof(int));

			}
			else if(rcnt < total){
 				readPage(fp, pagebuf, d-1); 
 				memcpy(pagebuf+RECORD_SIZE*rcnt, recordbuf, RECORD_SIZE); 
				writePage(fp, pagebuf, d-1);
			}
 			
			writePage(fp, header, 0); 


		}
	
	}
	else {

		readPage(fp, pagebuf, head3);
		
		memcpy(&prevpgn, pagebuf+100*head4+sizeof(char), sizeof(int));
       	 	memcpy(&prevrcn, pagebuf+100*head4+sizeof(char)+sizeof(int),  sizeof(int));

		memcpy(header+sizeof(int)*2, &prevpgn, sizeof(int));
		memcpy(header+sizeof(int)*3, &prevrcn, sizeof(int));

		writePage(fp, header, 0); 

		memcpy(pagebuf+100*head4, recordbuf, RECORD_SIZE);
		
		writePage(fp, pagebuf, head3);

		


	}


 	readPage(fp, pagebuf, 0); 

	memcpy(&d, pagebuf, sizeof(int));
        memcpy(&gg, pagebuf+sizeof(int),  sizeof(int));
        memcpy(&hh, pagebuf+sizeof(int)*2,  sizeof(int));
        memcpy(&vv, pagebuf+sizeof(int)*3,  sizeof(int));


	//printf("==== %d %d %d %d\n", d, gg, hh, vv);

}

void delete(FILE *fp, const char *sn)
{
	int deletepgn;
	int deleterec;

	int d;
	int gg; 
	int hh;
	int vv;

	int aaa;
	int bbb; 
	char ccc; 

	char g1;
	int g2;
	int g3;
	 

	int ggg;
       int fflag = 0; 

	char recordbuf[RECORD_SIZE];
        char pagebuf[PAGE_SIZE];
        char headerbuf[PAGE_SIZE];
	Person p; 

 	readPage(fp, headerbuf, 0); 

	memcpy(&d, headerbuf, sizeof(int));
        memcpy(&gg, headerbuf+sizeof(int),  sizeof(int));
        memcpy(&hh, headerbuf+sizeof(int)*2,  sizeof(int));
        memcpy(&vv, headerbuf+sizeof(int)*3,  sizeof(int));

	//printf("first header : %d %d %d %d\n", d, gg, hh, vv);
	
	for(int i=1; i<d; i++){
 		readPage(fp, pagebuf, i); 

		for(int t = 0; t<total; t++){
	                if(strtokRecordbuf(pagebuf, t, recordbuf)>0)
				unpack(recordbuf, &p);
			else 
				continue; 
		
			if(!strcmp(p.sn , sn)){
                        	deletepgn = i;
				deleterec = t;
				fflag = 1;
				break;
                	}

		}
		if(fflag == 1)
			break;
		
	}

	ccc = '*'; 

	memcpy(pagebuf+100*deleterec, &ccc, sizeof(char));
	memcpy(pagebuf+100*deleterec+sizeof(char), &hh, sizeof(int));
	memcpy(pagebuf+100*deleterec+sizeof(char)+sizeof(int), &vv, sizeof(int));
	writePage(fp, pagebuf, deletepgn); 

	readPage(fp, pagebuf, deletepgn);
	memcpy(&g1, pagebuf+100*deleterec, sizeof(char));
        memcpy(&g2, pagebuf+100*deleterec+sizeof(char),  sizeof(int));
        memcpy(&g3, pagebuf+100*deleterec+sizeof(char)+sizeof(int),  sizeof(int));

	//printf("in buf --> %c %d %d \n", g1, g2, g3);

	hh = deletepgn;
	vv = deleterec; 

        memcpy(headerbuf+sizeof(int)*2, &hh, sizeof(int));
        memcpy(headerbuf+sizeof(int)*3, &vv, sizeof(int));


	//printf("in header %d %d %d %d\n", d, gg, hh, vv);

	
	writePage(fp, headerbuf, 0); 
	
}


int strtokRecordbuf(char *pagebuf, int num, char recordbuf[RECORD_SIZE]){
     
	char go; 

	memcpy(&go, pagebuf+num*100, sizeof(char));

	if(go == '*')
		return -1; 
	
	memcpy(recordbuf, pagebuf+100*num, RECORD_SIZE);

	return 1;
}




void makeHeader(FILE *fp, char *pagebuf){

	flag = 1; 

        for(int i=0; i<PAGE_SIZE; i++)
                pagebuf[i] = (char)0xff;

        int a = 1;
        int b = 0;
        int c = -1;



        memcpy(pagebuf, &a, sizeof(int));
        memcpy(pagebuf+sizeof(int), &b, sizeof(int));
        memcpy(pagebuf+sizeof(int)*2, &c, sizeof(int));
        memcpy(pagebuf+sizeof(int)*3, &c, sizeof(int));


        writePage(fp, pagebuf, 0);

}

void makeheaderInit(FILE *fp){
	char pagebuf[PAGE_SIZE];
	char headerbuf[PAGE_SIZE];
	int cnt;
	int size; 	
	int starr = 0;
	int test;
	int secondhead;
	int firstpgn =0;
	int firstrcn = 0;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	cnt = size / PAGE_SIZE;
	
	readPage(fp, headerbuf, 0);
	readPage(fp, pagebuf, cnt-1);

	char *ptr = strtok(pagebuf, "#");
	while(ptr!=NULL){
		++rcnt; 
		ptr = strtok(NULL, "#");
	
	}

	--rcnt; 
	rcnt = rcnt / 6;  

	
	readPage(fp, pagebuf, 0);
	int d;
	int gg; 
	int hh;
	int vv;
        memcpy(&d, pagebuf, sizeof(int));
        memcpy(&gg, pagebuf+sizeof(int),  sizeof(int));
        memcpy(&hh, pagebuf+sizeof(int)*2,  sizeof(int));
        memcpy(&vv, pagebuf+sizeof(int)*3,  sizeof(int));


	//printf("in init : %d %d %d %d\n", d, gg, hh, vv);

}


int main(int argc, char *argv[])
{
	FILE *fp;
        char *fname; 
        char *op;
        char pagebuf[PAGE_SIZE];
        Person P;
    
        fname = argv[2];
        op = argv[1];

	total = PAGE_SIZE / RECORD_SIZE;

	if(access(fname, F_OK)<0){
                fp = fopen(fname, "w+");
                readPage(fp, pagebuf, 0); 
                makeHeader(fp, pagebuf);
		memset(pagebuf, 0, PAGE_SIZE);
	}
	else {
                if((fp = fopen("person.dat", "r+")) == NULL){
                        fprintf(stderr,"file open error\n");
                        exit(1);
                }

		makeheaderInit(fp); 
        }


	 if(!strcmp(op, "i"))
        {

                strcpy(P.sn, argv[3]);
                strcpy(P.name, argv[4]);
                strcpy(P.age, argv[5]);
                strcpy(P.addr, argv[6]);
                strcpy(P.phone, argv[7]);
                strcpy(P.email, argv[8]);

                readPage(fp, pagebuf, 0); 
		
	int d;
	int gg; 
	int hh;
	int vv;
        memcpy(&d, pagebuf, sizeof(int));
        memcpy(&gg, pagebuf+sizeof(int),  sizeof(int));
        memcpy(&hh, pagebuf+sizeof(int)*2,  sizeof(int));
        memcpy(&vv, pagebuf+sizeof(int)*3,  sizeof(int));


	//printf("111 	%d %d %d %d\n", d, gg, hh, vv);

                insert(fp, &P);
        }
        else if(!strcmp(op, "d"))
        {

                readPage(fp, pagebuf, 0); 
		
	int d;
	int gg; 
	int hh;
	int vv;
        memcpy(&d, pagebuf, sizeof(int));
        memcpy(&gg, pagebuf+sizeof(int),  sizeof(int));
        memcpy(&hh, pagebuf+sizeof(int)*2,  sizeof(int));
        memcpy(&vv, pagebuf+sizeof(int)*3,  sizeof(int));


	//printf("111	%d %d %d %d\n", d, gg, hh, vv);


                delete(fp, argv[3]);
        }

        
	return 1;



}
