#include "kit.h"

char** files;
struct block {
    enum {
        KEYWORD,
        STRING,
        NUMBER,
        FLOAT,
        NEWLINE,
        FUNCTION,
        SYMBOL
    } type;
    union {
        struct block** blocks;
        char* content;
        double num_f;
        long num_i;
    };
} root = {.type=FUNCTION};

struct block* make_block(char type, void* ptr) {
    struct block* block = malloc(sizeof(struct block));
    block->type = type;
    if (type==FUNCTION) block->blocks=ptr;
    else if (type==NUMBER) block->num_i=(long)ptr;
    else if (type==FLOAT) block->num_f = (double)(uintptr_t)ptr;
    else block->content=ptr;
    return auto_free(block);
}

#define stack_block(type, ptr) root.blocks = array_append(root.blocks, make_block(type, ptr))

int parse_fd(FILE* fd) {
    static char check;
    if (check||check++) goto skip;
    lookup(spaces, " \t\n\r\v\f");
    lookup(equal_oper, "+-*/%%!><&|^");
    lookup(double_oper, "+-=><&|");
    lookup(delimiters, " \t\n\r\v\f,{}[]()+-*/%%=!><&|^~.\"\'");
    lookup(operators, "+-*/%%=!><&|^~.{[(,");
    skip:
    // static unsigned char keywords[32] = {0}; for (int i = 0; i < 128; i++) if (!isalpha(i) && !isdigit(i) && i != '_') bitset(keywords, i); flip(keywords);
    char* bytes = 0;
    char c;
    while ((c = getc(fd))!=EOF) {
        // if (c=='\n') {stack_block(NEWLINE, NULL); continue;} else 
        if (bitget(spaces,c)) continue;
        else if (c=='#') { //Skip comments
            while ((c = getc(fd))!=EOF&&c!='\n');
            // stack_block(NEWLINE, NULL);
            continue;
        }
        else str_append(&bytes,c);

        size_t s_line=line, s_column=column, s_ch=ch;

        if (c=='\''||c=='\"') {
            char cc=c, p=0;
            while ((c = getc(fd))!=EOF&&!(c==cc&&p!='\\')) {
                str_append(&bytes,c);
                p = c;
            }
            goto skip1;
        }
        else if (bitget(operators,c)) {
            char c2=getc(fd), c3=getc(fd), count=0;
            if (c2==EOF||c3==EOF) {
                if (c2==EOF) break;
                if (c3==EOF) count=bitget(operators,c2);
            } else {
                char cc = bitget(operators,c2);
                count = cc+(cc&&bitget(operators,c3));
            }
            switch (count) {      
            case 2:
                if ((c=='<'||c=='>')&&c==c2&&c3=='=') {
                    str_append(&bytes,c2);
                    str_append(&bytes,c3);
                    break;
                }

            case 1:
                ungetc(c3, fd);
                if (bitget(equal_oper,c)&&c2=='=') {
                    str_append(&bytes,c2);
                } else if (bitget(double_oper,c)&&c==c2) {
                    str_append(&bytes,c2);
                }

                break;

            case 0:
                ungetc(c3, fd);
                ungetc(c2, fd);
                break;
            }
            goto skip1;
        }

        while ((c = getc(fd))!=EOF&&!bitget(delimiters,c)) str_append(&bytes,c); ungetc(c, fd);
        skip1:

        char* handle_token(char** bytes);
        char* error = handle_token(&bytes);

        if (error) {
            //TODO: Make a function that will trigger an error
        }

        printf("%s ",bytes?bytes:"");
        free(bytes);
        bytes = 0;

        if (c==EOF) break;
    }
    printf("%s\n",bytes?bytes:"");
    free(bytes);
    
    return 0;
}

char* handle_token(char** bytes) {

    return NULL;
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
static void cleanup() {
    free(files);
}