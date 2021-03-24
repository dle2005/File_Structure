#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    
    FILE *rfp, *wfp;

    rfp = fopen(argv[1], "r");
    wfp = fopen(argv[2], "w");
    char buf[101];
    size_t count;

    while(!feof(rfp)) {
        count = fread(buf, sizeof(char), 100, rfp);
        fwrite(buf, sizeof(char), count, wfp);
        memset(buf, 0, 100);
    }

    fclose(rfp);
    fclose(wfp);
    return 0;
}
