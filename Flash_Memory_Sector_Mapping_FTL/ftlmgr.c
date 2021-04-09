#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
//#include "blkmap.h"
#include "sectormap.h"

int dd_read(int, char*);
int dd_write(int, char*);
int dd_erase(int);

FILE *flashfp;

int mappingTable[DATAPAGES_PER_DEVICE];
int freeBlockPbn;

int front = -1;
int rear = -1;
int queue[DATAPAGES_PER_DEVICE];

int isEmpty() {
    if(front == rear) return true;
    else return false;
}

void addQ(int value) {
    rear = (rear+1)%DATAPAGES_PER_DEVICE;
    queue[rear] = value;
}

int deleteQ() {
    front = (front + 1) % DATAPAGES_PER_DEVICE;
    return queue[front];
}

void ftl_open() {

    for(int i = 0; i < DATAPAGES_PER_DEVICE; i++)
        mappingTable[i] = -1;

    freeBlockPbn = DATABLKS_PER_DEVICE;

    return;
}

void ftl_read(int lsn, char *sectorbuf) {

    char pagebuf[PAGE_SIZE];

    if(mappingTable[lsn] != -1) {
        dd_read(mappingTable[lsn], pagebuf);
        memcpy(sectorbuf, pagebuf, SECTOR_SIZE);
    }

    return;
}

void ftl_write(int lsn, char *sectorbuf) {

    char pagebuf[PAGE_SIZE];
    char emptyPage[PAGE_SIZE];
    char SpareData_lpn[4];
    int count = 0;

    memset(emptyPage, (char)0xFF, PAGE_SIZE);
    
    for(int i = 0; i < BLOCKS_PER_DEVICE * PAGES_PER_BLOCK; i++) {
        if((i / PAGES_PER_BLOCK) != freeBlockPbn) {
            dd_read(i, pagebuf);
            if(strcmp(pagebuf, emptyPage) == 0) count++;
        }
    }

    if(count == 0 && !(isEmpty())) {
        int deletePpn = deleteQ();
        int deleteBlock = deletePpn / 4;
        for(int i = 0; i < 4; i++) {
            int from = deleteBlock * 4 + i;
            int to = freeBlockPbn * 4 + i;
            if(from == deletePpn) i++;
            dd_read(from, pagebuf);
            dd_write(to, pagebuf);
        }
        dd_erase(deleteBlock);
        freeBlockPbn = deleteBlock;
    }

    if(mappingTable[lsn] == -1) {
        for(int i = 0; i < BLOCKS_PER_DEVICE * PAGES_PER_BLOCK; i++) {
            if((i / PAGES_PER_BLOCK) != freeBlockPbn) {
                dd_read(i, pagebuf);
                if(strcmp(pagebuf, emptyPage) == 0) {
                    mappingTable[lsn] = i;
                    memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
                    sprintf(SpareData_lpn, "%d", lsn);
                    memcpy(&pagebuf[SECTOR_SIZE], SpareData_lpn, 4);
                    dd_write(i, pagebuf);
                }
            }
        }
    }
    else if(mappingTable[lsn] != -1 && !(isEmpty())){
        for(int i = 0; i < BLOCKS_PER_DEVICE * PAGES_PER_BLOCK; i++) {
            if((i / PAGES_PER_BLOCK) != freeBlockPbn) {
                dd_read(i, pagebuf);
                if(strcmp(pagebuf, emptyPage) == 0) {
                   addQ(mappingTable[lsn]);
                   mappingTable[lsn] = i;
                   memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
                   sprintf(SpareData_lpn, "%d", lsn);
                   memcpy(&pagebuf[SECTOR_SIZE], SpareData_lpn, 4);
                   dd_write(i, pagebuf);
                }
            }
        }
    }
    else if(mappingTable[lsn] != -1 && count == 0 && isEmpty()) {
        int deletePpn = mappingTable[lsn];
        int deleteBlock = deletePpn / 4;
        for(int i = 0; i < 4; i++) {
            int from = deleteBlock * 4 + i;
            int to = freeBlockPbn * 4 + i;
            if(from == deletePpn) i++;
            dd_read(from, pagebuf);
            dd_write(to, pagebuf);
        }
        dd_erase(deleteBlock);
        freeBlockPbn = deleteBlock;
        memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
        sprintf(SpareData_lpn, "%d", lsn);
        memcpy(&pagebuf[SECTOR_SIZE], SpareData_lpn, 4);
        dd_write(mappingTable[lsn], pagebuf);
    }
}





                    


            
