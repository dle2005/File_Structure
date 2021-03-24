#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {

    FILE *fp;

    fp = fopen(argv[1], "r+");
    
    long offset = atol(argv[2]);
    int datalen = strlen(argv[3]);

    fseek(fp, offset, SEEK_SET);

    fwrite(argv[3], sizeof(char), datalen, fp);

    fclose(fp);

    return 0;
}

