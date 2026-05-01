#include "kit.h"

char** tokens;

char* handle_token(char** bytes) {
    if (bytes) return (tokens = array_append(tokens, strdup(*bytes)),NULL);
    
    char** temp = tokens-1;

    #define token(offset) (*(temp+(offset)))

    while (*++temp) {
        
    }

    return NULL;
}