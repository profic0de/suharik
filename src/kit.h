#ifndef KIT_H
#define KIT_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>
#include <stdarg.h>

struct token{
    char* value;
    enum {
        KEY,
        NUM,
        STR,
        IDENT,
        OPER,
        NEWLINE
    } type;
    int line;
    int col;
    struct file* file;
};

extern struct file {
    char* filename;
    size_t filelen;
    char* bytes;
    struct token** tokens;
}** files;

void error(struct token* token, const char* message, ...);
void* auto_free(void* ptr);
void** array_append(void** arr, void* ptr);
#define array_append(arr, ptr) ((__typeof__(arr))array_append(((void**)(arr)), ((void*)(ptr))))
#define print(fmt, ...) printf("[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif
