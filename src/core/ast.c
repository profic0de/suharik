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

    size_t len = 0; 
    for (size_t i=0; tokens[i]; len=++i) types[i] = types[i*2]; // Shifting the types array bc the enum is int and the array consists of pointers so i need to multiply the index by 2 to skip the 0's :)

    for (size_t i=0, r=len-1; i<len; i++, r--) {
        if (r>0&&types[i]==types[i+1]&&types[i+1]==SYMBOL) { // Declaration
            printf("assigned %s with the value of %s\n",tokens[i+1],tokens[i]);
        }
    }

    return NULL;
}