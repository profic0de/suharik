#include "kit.h"

struct token{
    char* value;
    enum {
        KEY,
        NUM,
        STR,
        OPER
    } type;
};

int get_keyword(const char *line, const char **keywords) {
    int left = 0, right = 0;
    while (keywords[right]) right++;
    right--;
    
    while (left <= right) {
        int mid = (left + right) / 2;
        const char *kw = keywords[mid];
        
        int cmp = 0;
        int len = 0;
        for (const char *a = line, *b = kw; *b; a++, b++, len++) {
            if (*a != *b) {
                cmp = (*a < *b) ? -1 : 1;
                break;
            }
        }
        
        // matched entire keyword, check it's a word boundary
        if (cmp == 0 && !isalnum(line[len]) && line[len] != '_') {
            return len;
        }
        
        if (cmp < 0) left = mid + 1;
        else right = mid - 1;
    }
    return 0;
}

const char* keywords[] = {"func","#include",NULL};

static struct token* parse_token(struct file* file, size_t* offset) {
    char* end = file->bytes+file->filelen;
    char* bytes = file->bytes+*offset;
    size_t len;
    while (!(len = get_keyword(bytes++, keywords)))
        if (bytes>=end) return NULL;

    print("%lu: %s",len,bytes);
    struct token* token = auto_free(calloc(1, sizeof(struct token)));
    token->type=KEY;
    token->value=auto_free(strndup(bytes, len));
    *offset = (bytes - file->bytes) + len;
    return token;
}

int parse_file(int idx) {
    print("Started parsing:\n%s",files[idx]->bytes);
    struct token* token;
    size_t offset;
    while ((token = parse_token(files[idx], &offset))) files[idx]->tokens = array_append(files[idx]->tokens,token);
    auto_free(files[idx]->tokens);

    return 0;
}