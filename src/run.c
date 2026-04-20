#include "kit.h"

struct scope {
    
} root;

int file_store(char* filename);

int run_file(int idx) {
    struct token** temp = files[idx]->tokens-1;
    while (*++temp) {
        switch ((*temp)->type) {
        case KEY:
            if (!strcmp((*temp)->value,"import")) {
                do {
                    if (file_store((*++temp)->value)) {
                        error(*temp,"File %s is not present", (*temp)->value);
                        return 1;
                    }
                    // print("|%s|",(*temp)->value);
                } while ((++temp)[0]->type==OPER&&(temp)[0]->value[0]==',');
            }
            break;
        
        default:
            break;
        }
    }
    return 0;
}