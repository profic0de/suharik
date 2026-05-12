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
    #define chr (c=getc(fd))!=EOF

    int c;
    char* bytes = NULL;
    while (chr) {
        if (bytes) bytes = (printf("'%s' ",bytes), free(bytes), NULL);

        if (c=='#') { //Preprocessor
            size_t tc = column;
            while (chr&&c!='\n') str_append(&bytes, c);
            if (!bytes) continue;
            int len = strlen(bytes);
            if (len<10) continue;
            if (strncmp(bytes, "require ", 8)) continue;
            // puts(bytes+8);
            char* str = bytes+8;
            char end = str[0]=='\''||str[0]=='"'?str[0]:str[0]=='<'?'>':'\n';
            // putc(end, stdout);

            int i = 0;
            while (*++str&&*str!=end) i++;

            if (!*str) continue;
            // printf("%.*s\n", i+1, bytes+8);

            if (dict_append(&file[0]->requirements, auto_free(strndup(bytes+8, i+1))))
                return (error_message(file[0]->filename, line-1, tc+9, i+2, "error: Requirement allready satisfied"), bytes = (free(bytes), NULL), 1);
            continue;
        } else if (isspace(c)) continue;

        enum {
            NONE,
            NUMBER,
            FLOAT,
            KEYWORD,
            OPERATOR,
            STRING,
            PATH
        } token_type = NONE;

        size_t tc = column;

        char brackets = '\x00';

        ungetc(c, fd);
        if (isdigit(c)) token_type = NUMBER;
        else if (c=='"'||c=='\'') token_type = (str_append(&bytes, getc(fd)), brackets = c, STRING);
        else if (operators[c]) token_type = OPERATOR;
        else if (c==EOF) return 0;
        else token_type = KEYWORD;

        // str_append(&bytes, c);

        int exit=0, p=0;
        while (chr&&!exit) {
            switch (token_type) {
            case STRING:
                if (c=='\n') return (error_message(file[0]->filename, line-1, tc, 1, "error: Unmatched opening bracket"), bytes = (free(bytes), NULL), 1);
                if (c==brackets&&p!='\\') exit = 1;
                // if (p=='\\'); TODO: Replace \" with " etc. automatically
                if (!exit) str_append(&bytes, c);
                break;
            
            case KEYWORD:
                if (!(isalnum(c)||c=='_')) exit = (ungetc(c, fd), 1);

                if (!exit) str_append(&bytes, c);
                break;

            case OPERATOR:
                if (!operators[c]) exit = (ungetc(c, fd), 1);
                
                if (!exit) str_append(&bytes, c);
                break;

            case NUMBER:
                if (!isdigit(c)) exit = (ungetc(c, fd), 1);
                
                if (!exit) str_append(&bytes, c);
                break;

            default:
                break;
            }
            p=c;
        } if (c=='#') ungetc(c, fd);

        if (bytes) bytes = (printf("'%s' ",bytes), free(bytes), NULL);
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