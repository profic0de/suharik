#include "kit.h"

int file_store(char* filename);
int run_file(int idx);

int main(int argc, char *argv[]) {
    int r = file_store(argv[argc-1]);
    return r;
}