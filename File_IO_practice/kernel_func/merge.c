#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {

    FILE *fp1, *fp2;

    fp1 = fopen(argv[1], "r+");
    fp2 = fopen(argv[2], "r");
    
    char buf[101];
    size_t count;

    fseek(fp1, 0, SEEK_END);

    while(!feof(fp2)) {
        count = fread(buf, sizeof(char), 100, fp2);
        fwrite(buf, sizeof(char), count, fp1);
        memset(buf, 0, 100);
    }

    fclose(fp1);
    fclose(fp2);

    return 0;
}
