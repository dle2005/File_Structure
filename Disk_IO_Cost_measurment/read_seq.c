#include <stdio.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char* argv[]) {

    FILE *fp = fopen(argv[1], "r");

    struct timeval startTime, endTime;
    int diffTime;
    char buffer[101];

    gettimeofday(&startTime, NULL);

    while(!feof(fp)) {
        fread(buffer, sizeof(char), 100, fp);
        memset(buffer, 0, 100);
    }

    int records = ftell(fp) / 100;

    gettimeofday(&endTime, NULL);
    diffTime = (endTime.tv_usec - startTime.tv_usec) * 1000;
    printf("#records: %d timecost: %d us\n", records, diffTime);

    fclose(fp);

    return 0;
}
