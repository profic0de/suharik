//This file was first of all made so i won't get annoyed by the asan
#include "kit.h"

static void** pointers;

void* auto_free(void* ptr) {
    if (!ptr) return ptr;
    pointers = array_append(pointers, ptr);
    return ptr;
}

__attribute__((destructor))
void cleanup() {
    if (!pointers) return;
    void** temp = pointers;
    while (*temp) free(*temp++);
    free(pointers);
    pointers = 0;
}