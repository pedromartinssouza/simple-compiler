#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "translator.h"


int main(void) {
    lex_token_list *tokens_used = get_tokens_in_file("return_2.c");
    printf("TOKENS IDENTIFIED BY LEXER\n[");
    for (int i = 0; i < tokens_used->size; i++) {
        printf("%d", tokens_used->token_list[i].token_type);
        if (i != tokens_used->size - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    int *index = malloc(sizeof(int));
    *index = 0;

    program* prgm = parse_program(tokens_used, index);
    printf("PARSER DIAGNOSTIC OF THE PROGRAM\n");
    if (prgm->is_program) {
        translate_program(prgm);
    } else {
        printf("It's not a program\n");
    }
    
    return 0;
}