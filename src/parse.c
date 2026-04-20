#include "kit.h"

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
static struct token* parse_token(struct file* file, size_t* offset, int* line, int* col) {
    char* bytes = file->bytes + *offset;
    char* end = file->bytes + file->filelen;
    
    // Skip whitespace (but NOT newlines)
    while (bytes < end && isspace(*bytes) && *bytes != '\n') {
        bytes++;
        (*offset)++;
        (*col)++;
    }
    
    if (bytes >= end) return NULL;
    
    struct token* token = auto_free(calloc(1, sizeof(struct token)));
    char* start = bytes;
    int start_col = *col;
    int start_line = *line;
    
    // Newline(s)
    if (*bytes == '\n') {
        bytes++;
        (*offset)++;
        (*line)++;
        *col = 0;
        
        // Skip multiple consecutive newlines, treat as single newline
        while (bytes < end && *bytes == '\n') {
            bytes++;
            (*offset)++;
            (*line)++;
        }
        token->type = NEWLINE;
        token->value = NULL;
        token->line = start_line;
        token->col = start_col;
        token->file = file;
        return token;
    }
    
    // String
    if (*bytes == '"') {
        bytes++;
        (*col)++;
        while (bytes < end && *bytes != '"') {
            if (*bytes == '\\') {
                bytes++;
                (*col)++;
            }
            bytes++;
            (*col)++;
        }
        if (bytes < end) {
            bytes++;
            (*col)++;
        }
        token->type = STR;
    }
    // Number
    else if (isdigit(*bytes)) {
        while (bytes < end && (isdigit(*bytes) || *bytes == '.')) {
            bytes++;
            (*col)++;
        }
        token->type = NUM;
    }
    // Identifier/Keyword
    else if (isalpha(*bytes) || *bytes == '_' || *bytes == '#') {
        if (*bytes == '#') {
            bytes++;
            (*col)++;
            while (bytes < end && isalpha(*bytes)) {
                bytes++;
                (*col)++;
            }
        } else {
            while (bytes < end && (isalnum(*bytes) || *bytes == '_')) {
                bytes++;
                (*col)++;
            }
        }
        
        token->type = IDENT;
        size_t len = bytes - start;
        
        // Check if keyword
        const char* keywords[] = {"func", "import", "if", "else", NULL};
        for (const char** kw = keywords; *kw; kw++) {
            if (len == strlen(*kw) && strncmp(start, *kw, len) == 0) {
                token->type = KEY;
                break;
            }
        }
    }
    // Operator or punctuation
    else if (strchr("+-*/()=,{}<>!", *bytes)) {
        token->type = OPER;
        bytes++;
        (*col)++;
        
        // Check for two-character operators
        if (bytes < end) {
            char two_char[3] = {start[0], *bytes, '\0'};
            const char* multi_ops[] = {"++", "--", "+=", "-=", "*=", "/=", "==", "!=", "<=", ">=", ">>", "<<", NULL};
            
            for (const char** op = multi_ops; *op; op++) {
                if (strcmp(two_char, *op) == 0) {
                    bytes++;
                    (*col)++;
                    break;
                }
            }
        }
    }
    else {
        // Unknown character, skip it and return NULL
        (*offset)++;
        (*col)++;
        free(token);
        return NULL;
    }
    
    size_t len = bytes - start;
    token->value = auto_free(strndup(start, len));
    token->line = start_line;
    token->col = start_col;
    token->file = file;
    *offset += len;
    return token;
}

int parse_file(int idx) {
    struct token* token;
    size_t offset = 0;
    int line = 1, col = 0;
    while ((token = parse_token(files[idx], &offset, &line, &col))) {
        files[idx]->tokens = array_append(files[idx]->tokens, token);
    }
    auto_free(files[idx]->tokens);
    return 0;
}
