#include "kit.h"

char** files;

#define stack_block(type, ptr) root.blocks = array_append(root.blocks, make_block(type, ptr))

char* handle_token(char** bytes);
int parse_fd(FILE* fd) {
    static char check;
    if (check||check++) goto skip;
    lookup(spaces, " \t\n\r\v\f");
    lookup(equal_oper, "+-*/%%!><&|^");
    lookup(double_oper, "+-=><&|");
    lookup(delimiters, " \t\n\r\v\f,{}[]()+-*/%%=!><&|^~.\"\'");
    lookup(operators, "+-*/%%=!><&|^~.{[(,");
    skip:

    int c;
    char p = 0;
    char* bytes = 0;
    while ((c=getc(fd))!=EOF) {
        if (c=='#'); // Preprocessor

        p = c;
    }
    

    return 0;
}

int file_store(char* filename) {
    struct stat sb;
    if (stat(filename, &sb) == -1) {
        // perror("stat");
        return 1;
    }

    if (!S_ISREG(sb.st_mode)) {
        // print("%s is not a file",filename);
        return 1;
    }

    if (files) {
        char** temp = files-1;
        while (*++temp&&strcmp(*temp,filename));
        if (*temp) return 0;
    }

    FILE* fd = fopen(filename,"r");
    if (!fd) {
        print("failed to open %s",filename);
        return 1;
    }

    // fseek(fd, 0L, SEEK_SET);  // Move pointer to the end of the file
    // ftell(fd);
    // char* bytes = auto_free(malloc(size+1));

    // char bytes[4096];
    // size_t got = fread(bytes, sizeof(char), 4096, fd);
    // bytes[got] = 0;

    files = array_append(files, auto_free(strdup(filename)));

    parse_fd(fd);

    // char** temp = files;
    // while (*temp++);
    // parse_file((int)(temp-files)-2);

    fclose(fd);
    return 0;
}

__attribute__((destructor))
static void clean() {
    free(files);
}