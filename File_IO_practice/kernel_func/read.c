#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

    FILE *fp;

    fp = fopen(argv[1], "rt");

    long offset = atol(argv[2]);
    int readbyte = atoi(argv[3]);
    int count = 0;
    char ch;

    fseek(fp, offset, SEEK_SET);
    
    while((ch = fgetc(fp)) != EOF && count < readbyte) {
        putchar(ch);
        count++;
    }

    fclose(fp);

    return 0;
}
    
