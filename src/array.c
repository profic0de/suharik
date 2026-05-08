#include "kit.h"

#undef array_append
// I really like null terminated arrays
void** array_append(void** arr, void* ptr) {
    if (!ptr) return arr;
    if (!arr) {
        void** temp = calloc(2, sizeof(ptr));
        temp[0] = ptr;
        return temp;
    }
    void** temp = arr; 
    while (*temp++);

    size_t diff = (size_t)(temp-arr);
    arr[diff-1] = ptr;
    arr = realloc(arr, sizeof(void*)*(diff+1));
    arr[diff] = 0;
    return arr;
}

int dict_append(char*** arr, char* ptr) {
    if (!ptr) return 0;
    #define arr (*arr)

    if (!arr) {
        char** temp = calloc(2, sizeof(ptr));
        temp[0] = ptr;
        arr = temp;
        return 0;
    }

    char** temp = arr-1; 
    while (*++temp) if (!strcmp(*temp, ptr)) return 1;

    size_t diff = (size_t)(temp-arr);
    arr[diff-1] = ptr;
    arr = realloc(arr, sizeof(char*)*(diff+1));
    arr[diff] = 0;
    return 0;
    #undef arr
}

void str_append(char** str, char c) {
    if (!c) return;
    #define arr (*str)
    if (!arr) {
        arr = calloc(2, sizeof(c));
        arr[0] = c;
        return;
    }
    char* temp = arr;
    while (*temp++);

    size_t diff = (size_t)(temp-arr);
    arr[diff-1] = c;
    arr = realloc(arr, sizeof(char*)*(diff+1));
    arr[diff] = 0;
    return;
}