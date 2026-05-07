int parse_fd(FILE* fd) {
    static char check;
    if (check||check++) goto skip;
    lookup(spaces, " \t\n\r\v\f");
    lookup(equal_oper, "+-*/%%!><&|^");
    lookup(double_oper, "+-=><&|");
    lookup(delimiters, " \t\n\r\v\f,{}[]()+-*/%%=!><&|^~.\"\'");
    lookup(operators, "+-*/%%=!><&|^~.{[(,");
    skip:
    // static unsigned char keywords[32] = {0}; for (int i = 0; i < 128; i++) if (!isalpha(i) && !isdigit(i) && i != '_') bitset(keywords, i); flip(keywords);
    char* bytes = 0;
    char c;
    while ((c = getc(fd))!=EOF) {
        // if (c=='\n') {stack_block(NEWLINE, NULL); continue;} else 
        if (bitget(spaces,c)) continue;
        else if (c=='#') { //Skip comments
            while ((c = getc(fd))!=EOF&&c!='\n');
            // stack_block(NEWLINE, NULL);
            continue;
        }
        else str_append(&bytes,c);

        size_t s_line=line, s_column=column;

        if (c=='\''||c=='\"') {
            char cc=c, p=0;
            while ((c = getc(fd))!=EOF&&!(c==cc&&p!='\\')) {
                str_append(&bytes,c);
                p = c;
            }
            goto skip1;
        }
        else if (bitget(operators,c)) {
            char c2=getc(fd), c3=getc(fd), count=0;
            if (c2==EOF||c3==EOF) {
                if (c2==EOF) break;
                if (c3==EOF) count=bitget(operators,c2);
            } else {
                char cc = bitget(operators,c2);
                count = cc+(cc&&bitget(operators,c3));
            }
            switch (count) {
            case 2:
                if ((c=='<'||c=='>')&&c==c2&&c3=='=') {
                    str_append(&bytes,c2);
                    str_append(&bytes,c3);
                    break;
                } else {
                    char** temp = files;
                    while (*++temp);
                    error_message(*(--temp), s_line, s_column, 3, "error: unknown operator");
                    free(bytes);
                    return 1;
                }

            case 1:
                c3 = (ungetc(c3, fd),EOF);
                if (bitget(equal_oper,c)&&c2=='=') {
                    str_append(&bytes,c2);
                    break;
                } else if (bitget(double_oper,c)&&c==c2) {
                    str_append(&bytes,c2);
                    break;
                } else {
                    char** temp = files;
                    while (*++temp);
                    error_message(*(--temp), s_line, s_column, 3, "error: unknown operator");
                    free(bytes);
                    return 1;
                }

            case 0:
                ungetc(c3, fd);
                ungetc(c2, fd);
                break;
            }
            goto skip1;
        }

        while ((c = getc(fd))!=EOF&&!bitget(delimiters,c)) str_append(&bytes,c); ungetc(c, fd);
        skip1:

        bytes = auto_free(bytes);

        char* error = handle_token(&bytes);

        if (error) {
            char** temp = files;
            while (*++temp);
            error_message(*(--temp), s_line, s_column, strlen(bytes), error);
            free(bytes);
            free(error);
            return 1;
        }

        // printf("%s ",bytes?bytes:"");
        // free(bytes);
        bytes = 0;

        if (c==EOF) break;
    }
    // printf("%s\n",bytes?bytes:"");
    handle_token(NULL);
    free(bytes);
    
    return 0;
}