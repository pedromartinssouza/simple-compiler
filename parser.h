#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"

// Backus-Naur Form (BNF) for the language
// <program> ::= <function>
//      <function> ::= <kind> <identifier> "(" ")" "{" <statement> "}"
//          <kind> ::= "int"
//          <identifier> ::= "main"
//          <statement> ::= "return" <expression> ";"
//              <expression> ::= <term> { ("+" | "-") <term> }
//                  <term> ::= <factor> { ("*" | "/") <factor> }
//                      <factor> ::= "(" <expression> ")" | <UnOp> <factor> | <digit>
//                  <digit> ::= "0" | "1" | ... | "9"

factor *parse_factor(lex_token_list *list_of_tokens, int *index);
term *parse_term(lex_token_list *list_of_tokens, int *index);
expression *parse_expression(lex_token_list *list_of_tokens, int *index);
statement *parse_statement(lex_token_list *list_of_tokens, int *index);
identifier *parse_identifier(lex_token_list *list_of_tokens, int *index);
kind *parse_kind(lex_token_list *list_of_tokens, int *index);
function *parse_function(lex_token_list *list_of_tokens, int *index);
program *parse_program(lex_token_list *list_of_tokens, int *index);

expression *parse_expression(lex_token_list *list_of_tokens, int *index)
{
    int local_index = *index;
    expression *exp = malloc(sizeof(expression));

    expression *exp_list = NULL;
    int exp_list_index = -1;
    int exp_list_size = 0;

    exp->binop = NULL;
    exp->term = NULL;
    exp->is_expression = false;
    term *t = parse_term(list_of_tokens, &local_index);
    if (t->is_term)
    {
        while (list_of_tokens->token_list[local_index].token_type == TOKEN_PLUS || list_of_tokens->token_list[local_index].token_type == TOKEN_NEG)
        {
            exp_list_index++;
            exp_list_size++;

            exp_list = realloc(exp_list, exp_list_size * sizeof(expression));

            term *current_term = t;
            exp_list[exp_list_index].binop = malloc(sizeof(BinOp));

            exp_list[exp_list_index].binop->operation = &list_of_tokens->token_list[local_index];
            local_index++;

            exp_list[exp_list_index].binop->rTerm = parse_term(list_of_tokens, &local_index);
            if(exp_list_size > 1)
            {
                expression temp_exp = exp_list[exp_list_index - 1];
                exp_list[exp_list_index].binop->lTerm->binop = temp_exp.binop;
                exp_list[exp_list_index].binop->lTerm->binop = exp_list[exp_list_index - 1].binop;
            }
        }
        if (!exp_list_size)
            exp->term = t;
        exp = &exp_list[exp_list_index];
        exp->is_expression = true;
        *index = local_index;
        return exp;
    }
    exp->is_expression = false;
    return exp;
}

term *parse_term(lex_token_list *list_of_tokens, int *index)
{
    int local_index = *index;
    term *t = malloc(sizeof(term));
    t->binop = NULL;
    t->factor = NULL;

    factor *f = parse_factor(list_of_tokens, &local_index);
    if (f->is_factor)
    {
        while (list_of_tokens->token_list[local_index].token_type == TOKEN_MULTIPLICATION || list_of_tokens->token_list[local_index].token_type == TOKEN_DIVISION)
        { 
            if (t->binop == NULL)
            {
                t->binop = malloc(sizeof(BinOp));
                t->binop->lFactor = t->binop->rFactor = NULL;
                t->binop->rTerm = malloc(sizeof(term));
                t->binop->rTerm->binop = malloc(sizeof(BinOp));
                t->binop->rTerm->binop->lFactor = malloc(sizeof(factor));
                t->binop->rTerm->binop->rFactor = malloc(sizeof(factor));
            }
            t->binop->lTerm->binop = t->binop;
            t->binop = malloc(sizeof(BinOp));
            t->binop->rTerm = malloc(sizeof(term));
            t->binop->rTerm->binop = malloc(sizeof(BinOp));
            t->binop->lTerm = malloc(sizeof(term));
            t->binop->lTerm->binop = malloc(sizeof(BinOp));

            t->binop->operation = &list_of_tokens->token_list[local_index];
            local_index++;

            factor *next_factor = parse_factor(list_of_tokens, &local_index);
            t->binop->rTerm->binop->lFactor = f;
            t->binop->rTerm->binop->rFactor = next_factor;
        }
        if (t->binop == NULL)
            t->factor = f;

        t->is_term = true;
        *index = local_index;
        return t;
    }
    t->is_term = false;
    return t;
}

factor *parse_factor(lex_token_list *list_of_tokens, int *index)
{
    int local_index = *index;
    factor *f = malloc(sizeof(factor));
    f->expression = NULL;
    f->factor = NULL;
    f->operation = NULL;
    f->is_factor = false;

    // factor -> "(" <expression> ")" | <UnOp> <factor> | <digit>
    if (list_of_tokens->token_list[local_index].token_type == TOKEN_LPAREN)
    {
        local_index++;
        f->expression = parse_expression(list_of_tokens, &local_index);
        if (f->expression->is_expression)
        {
            if (list_of_tokens->token_list[local_index].token_type == TOKEN_RPAREN)
            {
                f->is_factor = true;
                local_index++;
                *index = local_index;
                return f;
            }
        }
    }
    // factor -> <UnOp> <factor>
    else if (is_unary_operator(list_of_tokens->token_list[local_index]))
    {
        local_index++;
        f->factor = parse_factor(list_of_tokens, &local_index);
        if (f->factor->is_factor)
        {
            f->is_factor = true;
            *index = local_index;
            return f;
        }
    }
    // factor -> <int>
    else if (list_of_tokens->token_list[local_index].token_type == TOKEN_NUMBER)
    {
        f->is_factor = true;
        f->value = *list_of_tokens->token_list[local_index].value;
        local_index++;
        *index = local_index;
        return f;
    }
    f->value = '\0';
    f->is_factor = false;
    return f;
}

statement* parse_statement(lex_token_list *list_of_tokens, int *index)
{
    int local_index = *index;
    statement *stmt = malloc(sizeof(statement));
    if (list_of_tokens->token_list[local_index].token_type == TOKEN_RETURN)
    {
        local_index++;
        stmt->expression = parse_expression(list_of_tokens, &local_index);
        if(stmt->expression->is_expression)
        {
            if (list_of_tokens->token_list[local_index].token_type == TOKEN_SEMICOLON)
            {
                local_index++;
                *index = local_index;
                stmt->is_statement = true;
                return stmt;
            }
        }
    }
    stmt->is_statement = false;
    return stmt;
}

identifier* parse_identifier(lex_token_list *list_of_tokens, int *index)
{
    int local_index = *index;
    identifier *id = malloc(sizeof(identifier));
    if (list_of_tokens->token_list[local_index].token_type == TOKEN_MAIN)
    {
        id->name = list_of_tokens->token_list[local_index].value;
        id->is_identifier = true;
        local_index++;
        *index = local_index;
        return id;
    }
    id->is_identifier = false;
    return id;
}

kind* parse_kind(lex_token_list *list_of_tokens, int *index)
{
    int local_index = *index;
    kind *k = malloc(sizeof(kind));
    if (list_of_tokens->token_list[local_index].token_type == TOKEN_INT)
    {
        k->name = list_of_tokens->token_list[local_index].value;
        k->is_kind = true;
        local_index++;
        *index = local_index;
        return k;
    }
    k->is_kind = false;
    return k;
}

function* parse_function(lex_token_list *list_of_tokens, int *index)
{
    int local_index = *index;
    function *func = malloc(sizeof(function));
    func->kind = parse_kind(list_of_tokens, &local_index);
    if (func->kind->is_kind)
    {
        func->identifier = parse_identifier(list_of_tokens, &local_index);
        if (func->identifier->is_identifier)
        {
            if (list_of_tokens->token_list[local_index].token_type == TOKEN_LPAREN)
            {
                local_index++;
                if (list_of_tokens->token_list[local_index].token_type == TOKEN_RPAREN)
                {
                    local_index++;
                    if (list_of_tokens->token_list[local_index].token_type == TOKEN_LBRACE)
                    {
                        local_index++;
                        func->statement = parse_statement(list_of_tokens, &local_index);
                        if (func->statement->is_statement)
                        {
                            if (list_of_tokens->token_list[local_index].token_type == TOKEN_RBRACE)
                            {
                                func->is_function = true;
                                local_index++;
                                *index = local_index;
                                return func;
                            }
                        }
                    }
                }
            }
        }
    }
    func->is_function = false;
    return func;
}

program* parse_program(lex_token_list *list_of_tokens, int *index)
{
    int local_index = *index;
    program *prog = malloc(sizeof(program));
    prog->function = parse_function(list_of_tokens, &local_index);
    if (prog->function->is_function)
    {
        prog->is_program = true;
        *index = local_index;
        return prog;
    }
    prog->is_program = false;
    return prog;
}

#endif