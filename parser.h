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
//              <expression> ::= <digit> { <digit> } | <UnOp> <expression>
//                  <digit> ::= "0" | "1" | ... | "9"

expression *parse_expression(lex_token_list *list_of_tokens, int *index)
{
    int local_index = *index;
    expression *expr = malloc(sizeof(expression));
    expr->token = &list_of_tokens->token_list[*index];
    if (list_of_tokens->token_list[*index].token_type == TOKEN_NUMBER)
    {
        expr->value = *list_of_tokens->token_list[local_index].value;
        expr->is_expression = true;
        local_index++;
        *index = local_index;
        return expr;
    }
    if (is_unary_operator(list_of_tokens->token_list[local_index]))
    {
        expr->value = *list_of_tokens->token_list[local_index].value;
        local_index++;
        expr->expression = parse_expression(list_of_tokens, &local_index);
        if (expr->expression->is_expression)
        {
            expr->is_expression = true;
            *index = local_index;
            return expr;
        }
    }
    expr->is_expression = false;
    return expr;
}

statement* parse_statement(lex_token_list *list_of_tokens, int *index)
{
    int local_index = *index;
    statement *stmt = malloc(sizeof(statement));
    stmt->token = &list_of_tokens->token_list[*index];
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
    id->token = &list_of_tokens->token_list[local_index];
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
    k->token = &list_of_tokens->token_list[local_index];
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
    func->token = &list_of_tokens->token_list[local_index];
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
    prog->token = &list_of_tokens->token_list[local_index];
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