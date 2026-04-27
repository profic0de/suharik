#include "kit.h"

//

void error_message(const char* filename, size_t line, size_t column, char* error) {
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
    
    fprintf(stderr, "%s:%zu:%zu: %s%s:%s %s\n", filename, line + 1, column, color_code, msg_type, "\033[0m", msg_text);
    
    FILE* fd = fopen(filename, "r");
    if (!fd) return;
    
    char line_buffer[1024] = {0};
    size_t current_line = 0;
    
    while (fgets(line_buffer, sizeof(line_buffer), fd)) {
        if (current_line == line) {
            // Find word boundaries around the error column
            size_t col_idx = column - 1;
            size_t word_start = col_idx;
            size_t word_end = col_idx;
            
            // Find start of word
            while (word_start > 0 && (isalnum(line_buffer[word_start-1]) || line_buffer[word_start-1] == '_')) {
                word_start--;
            }
            
            // Find end of word
            while (isalnum(line_buffer[word_end]) || line_buffer[word_end] == '_') {
                word_end++;
            }
            
            // Print line with colored token
            fprintf(stderr, "%5zu | ", line + 1);
            for (size_t i = 0; line_buffer[i] && line_buffer[i] != '\n'; i++) {
                if (i >= word_start && i < word_end) {
                    fprintf(stderr, "%s%c\033[0m", color_code, line_buffer[i]);
                } else {
                    fprintf(stderr, "%c", line_buffer[i]);
                }
            }
            fprintf(stderr, "\n");
            
            // Print underline
            fprintf(stderr, "      | ");
            for (size_t i = 0; i < word_start; i++) fprintf(stderr, " ");
            
            fprintf(stderr, "%s", color_code);
            fprintf(stderr, "^");
            for (size_t i = word_start + 1; i < word_end; i++) {
                fprintf(stderr, "~");
            }
            fprintf(stderr, "\033[0m\n");
            break;
        }
        current_line++;
    }
    
    fclose(fd);
}