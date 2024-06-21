#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h" 
#include "translator.h"


void print_expression(expression *e);
void print_factor(factor *f);
void print_term(term *t, int depth);
void print_binop(BinOp *b, int depth);

int main(void) {
    lex_token_list *tokens_used = get_tokens_in_file("fourier.quc");
    printf("TOKENS IDENTIFIED BY LEXER\n");
    for (int i = 0; i < tokens_used->size; i++) {
        printf("\t%s", tokens_used->token_list[i].value);
        if (i != tokens_used->size - 1) {
            printf("\n");
        }
    }
    printf("\n");

    int *index = malloc(sizeof(int));
    *index = 0;

    program* prgm = parse_program(tokens_used, index);
    printf("PARSER DIAGNOSTIC OF THE PROGRAM\n");
    if (prgm->is_program) {
        printf("It's a program\n");
    } else {
        printf("It's not a program\n");
    }

    printf("TRANSLATOR DIAGNOSTIC OF THE PROGRAM\n");
    translate_program(prgm);
    return 0;
}
