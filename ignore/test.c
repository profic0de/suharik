#include <stdio.h>

#define false 0
#define true 1

int main() {

    unsigned char c = '[';
    size_t bytes = *(size_t*)"[]{}()";

    size_t r = 0x0101010101010101*c;
    printf("%016lx\n",r);
    r = r ^ bytes;
    printf("%016lx\n",r);
    r = (r - 0x0101010101010101) & ~r & 0x8080808080808080;
    printf("%016lx\n",r);

    return 0;
}