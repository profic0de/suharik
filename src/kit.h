#ifndef KIT_H
#define KIT_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdint.h>

extern size_t line, column;
void cleanup(void);
int __getc(FILE *__stream);
int __ungetc(int __c, FILE *__stream);
#define getc(__stream) __getc(__stream)
#define ungetc(__c, __stream) __ungetc(__c, __stream)
extern struct file {
    char* filename;
    char** requirements;
}** files;
void* auto_free(void* ptr);
void str_append(char** str, char c);
int dict_append(char*** arr, char* ptr);
void** array_append(void** arr, void* ptr);
void error_message(const char* filename, size_t s_line, size_t s_column, size_t token_len, const char* fmt, ...);

enum token_type {NONE,NUMBER,FLOAT,KEYWORD,SYMBOL,STRING,PATH,WORD};
struct AST {
    char* name;

    union {
        struct AST** array;
        uintptr_t value;
        // Can be pointer to type declaration
        void* pointer;
    };

    enum {
        ROOT,

        ARRAY,
        VARIABLE,
        FUNCTION,
        OBJECT,
        TYPE,

        OPERATION,
        VALUE,
    } type;
};

char* handle_token(char** bytes, enum token_type token_type);

#define lookup(size_t, c) (((0x0101010101010101*c ^ size_t) - 0x0101010101010101) & ~(0x0101010101010101*c ^ size_t) & 0x8080808080808080)
#define array_append(arr, ptr) ((__typeof__(arr))array_append(((void**)(arr)), ((void*)(ptr))))
#define print(fmt, ...) printf("[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif
