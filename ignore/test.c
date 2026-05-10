#include <stdio.h>

int main() {
    char c = 'a';
    printf("%i\n",((unsigned)((c) | 32) - 'a') < 26);
    
    return 0;
}