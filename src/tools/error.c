#include "kit.h"
#include <stdarg.h>

//This file wasn't made by a human

void error_message(const char* filename, size_t s_line, size_t s_column, size_t token_len, const char* fmt, ...) {
    // Format the error message
    char error[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(error, sizeof(error), fmt, args);
    va_end(args);
    // Determine message type and color
    const char* msg_type = "note";
    const char* color_code = "\033[1;36m";  // cyan by default
    const char* msg_text = error;
    
    // Check in order of string length (longest first to avoid partial matches)
    if (strncmp(error, "fatal error", 11) == 0) {
        msg_type = "fatal error";
        color_code = "\033[1;31m";  // bright red
        msg_text = error + 11;
        while (*msg_text && (*msg_text == ' ' || *msg_text == ':')) msg_text++;
    } else if (strncmp(error, "warning", 7) == 0) {
        msg_type = "warning";
        color_code = "\033[1;35m";  // purple/magenta
        msg_text = error + 7;
        while (*msg_text && (*msg_text == ' ' || *msg_text == ':')) msg_text++;
    } else if (strncmp(error, "remark", 6) == 0) {
        msg_type = "remark";
        color_code = "\033[1;36m";  // cyan
        msg_text = error + 6;
        while (*msg_text && (*msg_text == ' ' || *msg_text == ':')) msg_text++;
    } else if (strncmp(error, "error", 5) == 0) {
        msg_type = "error";
        color_code = "\033[1;31m";  // red
        msg_text = error + 5;
        while (*msg_text && (*msg_text == ' ' || *msg_text == ':')) msg_text++;
    } else if (strncmp(error, "info", 4) == 0) {
        msg_type = "info";
        color_code = "\033[1;36m";  // cyan
        msg_text = error + 4;
        while (*msg_text && (*msg_text == ' ' || *msg_text == ':')) msg_text++;
    } else if (strncmp(error, "note", 4) == 0) {
        msg_type = "note";
        color_code = "\033[1;36m";  // cyan
        msg_text = error + 4;
        while (*msg_text && (*msg_text == ' ' || *msg_text == ':')) msg_text++;
    }
    
    fprintf(stderr, "%s:%zu:%zu: %s%s:%s %s\n", filename, s_line + 1, s_column, color_code, msg_type, "\033[0m", msg_text);
    
    FILE* fd = fopen(filename, "r");
    if (!fd) return;
    
    char line_buffer[1024] = {0};
    size_t current_line = 0;
    
    while (fgets(line_buffer, sizeof(line_buffer), fd)) {
        if (current_line == s_line) {
            size_t word_start = s_column - 1;
            size_t word_end = word_start + token_len;
            size_t line_len = strlen(line_buffer);
            if (line_len > 0 && line_buffer[line_len-1] == '\n') line_len--;
            
            // Get terminal width (default to 80)
            int term_width = 80;
            const char* cols = getenv("COLUMNS");
            if (cols) term_width = atoi(cols);
            
            // Calculate context width (1/3 of terminal, minus space for line number and separators)
            size_t context_width = (term_width - 10) / 3;
            if (context_width < 10) context_width = 10;
            
            // Calculate context boundaries
            size_t context_start = (word_start > context_width) ? word_start - context_width : 0;
            size_t context_end = (word_end + context_width < line_len) ? word_end + context_width : line_len;
            
            // Print line with context
            fprintf(stderr, "%5zu | ", s_line + 1);
            
            // Print "..." if truncated at start
            if (context_start > 0) fprintf(stderr, "...");
            
            // Adjust word position for display
            size_t display_word_start = word_start - context_start;
            size_t display_word_end = word_end - context_start;
            
            // Print the context
            for (size_t i = context_start; i < context_end; i++) {
                if (i >= word_start && i < word_end) {
                    fprintf(stderr, "%s%c\033[0m", color_code, line_buffer[i]);
                } else {
                    fprintf(stderr, "%c", line_buffer[i]);
                }
            }
            
            // Print "..." if truncated at end
            if (context_end < line_len) fprintf(stderr, "...");
            fprintf(stderr, "\n");
            
            // Print underline
            fprintf(stderr, "      | ");
            if (context_start > 0) fprintf(stderr, "   ");  // Account for "..."
            for (size_t i = 0; i < display_word_start; i++) fprintf(stderr, " ");
            
            fprintf(stderr, "%s", color_code);
            fprintf(stderr, "^");
            for (size_t i = display_word_start + 1; i < display_word_end; i++) {
                fprintf(stderr, "~");
            }
            fprintf(stderr, "\033[0m\n");
            break;
        }
        current_line++;
    }
    
    fclose(fd);
}