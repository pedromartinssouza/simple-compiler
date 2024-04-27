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
    lex_token_list *tokens_used = get_tokens_in_file("return_2.c");
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

    printf("SHOWING THE EXPRESSION\n");
    print_expression(prgm->function->statement->expression);
    printf("\n");
    
    printf("TRANSLATED INTO ASSEMBLY\n");
    translate_program(prgm);


    return 0;
}

void print_binop(BinOp *b, int depth)
{
    if (b->lTerm != NULL)
    {
        printf("(");
        print_term(b->lTerm, depth);
    }
    if (b->lFactor != NULL)
    {
        print_factor(b->lFactor);
    }
    if (b->operation != NULL)
    {
        printf("%s", b->operation->value);
    }
    if (b->rTerm != NULL)
    {
        print_term(b->rTerm, depth);
        printf(")");
    }
    if (b->rFactor != NULL)
    {
        print_factor(b->rFactor);
    }
}
void print_term(term *t, int depth)
{
    if (t->binop != NULL)
    {
        print_binop(t->binop, depth + 1);
    }
    else
    {
        print_factor(t->factor);
    }
}
void print_factor(factor *f)
{
    printf("%c", f->value);
    if (f->operation != NULL)
    {
        printf("%s", f->operation->regex);
    }
    if (f->expression != NULL)
    {
        print_expression(f->expression);
    }
    if (f->factor != NULL)
    {
        print_factor(f->factor);
    }
}
void print_expression(expression *e)
{
    int depth = 0;
    if (e->term != NULL)
    {
        print_term(e->term, depth);
    }
    if (e->binop != NULL)
    {
        print_binop(e->binop, depth);
    }
}
