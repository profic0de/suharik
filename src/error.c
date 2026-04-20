#include "kit.h"

#define RED "\x1b[31m"
#define RESET "\x1b[0m"

void error_at_token(struct token* token, const char* message, va_list args) {
    // Header
    fprintf(stderr, "  File \"%s\", line %d, col %d\n", 
            token->file->filename, token->line, token->col);
    
    // Extract the line from file
    char* line_start = token->file->bytes;
    char* line_end = token->file->bytes;
    
    // Find start of current line
    for (char* p = token->file->bytes; p < token->file->bytes + token->file->filelen; p++) {
        if (p - token->file->bytes >= token->col) {
            line_start = p;
            while (line_start > token->file->bytes && *(line_start-1) != '\n') line_start--;
            line_end = p;
            while (line_end < token->file->bytes + token->file->filelen && *line_end != '\n') line_end++;
            break;
        }
    }
    
    // Print the code line with token highlighted
    fprintf(stderr, "    ");
    
    // Calculate token start/end positions in line
    int token_start = token->col - (line_start - token->file->bytes);
    int token_end = token_start + (token->value ? strlen(token->value) : 1);
    
    // Print line with highlighted token
    for (int i = 0; i < line_end - line_start; i++) {
        if (i >= token_start && i < token_end) {
            fprintf(stderr, RED);
        }
        fprintf(stderr, "%c", line_start[i]);
        if (i >= token_start && i < token_end) {
            fprintf(stderr, RESET);
        }
    }
    fprintf(stderr, "\n    ");
    
    // Print pointer to error
    int col_offset = token->col - (line_start - token->file->bytes);
    for (int i = 0; i < col_offset; i++) fprintf(stderr, " ");
    fprintf(stderr, RED "^" RESET "\n");
    
    // Print error message
    fprintf(stderr, RED "SyntaxError: " RESET);
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");
}

void error(struct token* token, const char* message, ...) {
    va_list args;
    va_start(args, message);
    
    if (token) {
        error_at_token(token, message, args);
    } else {
        fprintf(stderr, "Error: ");
        vfprintf(stderr, message, args);
        fprintf(stderr, "\n");
    }
    
    va_end(args);
}