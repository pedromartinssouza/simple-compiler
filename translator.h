#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"

// Backus-Naur Form (BNF) for the language
// <program> ::= <function>
//      <function> ::= <kind> <identifier> "(" ")" "{" <statement> "}"
//          <kind> ::= "int"
//          <identifier> ::= "main"
//          <statement> ::= "return" <expression> ";"
//              <expression> ::= <digit> { <digit> }
//                  <digit> ::= "0" | "1" | ... | "9"

// Corresponding assembly code
// .globl _<identifier>
// _<identifier>:
//      movl $<expression>, %eax
//      ret

// function prototypes
void translate_program(program *prg);
void translate_function(function *func);
void translate_statement(statement *stmt);
void translate_expression(expression *expr);
void translate_term(term *t);
void translate_factor(factor *f);

void translate_program(program *prg) {
    translate_function(prg->function);
}

void translate_function(function *func) {
    printf(".globl _%s\n", func->identifier->name);
    printf("_%s:\n", func->identifier->name);
    translate_statement(func->statement);
}

void translate_statement(statement *stmt) {
    translate_expression(stmt->expression);
    printf("ret\n");
}

void translate_expression(expression *expr) {
    if (expr->term != NULL) {
        translate_term(expr->term);
    } else if (expr->binop != NULL) {
        if (expr->binop->lTerm != NULL) {
            translate_term(expr->binop->lTerm);
        }
        if (expr->binop->rTerm != NULL) {
            translate_term(expr->binop->rTerm);
        }
    }
}

void translate_term(term *t) {
    if (t->factor != NULL) {
        translate_factor(t->factor);
    } else if (t->binop != NULL) {
        if (t->binop->lTerm != NULL) {
            translate_term(t->binop->lTerm);
        }
        printf("push %%eax\n");
        if (t->binop->rTerm != NULL) {
            translate_term(t->binop->rTerm);
        }
        printf("pop %%ecx\n");
        if (t->binop->operation->token_type == TOKEN_PLUS) {
            printf("addl %%ecx, %%eax\n");
        } else if (t->binop->operation->token_type == TOKEN_NEG) {
            printf("subl %%ecx, %%eax\n");
        } else if (t->binop->operation->token_type == TOKEN_MULTIPLICATION) {
            printf("imul %%ecx, %%eax\n");
        } else if (t->binop->operation->token_type == TOKEN_DIVISION) {
            printf("cdq\n");
            printf("idivl %%ecx\n");
        }
    }
}

void translate_factor(factor *f) {
    if (f->expression != NULL) {
        translate_expression(f->expression);
    } 
    else if (f->operation != NULL) {
        if (f->operation->token_type == TOKEN_NEG) {
            if (f->factor != NULL) {
                translate_factor(f->factor);
            }
            printf("negl %%eax\n");
        } else if (f->operation->token_type == TOKEN_BITWISE_COMPLEMENT) {
            if (f->factor != NULL) {
                translate_factor(f->factor);
            }
            printf("notl %%eax\n");
        } else if (f->operation->token_type == TOKEN_NEGATION) {
            if (f->factor != NULL) {
                translate_factor(f->factor);
            }
            printf("cmpl $0, %%eax\n");
            printf("movl $0, %%eax\n");
            printf("sete %%al\n");
        }
    }
    else {
            printf("movl $%c, %%eax\n", f->value);
    }
}

#endif