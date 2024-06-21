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

generic_variable_instance find_variable_in_cache(variable_cache cache, char* name) {
    for (int i = 0; i < cache.size; i++) {
        if (strcmp(cache.variables[i].name, name) == 0) {
            return cache.variables[i];
        }
    }
    return (generic_variable_instance) {NULL, 0, void_def};
}

void q_print_bit_declaration(char* var, int size)
{
    if(size)
        printf("bit[%d] %s;\n", size, var);
    else
        printf("bit %s;\n", var);
}

void q_print_qubit_declaration(char* var, int size)
{
    if(size)
        printf("qubit[%d] %s;\n", size, var);
    else
        printf("qubit %s;\n", var);
}

void q_print_hadamard(char* var)
{
    printf("h %s;\n", var);
}

void q_print_cphase(char* var1, char *pos1, char* var2, char *pos2, char* phase)
{
    printf("cphase(%s) %s[%s], %s[%s];\n", phase, var1, pos1, var2, pos2);
}

void q_print_measure(char* qvar, char* cvar)
{
    printf("measure %s -> %s;\n", qvar, cvar);
}

void q_print_barrier()
{
    printf("barrier;\n");
}

variable_cache variables;

// function prototypes
void translate_program(program *prg);
void translate_function(function *func);
void translate_statement(statement *stmt);
void translate_expression(expression *expr);
void translate_term(term *t);
void translate_factor(factor *f);

void translate_program(program *prg) {
    variables = prg->variables;
    translate_function(prg->function);
}

void translate_function(function *func) {
    printf(".globl _%s\n", func->identifier->name);
    printf("_%s:\n", func->identifier->name);

    for (int i = 0; i < func->statement_count; i++)
        translate_statement(func->statement + i);
}

void translate_statement(statement *stmt) {
    if (stmt->statement_type == STATEMENT_RETURN) {
        translate_expression(stmt->expression);
        printf("ret\n");
    }
    if (stmt->statement_type == STATEMENT_BARRIER) {
        printf("barrier;\n");
    }
    if (stmt->statement_type == STATEMENT_DECLARATION) {
        generic_variable_instance var = find_variable_in_cache(variables, stmt->identifier);
        if (var.var_def.type == VARIABLE_BIT) {
            q_print_bit_declaration(var.name, var.size);
        } else if (var.var_def.type == VARIABLE_QUBIT) {
            q_print_qubit_declaration(var.name, var.size);
        }
    }
    if (stmt->statement_type == STATEMENT_EXPRESSION) {
        translate_expression(stmt->expression);
    }
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
        if (t->binop->operation->token_type == TOKEN_CPHASE)
        {
            q_print_cphase(
                t->binop->lFactor->identifier, 
                t->binop->lFactor->position, 
                t->binop->rFactor->identifier,
                t->binop->rFactor->position,
                t->binop->operation->value);
        }
        else {
            if (t->binop->operation->token_type == TOKEN_PLUS) {
                printf("movl $0, %%eax\n");
            }
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
}

void translate_factor(factor *f) {
    if (f->expression != NULL) {
        if (f->factor_type == FACTOR_IDENTIFIER) {
            generic_variable_instance lVar = find_variable_in_cache(variables, f->identifier);
            if (lVar.var_def.type == VARIABLE_BIT) {
                if (f->expression->term->factor->factor_type == FACTOR_IDENTIFIER && f->expression->term->factor->expression == NULL) {
                    generic_variable_instance rVar = find_variable_in_cache(variables, f->expression->term->factor->identifier);
                    if (rVar.var_def.type == VARIABLE_QUBIT) {
                        q_print_measure(rVar.name, lVar.name);
                    }
                }
            }
        }
        else {
            translate_expression(f->expression);
        }
        
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
        } else if (f->operation->token_type == TOKEN_HADAMARD) {
            printf("h %s[%s];\n", f->factor->identifier, f->factor->position);
        } else if (f->operation->token_type == TOKEN_CPHASE)
        {
            q_print_cphase(
                f->factor->identifier, 
                f->factor->position, 
                f->factor->expression->term->factor->identifier,
                f->factor->expression->term->factor->position,
                f->operation->value);
        }
    }
    else if (f->factor_type == FACTOR_NUMBER) {
        printf("movl $%c, %%eax\n", f->value);
    }
    else {
            printf("movl $%c, %%eax\n", f->value);
    }
}

#endif