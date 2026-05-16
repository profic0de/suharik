#include "kit.h"

char** tokens;
enum token_type* types;
struct AST root = {.type=ROOT, .name="root"};

char* type_to_char(enum token_type token_type) {
    static char str[16];
    for (size_t i = 0; i < sizeof(str); i++) str[i] = 0;

    switch (token_type) {
    #define case_macro(val) case val: return (strcpy(str, #val), str);

    case_macro(NONE);
    case_macro(NUMBER);
    case_macro(FLOAT);
    case_macro(KEYWORD);
    case_macro(SYMBOL);
    case_macro(STRING);
    case_macro(PATH);

    #undef case_macro
    default:
        return (strcpy(str, "UNKNOWN"), str);
    }
    return str;
}

char* handle_token(char** bytes, enum token_type token_type) {
    // if (bytes) print("type: %s, token: %s",type_to_char(token_type),*bytes);
    if (bytes) {
        tokens = array_append(tokens, strdup(*bytes));
        types = array_append(types, token_type);
        return NULL;
    }

    size_t n;

    for (size_t i=0; tokens[i]; i++) {
        if (tokens[(n=i+1)]) types[n] = types[n*2]; // Shifting the types array bc the enum is int and the array consists of pointers so i need to multiply the index by 2 to skip the 0's :)

        print("type: %s, token: %s",type_to_char(types[i]),tokens[i]);
    }

    return NULL;
}