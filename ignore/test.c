#include <stdio.h>

int main() {

    char* str = "return";
    size_t val = 0;
    size_t cmp = (size_t)'retu'<<(8*2)|'rn';
    while (*str) val = (val<<8)|*str++;

    printf("%016lx\n",val);
    printf("%016lx\n",cmp);

    return 0;
}