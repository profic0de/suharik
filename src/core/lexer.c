#include "kit.h"

struct file ** files;

#define stack_block(type, ptr) root.blocks = array_append(root.blocks, make_block(type, ptr))

int parse_fd(FILE* fd) {
    static char* list = "+-/*!=%%><()[]{}&|~^;,.";
    static char operators[256];
    if (operators[(int)*list]) goto skip;
    char* _list = list-1;
    while (*++_list) operators[(int)*_list]++;
    skip:

    struct file** file = files-1; while (*++file); file -= 1;
    #define chr ((c=getc(fd))!=EOF)

    int c = 0;
    char* bytes = NULL;
    while (c!=EOF) {
        enum token_type token_type = NONE;

        // [ ] Getting the token type
        if (!chr) break;
        if (isspace(c)) continue;
        if (c=='#') {
            while (chr&&c!='\n');
            continue;
        }

        if (isdigit(c)) {
            token_type = NUMBER;
            str_append(&bytes, c);
            while (chr) {
                if (isspace(c)||(operators[c]&&c!='.')) break;
                else if (isalpha(c)) return (free(bytes),error_message(file[0]->filename, line, column, 1, "error: a number can't contain letters"), 1);
                else if (c=='.'&&token_type==NUMBER) token_type = FLOAT;
                else if (c=='.') return (free(bytes),error_message(file[0]->filename, line, column, 1, "error: a number can only have one dot"), 1);
                str_append(&bytes, c);
            }
        } else if (operators[c]) {
            token_type = SYMBOL;
            str_append(&bytes, c);
            if (lookup(*(size_t*)"[]{}();,", c)||c=='.') c=getc(fd);
            else while (chr&&operators[c]) str_append(&bytes, c);
            ungetc(c, fd);
        } else if (c=='\''||c=='"') {
            token_type = STRING;
            size_t a = column;
            char b = c, p = 0, po = 0;
            str_append(&bytes, c);
            while (chr&&c==b?(po!=p&&p=='\\'):1&&c!='\n') po = (str_append(&bytes, c), p), p = c;
            if (c=='\n') return (free(bytes), error_message(file[0]->filename, line-1, a, 1, "error: string not closed"), 1);
        } else {
            token_type = KEYWORD;
            char p = 0;
            size_t col = column;
            size_t val = c;
            size_t len = 1;
            str_append(&bytes, c);
            while (chr) {
                if (!(isalnum(c)||c=='.')) {
                    if (p=='.') {
                        if (c=='\n') ungetc(c, fd);
                        return (free(bytes), error_message(file[0]->filename, line, col+1, 1, "error: expected a keyword"), 1);
                    } break;
                } if (p=='.'&&!isalpha(c)) return (free(bytes), error_message(file[0]->filename, line, col+1, 1, "error: invalid keyword"), 1);
                if (c=='.') token_type = PATH;
                if (p==c&&c=='.') return (free(bytes), error_message(file[0]->filename, line, column, 1, "error: expected a keyword"), 1);
                // if (token_type==PATH);
                str_append(&bytes, (p=c));
                col = column;
                val = (val<<8)|c;
                len++;
            } if (token_type==KEYWORD&&len<=7) {
                // break, if, while, else, return
                size_t words[] = {
                    (size_t)'brea'<<(8*1)|'k',
                    (size_t)'if',
                    (size_t)'whil'<<(8*1)|'e',
                    (size_t)'else',
                    (size_t)'retu'<<(8*2)|'rn',
                    (size_t)'cont'<<(8*4)|'inue',
                    (size_t)'obj',
                    0
                };
                
                int i = 0;
                while (words[i]) if (words[i++]==val) token_type = WORD;

            }
            ungetc(c, fd);
        }

        handle_token(&bytes, token_type);
        if (bytes) bytes = (free(bytes), NULL);
    }
    handle_token(NULL, 0);

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