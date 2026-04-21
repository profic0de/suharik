#include "kit.h"

struct file** files;
struct block {
    enum {
        STRING,
        INTEGER,
        NEWLINE,
        FUNCTION,
        SYMBOL,
        KEYWORD
    } type;
    union {
        struct block** blocks;
        char* content;
    };
} root;

int parse_chunks(FILE* fd) {
    char bytes[4096];
    fread(bytes, sizeof(char), sizeof(bytes), fd);
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
        struct file **temp = files-1;
        while (*++temp&&strcmp(temp[0]->filename,filename));
        if (*temp) return 0;
    }

    FILE* fd = fopen(filename,"r");
    if (!fd) {
        print("failed to open %s",filename);
        return 1;
    }

    fseek(fd, 0L, SEEK_SET);  // Move pointer to the end of the file
    // ftell(fd);
    // char* bytes = auto_free(malloc(size+1));

    // char bytes[4096];
    // size_t got = fread(bytes, sizeof(char), 4096, fd);
    // bytes[got] = 0;

    struct file* file = auto_free(calloc(1,sizeof(struct file)));
    // file->bytes = bytes;
    // file->filelen = 4096;
    file->filename = filename;    

    files = array_append(files, file);
    
    struct file** temp = files;
    while (*temp++);
    // parse_file((int)(temp-files)-2);

    fclose(fd);
    return 0;
}

__attribute__((destructor))
static void cleanup() {
    free(files);
}