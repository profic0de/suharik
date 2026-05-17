#include <stdio.h>

int main() {

    size_t r = (size_t)0xFF<<(8*7);

    printf("%016lx\n",r);

    return 0;
}