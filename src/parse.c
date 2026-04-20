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
    
    struct token* token = auto_free(calloc(1, sizeof(struct token)));
    char* start = bytes;
    
    // String
    if (*bytes == '"') {
        bytes++;
        while (bytes < end && *bytes != '"') {
            if (*bytes == '\\') bytes++;
            bytes++;
        }
        if (bytes < end) bytes++;
        token->type = STR;
    }
    // Number
    else if (isdigit(*bytes)) {
        while (bytes < end && (isdigit(*bytes) || *bytes == '.')) bytes++;
        token->type = NUM;
    }
    // Identifier/Keyword (including # for #include)
    else if (isalpha(*bytes) || *bytes == '_' || *bytes == '#') {
        // For #include, consume # and letters
        if (*bytes == '#') {
            bytes++;
            while (bytes < end && isalpha(*bytes)) bytes++;
        } else {
            while (bytes < end && (isalnum(*bytes) || *bytes == '_')) bytes++;
        }
        
        token->type = IDENT;
        size_t len = bytes - start;
        
        // Check if keyword
        const char* keywords[] = {"func", "#include", "if", "else", NULL};
        for (const char** kw = keywords; *kw; kw++) {
            if (len == strlen(*kw) && strncmp(start, *kw, len) == 0) {
                token->type = KEY;
                break;
            }
        }
    }
    // Operator or punctuation
    else if (strchr("+-*/()=,{}><", *bytes)) {
        bytes++;
        token->type = OPER;
    }
    else {
        // Unknown character, skip it and return NULL
        (*offset)++;
        free(token);
        return NULL;
    }
    
    size_t len = bytes - start;
    token->value = auto_free(strndup(start, len));
    *offset += len;
    return token;
}

int parse_file(int idx) {
    // print("Started parsing:\n%s",files[idx]->bytes);
    struct token* token;
    size_t offset = 0;
    while ((token = parse_token(files[idx], &offset))) files[idx]->tokens = array_append(files[idx]->tokens,token);
    auto_free(files[idx]->tokens);
    struct token** temp = files[idx]->tokens-1;
    while (*++temp) {
        printf("%s\n",(*temp)->value);
    }

    return 0;
}