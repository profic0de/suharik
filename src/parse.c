#include "kit.h"

struct token{
    char* value;
    enum {
        KEY,
        NUM,
        STR,
        IDENT,
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

// const char* keywords[] = {"func","#include",NULL};
static struct token* parse_token(struct file* file, size_t* offset) {
    char* bytes = file->bytes + *offset;
    char* end = file->bytes + file->filelen;
    
    // Skip whitespace
    while (bytes < end && isspace(*bytes)) {
        bytes++;
        (*offset)++;
    }
    
    if (bytes >= end) return NULL;
    
    regex_t regex;
    regmatch_t match;
    int ret;
    
    const char* patterns[] = {
        "^[0-9]+\\.?[0-9]*",
        "^(func|#include|if|else)",
        "^\"[^\"]*\"",
        "^[a-zA-Z_][a-zA-Z0-9_]*",
        "^[+*/()==-]",
        NULL
    };
    
    int types[] = {NUM, KEY, STR, IDENT, OPER};
    
    for (int i = 0; patterns[i]; i++) {
        memset(&regex, 0, sizeof(regex));  // Initialize regex_t
        
        if (regcomp(&regex, patterns[i], REG_EXTENDED) != 0) {
            print("regcomp failed for pattern %d", i);
            continue;
        }
        
        // Create null-terminated substring for regexec
        size_t remaining = end - bytes;
        char* temp = auto_free(malloc(remaining + 1));
        memcpy(temp, bytes, remaining);
        temp[remaining] = '\0';
        
        ret = regexec(&regex, temp, 1, &match, 0);
        regfree(&regex);
        
        if (ret == 0) {  // Match found
            struct token* token = auto_free(calloc(1, sizeof(struct token)));
            token->type = types[i];
            size_t len = match.rm_eo - match.rm_so;
            
            if (bytes + len > end) {
                return NULL;
            }
            
            token->value = auto_free(strndup(bytes, len));
            *offset += len;
            return token;
        }
    }
    return NULL;
}


int parse_file(int idx) {
    // print("Started parsing:\n%s",files[idx]->bytes);
    struct token* token;
    size_t offset = 0;
    while ((token = parse_token(files[idx], &offset))) files[idx]->tokens = array_append(files[idx]->tokens,token);
    auto_free(files[idx]->tokens);
    files[idx]->tokens;
    while () {
        
    }
    

    return 0;
}