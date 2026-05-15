#include "kit.h"

char** tokens;
enum token_type* types;

char* handle_token(char** bytes, enum token_type token_type) {
    if (bytes) return (tokens = array_append(tokens, strdup(*bytes)), types = array_append(types, token_type),NULL);

    for (size_t i=0; tokens[i]; i++) {
        
    }

    return NULL;
}