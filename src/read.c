#include "kit.h"

struct file** files;
int parse_file(int idx);

int file_store(char* filename) {
    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        return 1;
    }

    if (!S_ISREG(sb.st_mode)) {
        print("%s is not a file",filename);
        return 1;
    }

    FILE* fd = fopen(filename,"r");
    if (!fd) {
        print("failed to open %s",filename);
        return 1;
    }

    fseek(fd, 0L, SEEK_END);  // Move pointer to the end of the file
    size_t size = ftell(fd);    // Get current position (total bytes)
    fseek(fd, 0L, SEEK_SET);

    char* bytes = auto_free(malloc(size+1));

    size_t got = fread(bytes, sizeof(char), size, fd);
    if (size > got) {
        print("%lu",got);
        return 1;
    }
    bytes[got] = 0;
    
    struct file* file = auto_free(calloc(1,sizeof(struct file)));
    file->bytes = bytes;    
    file->filelen = size;    
    file->filename = filename;    

    files = array_append(files, file);
    
    struct file** temp = files;
    while (*temp++);
    parse_file((int)(temp-files)-2);

    fclose(fd);
    return 0;
}

__attribute__((destructor))
static void cleanup() {
    free(files);
}