#include <stdio.h>

#define false 0
#define true 1

int main() {

    char* str = "\\x\"";
    int close = 0;
    char po = str[0];
    char p = str[1];
    char c = str[2];
    char b = str[2];

    // if (po^p?p=='\\':0) check = 1;

    close = ((po!=p&&p=='\\'));

    printf("continue: %s\n",close?"true":"false");

    return 0;
}