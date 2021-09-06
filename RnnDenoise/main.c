#include <stdio.h>
#include "rnnoise.h"
#include <time.h>
#include <winsock.h>
#include <stdint.h>
#define FRAME_SIZE 480

static inline uint64_t get_time()
{   
    static int ever = 0;
    static LARGE_INTEGER frequency;
    if (!ever) {
        QueryPerformanceFrequency(&frequency);
        ever = 1;
    }
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return (t.QuadPart * (uint64_t)1e9) / frequency.QuadPart;
}

int main(int argc, char** argv) {
    int i;
    int first = 1;
    float x[FRAME_SIZE];
    FILE* f1, * fout;
    DenoiseState* st;
    st = rnnoise_create(NULL);
    char* in_file = "D:\\test1.raw";
    char* out_file = "D:\\out.raw";
    f1 = fopen(in_file, "rb");
    fout = fopen(out_file, "wb");

    while (1) {
        char tmp[FRAME_SIZE << 2];
        fread(tmp, sizeof(char), FRAME_SIZE << 2, f1);
        if (feof(f1)) break;
        uint64_t start = get_time();

        for (i = 0; i < FRAME_SIZE; i++) {
            x[i] = ((short*)tmp)[i * 2];
        }
        rnnoise_process_frame(st, x, x);
        for (i = 0; i < FRAME_SIZE; i++) {
            ((short*)tmp)[i * 2] = x[i];
            ((short*)tmp)[i * 2 + 1] = x[i];
        }
        uint64_t end = get_time();
        printf("time %lu", (end - start) / 1000000);
        fwrite(tmp, sizeof(char), FRAME_SIZE << 2 , fout);
    }
   
    rnnoise_destroy(st);
    fclose(f1);
    fclose(fout);
    return 0;
}