#include "kit.h"

char** tokens;

char* handle_token(char** bytes) {
    char* token = bytes ? *bytes : NULL;
    static char* previous_token = NULL;

    if (previous_token&&token&&previous_token[0]=='>')
        print("%s",token);

    if (previous_token) previous_token = (free(previous_token), NULL);
    previous_token = token;
    return NULL;
}