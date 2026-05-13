#include "kit.h"

struct file ** files;

#define stack_block(type, ptr) root.blocks = array_append(root.blocks, make_block(type, ptr))

char* handle_token(char** bytes);
int parse_fd(FILE* fd) {
    static char check;
    if (check||check++) goto skip;
    static char operators[256] = {0};
    operators['+']++;
    operators['-']++;
    operators['/']++;
    operators['*']++;
    operators['!']++;
    operators['=']++;
    operators['%']++;
    operators['>']++;
    operators['<']++;
    operators['(']++;
    operators[')']++;
    operators['&']++;
    operators['|']++;
    operators['^']++;
    operators['~']++;
    operators['{']++;
    operators['}']++;
    skip:

    struct file** file = files-1; while (*++file); file -= 1;
    #define chr ((c=getc(fd))!=EOF)

    int c = 0;
    char* bytes = NULL;
    while (c!=EOF) {
        enum {
            NONE,
            NUMBER,
            FLOAT,
            KEYWORD,
            SYMBOL,
            STRING,
            PATH
        } token_type = NONE;

        // [ ] Getting the token type
        if (!chr) return 0;
        else if (c=='#') while (chr&&c!='\n');
        if (isspace(c)) while (chr&&isspace(c));

        if (isdigit(c)) token_type = NUMBER;
        else if (operators[c]) token_type = SYMBOL;
        printf("%c ", c);
        
        if (bytes) bytes = (printf("%s ",bytes), free(bytes), NULL);    
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
        struct file** temp = files-1;
        while (*++temp&&strcmp(temp[0]->filename,filename));
        if (*temp) return 0;
    }

    FILE* fd = fopen(filename,"r");
    if (!fd) {
        print("failed to open %s",filename);
        return 1;
    }

    struct file* file = auto_free(malloc(sizeof(struct file)));
    file->filename = auto_free(strdup(filename));
    file->requirements = NULL;

    files = array_append(files, file);

    int r = parse_fd(fd);

    auto_free(file->requirements);

    fclose(fd);
    return r;
}

__attribute__((destructor))
static void clean() {
    free(files);
}