#include "kit.h"

size_t line, column;

#define UNGETC_STACK_MAX 8
static size_t column_stack[UNGETC_STACK_MAX];
static int stack_top = 0;
static FILE *last_stream = NULL;

static inline void reset_stack(FILE *stream) {
    if (stream != last_stream) {
        stack_top = 0;
        last_stream = stream;
    }
}

#undef getc
int __getc(FILE *__stream) {
    reset_stack(__stream);
    char c = getc(__stream);
    if (c=='\n') column = (++line-line);
    else column++;
    return c;
}

#undef ungetc
int __ungetc(int __c, FILE *__stream) {
    reset_stack(__stream);
    if (__c == EOF) return EOF;
    int ret = ungetc(__c, __stream);
    if (ret == EOF) return EOF;

    if (stack_top < UNGETC_STACK_MAX)
        column_stack[stack_top++] = column;

    if (__c == '\n') {
        line--;
        column = (stack_top > 0) ? column_stack[--stack_top] : 0;
    } else {
        if (column > 0) column--;
    }

    return ret;
}