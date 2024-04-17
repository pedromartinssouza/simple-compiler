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
//    if (is_unary_operator(*expr->token)) {
//        if (expr->token->token_type == TOKEN_NEG) {
//            translate_expression(expr->expression);
//            printf("negl %%eax\n");
//        } else if (expr->token->token_type == TOKEN_BITWISE_COMPLEMENT) {
//            translate_expression(expr->expression);
//            printf("notl %%eax\n");
//        } else if (expr->token->token_type == TOKEN_NEGATION) {
//            translate_expression(expr->expression);
//            printf("cmpl $0, %%eax\n");
//            printf("movl $0, %%eax\n");
//            printf("sete %%al\n");
//        }
//    } else {
//        printf("movl $%c, %%eax\n", expr->value);
//    }
}

#endif