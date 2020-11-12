#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "person.h"

void readPage(FILE *fp, char *pagebuf, int pagenum) {
    fseek(fp, PAGE_SIZE * pagenum, SEEK_SET);
    fread(pagebuf, sizeof(char), PAGE_SIZE, fp);

}

void writePage(FILE *fp, const char *pagebuf, int pagenum) {
    fseek(fp, PAGE_SIZE * pagenum, SEEK_SET);
    fwrite(pagebuf, sizeof(char), PAGE_SIZE, fp);
}

void pack(char *recordbuf, const Person *p) {
    sprintf(recordbuf, "%s#%s#%s#%s#%s#%s#", p -> sn, p -> name, p -> age, p -> addr, p -> phone, p -> email);
    memset(recordbuf + strlen(recordbuf), (char)0xFF, RECORD_SIZE - strlen(recordbuf)); 
}

void unpack(const char *recordbuf, Person *p) {
    sscanf(recordbuf, "%[^'#']#%[^'#']#%[^'#']#%[^'#']#%[^'#']#%[^'#']#",
            p -> sn, p -> name, p -> age, p -> addr, p -> phone, p -> email);
}

void setHeadPage(char *pagebuf, int pageNum, int recordNum, int deletePageNum, int deleteRecordNum) {
    memset(pagebuf, 0xFF, PAGE_SIZE);
    memcpy(pagebuf, &pageNum, sizeof(int));
    memcpy(pagebuf+4, &recordNum, sizeof(int));
    memcpy(pagebuf+8, &deletePageNum, sizeof(int));
    memcpy(pagebuf+12, &deleteRecordNum, sizeof(int));
}    

void insert(FILE *fp, const Person *p) {

    char pagebuf[PAGE_SIZE + 1] = { };

    int pageNum = 1;
    int recordNum = 0;
    int deletePageNum = -1;
    int deleteRecordNum = -1;

    readPage(fp, pagebuf, 0);

    if(strlen(pagebuf) == 0) {
        setHeadPage(pagebuf, pageNum, recordNum, deletePageNum, deleteRecordNum);
        writePage(fp, pagebuf, 0);
    }
    else {
        memcpy(&pageNum, pagebuf, sizeof(int));
        memcpy(&recordNum, pagebuf+4, sizeof(int));
        memcpy(&deletePageNum, pagebuf+8, sizeof(int));
        memcpy(&deleteRecordNum, pagebuf+12, sizeof(int));
    }

    char *recordbuf = malloc(sizeof(char)*100);
    pack(recordbuf, p);

    int RecordPerPage = PAGE_SIZE / RECORD_SIZE;
 
    if(deletePageNum == -1) {

        int maxRecordNum = pageNum * RecordPerPage - RecordPerPage;

        if(maxRecordNum == recordNum) {
            pageNum++;
            recordNum++;
            setHeadPage(pagebuf, pageNum, recordNum, deletePageNum, deleteRecordNum);
            writePage(fp, pagebuf, 0);

            memset(pagebuf, (char)0xFF, PAGE_SIZE);
            memcpy(pagebuf, recordbuf, RECORD_SIZE);

            writePage(fp, pagebuf, pageNum-1);
        }
        else {
            int index = recordNum % RecordPerPage;
            index = index * RECORD_SIZE;
            
            memset(pagebuf, (char)0xFF, PAGE_SIZE);
            readPage(fp, pagebuf, pageNum-1);

            memcpy(pagebuf + index, recordbuf, RECORD_SIZE);
            writePage(fp, pagebuf, pageNum-1);

            recordNum++;
            setHeadPage(pagebuf, pageNum, recordNum, deletePageNum, deleteRecordNum);
            writePage(fp, pagebuf, 0);
        }
 
    }
    else {
        int index = deleteRecordNum % RecordPerPage;
        int nextDeletePage, nextDeleteRecord;
        index = index * RECORD_SIZE;

        memset(pagebuf, (char)0xFF, PAGE_SIZE);
        readPage(fp, pagebuf, deletePageNum);

        memcpy(&nextDeletePage, pagebuf + index + 1, sizeof(int));
        memcpy(&nextDeleteRecord, pagebuf + index + 5, sizeof(int));
        memcpy(pagebuf + index, recordbuf, RECORD_SIZE);
        writePage(fp, pagebuf, deletePageNum);

        recordNum++;
        setHeadPage(pagebuf, pageNum, recordNum, nextDeletePage, nextDeleteRecord);
        writePage(fp, pagebuf, 0);
    }
}

void delete(FILE *fp, const char *sn) {

    char pagebuf[PAGE_SIZE+1] = { };
 
    int pageNum, recordNum, deletePageNum, deleteRecordNum ;
 
    readPage(fp, pagebuf, 0);
    memcpy(&pageNum, pagebuf, sizeof(int));
    memcpy(&recordNum, pagebuf+4, sizeof(int));
    memcpy(&deletePageNum, pagebuf+8, sizeof(int));
    memcpy(&deleteRecordNum, pagebuf+12, sizeof(int));
 
    Person *person = (Person*)malloc(sizeof(Person));
 
    for( int i = 1; i < pageNum; i++) {
 
        memset(pagebuf, (char)0xFF, PAGE_SIZE);
        readPage(fp, pagebuf, i);
 
        for (int j = 0; j < PAGE_SIZE / RECORD_SIZE; j++) {
 
            int index = j * RECORD_SIZE;
            char tempRecord[RECORD_SIZE + 1] = { } ;
            memcpy(tempRecord, pagebuf + index, RECORD_SIZE);
            unpack(tempRecord, person);
 
            if(!strcmp(person -> sn, sn)) {
 
                memcpy(pagebuf + index, "*", 1);
                memcpy(pagebuf + index + 1, &deletePageNum, sizeof(int));
                memcpy(pagebuf + index + 5, &deleteRecordNum, sizeof(int));
                writePage(fp, pagebuf, i);
 
                deletePageNum = i;
                deleteRecordNum = j;
                recordNum--;

                setHeadPage(pagebuf, pageNum, recordNum, deletePageNum, deleteRecordNum);
                writePage(fp, pagebuf, 0);
 
                exit(1);
            }
        }
    }
}

int main(int argc, char *argv[])
{
	FILE *fp;  // 레코드 파일의 파일 포인터

    if((fp = fopen(argv[2], "r+")) == NULL)
        fp = fopen(argv[2], "w+");

    Person *person = (Person*)malloc(sizeof(Person));
    
    if(!strcmp(argv[1], "i")) {

        strcpy(person -> sn, argv[3]);
        strcpy(person -> name, argv[4]);
        strcpy(person -> age, argv[5]);
        strcpy(person -> addr, argv[6]);
        strcpy(person -> phone, argv[7]);
        strcpy(person -> email, argv[8]);

        insert(fp, person);

    }
    else if(!strcmp(argv[1], "d")) {

        delete(fp, argv[3]);

    }
	return 1;
}
