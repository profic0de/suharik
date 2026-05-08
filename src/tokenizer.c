#include "kit.h"

struct file ** files;

#define stack_block(type, ptr) root.blocks = array_append(root.blocks, make_block(type, ptr))

char* handle_token(char** bytes);
int parse_fd(FILE* fd) {
    // static char check;
    // if (check||check++) goto skip;
    // lookup(spaces, " \t\n\r\v\f");
    // lookup(equal_oper, "+-*/%%!><&|^");
    // lookup(double_oper, "+-=><&|");
    // lookup(delimiters, " \t\n\r\v\f,{}[]()+-*/%%=!><&|^~.\"\'");
    // lookup(operators, "+-*/%%=!><&|^~.{[(,");
    // skip:

    #define chr (c=getc(fd))!=EOF

    int c;
    char* bytes = NULL;
    while (chr) {
        if (bytes) bytes = (free(bytes), NULL);

        if (c=='#') { //Preprocessor
            while (chr&&c!='\n') str_append(&bytes, c);
            if (!bytes) continue;
            int len = strlen(bytes);
            if (len<10) continue;
            if (strncmp(bytes, "require ", 8)) continue;
            // puts(bytes+8);
            char* temp = bytes+8;
            char end = temp[0]=='\''||temp[0]=='"'?temp[0]:temp[0]=='<'?'>':'\n';
            // putc(end, stdout);

            int i = 0;
            while (*++temp&&*temp!=end) i++;

            if (!(*temp)) continue;
            // printf("%.*s\n", i, bytes+9);
            //TODO: Use this someday
        }

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