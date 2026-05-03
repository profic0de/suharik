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

#define bitset(arr, bit) ((arr)[(bit) >> 3] |= (1 << ((bit) & 7)))
#define bitget(arr, bit) (((arr)[(bit) >> 3] >> ((bit) & 7)) & 1)
#define lookup(name, str) static unsigned char name[32] = {0}; do { for (const char* p = (str); *p; p++) { bitset(name, (unsigned char)*p); } } while (0)
#define flip(arr) do { for (size_t i = 0; i < (32); i++) { (arr)[i] ^= 0xFF; } } while (0)

extern size_t line, column;
void cleanup(void);
int __getc(FILE *__stream);
int __ungetc(int __c, FILE *__stream);
#define getc(__stream) __getc(__stream)
#define ungetc(__c, __stream) __ungetc(__c, __stream)
extern char** files;
void* auto_free(void* ptr);
void str_append(char** str, char c);
void** array_append(void** arr, void* ptr);
void error_message(const char* filename, size_t s_line, size_t s_column, size_t token_len, const char* fmt, ...);
#define array_append(arr, ptr) ((__typeof__(arr))array_append(((void**)(arr)), ((void*)(ptr))))
#define print(fmt, ...) printf("[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#undef EOF
#define EOF 0xFF

#endif
