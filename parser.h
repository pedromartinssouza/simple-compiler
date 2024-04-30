#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"

// Backus-Naur Form (BNF) for the language
// <program> ::= <function>
//      <function> ::= <kind> <identifier> "(" ")" "{" { <statement> } "}"
//          <kind> ::= "int"
//          <identifier> ::= "main"
//          <statement> ::= "return" <expression> ";" 
//                          | <kind> <id> [<number>] [ "=" <expression> ] ";" 
//                          | <expression> ";"
//              <expression> ::= <term> { ("+" | "-") <term> }
//                  <term> ::= <factor> { ("*" | "/") <factor> }
//                      <factor> ::= "(" <expression> ")" | <UnOp> <factor> | <digit> | <id> | <id> = <expression>
//                  <number> ::= <digit> { <digit> }
//                  <digit> ::= "0" | "1" | ... | "9"
//                  <id> ::= "a" | "b" | ... | "z" { "a" | "b" | ... | "z" | "0" | "1" | ... | "9" }
//                  <UnOp> ::= "-" | "!" | "~"

variable_cache cache;

factor *parse_factor(lex_token_list *list_of_tokens, int *index);
term *parse_term(lex_token_list *list_of_tokens, int *index);
expression *parse_expression(lex_token_list *list_of_tokens, int *index);
statement *parse_statement(lex_token_list *list_of_tokens, int *index);
identifier *parse_identifier(lex_token_list *list_of_tokens, int *index);
kind *parse_kind(lex_token_list *list_of_tokens, int *index);
function *parse_function(lex_token_list *list_of_tokens, int *index);
program *parse_program(lex_token_list *list_of_tokens, int *index);

term *copy_term(term *t);
BinOp *copy_binop(BinOp *b);
factor *copy_factor(factor *f);
expression *copy_expression(expression *e);
statement *copy_statement(statement *s);

expression *parse_expression(lex_token_list *list_of_tokens, int *index)
{
    int local_index = *index;
    expression *exp = malloc(sizeof(expression));

    cache.size = 0;
    exp->binop = NULL;
    exp->term = NULL;
    exp->is_expression = false;
    term *t = parse_term(list_of_tokens, &local_index);
    if (t->is_term)
    {
        //region previous_binop
        exp->term = t;
        BinOp *previous_binop = NULL;
        while(
            list_of_tokens->token_list[local_index].token_type == TOKEN_PLUS || 
            list_of_tokens->token_list[local_index].token_type == TOKEN_NEG ||
            list_of_tokens->token_list[local_index].token_type == TOKEN_AND ||
            list_of_tokens->token_list[local_index].token_type == TOKEN_OR ||
            list_of_tokens->token_list[local_index].token_type == TOKEN_EQUAL ||
            list_of_tokens->token_list[local_index].token_type == TOKEN_NOT_EQUAL ||
            list_of_tokens->token_list[local_index].token_type == TOKEN_LESS_THAN ||
            list_of_tokens->token_list[local_index].token_type == TOKEN_LESS_THAN_EQUAL ||
            list_of_tokens->token_list[local_index].token_type == TOKEN_GREATER_THAN ||
            list_of_tokens->token_list[local_index].token_type == TOKEN_GREATER_THAN_EQUAL 
        )
        {
            exp->term = NULL;
            if (exp->binop == NULL)
            {
                exp->binop = malloc(sizeof(BinOp));
                exp->binop->lTerm = malloc(sizeof(term));
                exp->binop->lTerm->binop = malloc(sizeof(BinOp));
                exp->binop->lTerm->binop->lFactor = malloc(sizeof(factor));
                exp->binop->lTerm->binop->rFactor = malloc(sizeof(factor));
                exp->binop->rTerm = malloc(sizeof(term));
                exp->binop->rTerm->binop = malloc(sizeof(BinOp));
                exp->binop->rTerm->binop->lFactor = malloc(sizeof(factor));
                exp->binop->rTerm->binop->rFactor = malloc(sizeof(factor));
            }
            exp->binop = malloc(sizeof(BinOp));
            exp->binop->rTerm = malloc(sizeof(term));
            exp->binop->rTerm->binop = malloc(sizeof(BinOp));
            exp->binop->lTerm = malloc(sizeof(term));
            exp->binop->lTerm->binop = malloc(sizeof(BinOp));

            exp->binop->operation = &list_of_tokens->token_list[local_index];
            local_index++;

            term *next_term = parse_term(list_of_tokens, &local_index);
            exp->binop->rTerm = copy_term(next_term);
            if (previous_binop != NULL)
                exp->binop->lTerm->binop = previous_binop;
            else
                exp->binop->lTerm = copy_term(t);
            previous_binop = malloc(sizeof(BinOp));
            previous_binop->lTerm = malloc(sizeof(term));
            previous_binop->lTerm->binop = malloc(sizeof(BinOp));
            previous_binop->lTerm->binop->lFactor = malloc(sizeof(factor));
            previous_binop->lTerm->binop->rFactor = malloc(sizeof(factor));
            previous_binop->rTerm = malloc(sizeof(term));
            previous_binop->rTerm->binop = malloc(sizeof(BinOp));
            previous_binop->rTerm->binop->lFactor = malloc(sizeof(factor));
            previous_binop->rTerm->binop->rFactor = malloc(sizeof(factor));
            previous_binop = copy_binop(exp->binop);
        }
        exp->is_expression = true;
        *index = local_index;
        return exp;
    }
    exp->is_expression = false;
    variable_cache *temp = &cache;
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
        t->factor = f;
        BinOp *previous_binop = NULL;
        while (list_of_tokens->token_list[local_index].token_type == TOKEN_MULTIPLICATION || list_of_tokens->token_list[local_index].token_type == TOKEN_DIVISION)
        {
            t->factor = NULL;
            t->binop = malloc(sizeof(BinOp));
            t->binop->rTerm = malloc(sizeof(term));
            t->binop->rTerm->binop = NULL;
            t->binop->lTerm = malloc(sizeof(term));
            t->binop->lTerm->binop = NULL;
            
            t->binop->operation = &list_of_tokens->token_list[local_index];
            local_index++;

            factor *next_factor = parse_factor(list_of_tokens, &local_index);
            t->binop->rTerm->factor = copy_factor(next_factor);
            t->binop->rTerm->is_term = true;
            if (previous_binop != NULL)
                t->binop->lTerm->binop = previous_binop;
            else
                t->binop->lTerm->factor = copy_factor(f);
            t->binop->lTerm->is_term = true;
            previous_binop = malloc(sizeof(BinOp));
            previous_binop->lTerm = malloc(sizeof(term));
            previous_binop->lTerm->binop = malloc(sizeof(BinOp));
            previous_binop->lTerm->binop->lFactor = malloc(sizeof(factor));
            previous_binop->lTerm->binop->rFactor = malloc(sizeof(factor));
            previous_binop->rTerm = malloc(sizeof(term));
            previous_binop->rTerm->binop = malloc(sizeof(BinOp));
            previous_binop->rTerm->binop->lFactor = malloc(sizeof(factor));
            previous_binop->rTerm->binop->rFactor = malloc(sizeof(factor));
            previous_binop = copy_binop(t->binop);
        }
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
                f->factor_type = FACTOR_EXPRESSION;
                local_index++;
                *index = local_index;
                return f;
            }
        }
    }
    // factor -> <UnOp> <factor>
    else if (is_unary_operator(list_of_tokens->token_list[local_index]))
    {
        f->operation = &list_of_tokens->token_list[local_index];
        local_index++;
        f->factor = parse_factor(list_of_tokens, &local_index);
        if (f->factor->is_factor)
        {
            f->is_factor = true;
            f->factor_type = FACTOR_UNOP;
            *index = local_index;
            return f;
        }
    }
    // factor -> <digit>
    else if (is_token_regex_int(list_of_tokens->token_list[local_index]))
    {
        f->is_factor = true;
        f->value = *list_of_tokens->token_list[local_index].value;
        f->factor_type = FACTOR_NUMBER;
        local_index++;
        *index = local_index;
        return f;
    }
    // factor -> <id> "=" <expression>
    else if (list_of_tokens->token_list[local_index].token_type == TOKEN_ID)
    {
        f->is_factor = true;
        f->identifier = list_of_tokens->token_list[local_index].value;
        f->factor_type = FACTOR_IDENTIFIER;
        local_index++;
        if (list_of_tokens->token_list[local_index].token_type == TOKEN_ASSIGNMENT)
        {
            local_index++;
            f->expression = parse_expression(list_of_tokens, &local_index);
            if (!f->expression->is_expression)
            {
                f->expression = NULL;
            }
        }
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
                stmt->statement_type = STATEMENT_RETURN;
                return stmt;
            }
        }
    }
    else if (list_of_tokens->token_list[local_index].token_type == TOKEN_INT)
    {
        int var_type = list_of_tokens->token_list[local_index].token_type;
        local_index++;
        if (list_of_tokens->token_list[local_index].token_type == TOKEN_ID)
        {
            stmt->identifier = list_of_tokens->token_list[local_index].value;
            local_index++;
            stmt->statement_type = STATEMENT_DECLARATION;
            if (list_of_tokens->token_list[local_index].token_type == TOKEN_ASSIGNMENT)
            {
                local_index++;
                stmt->expression = parse_expression(list_of_tokens, &local_index);
                if (!stmt->expression->is_expression)
                { 
                    stmt->expression = NULL;
                }
            }
            if (list_of_tokens->token_list[local_index].token_type == TOKEN_SEMICOLON)
            {
                cache.variables = realloc(cache.variables, sizeof(generic_variable_instance) * (cache.size + 1));
                cache.variables[cache.size].name = stmt->identifier;
                cache.variables[cache.size].size = 1;
                switch (var_type)
                {
                case TOKEN_INT:
                    cache.variables[cache.size].var_def = int_def;
                    break;
                default:
                    break;
                }
                cache.size++;

                local_index++;
                *index = local_index;
                stmt->is_statement = true;
                return stmt;
            }
        }
    }
    stmt->expression = parse_expression(list_of_tokens, &local_index);
    if (stmt->expression->is_expression)
    {
        if (list_of_tokens->token_list[local_index].token_type == TOKEN_SEMICOLON)
        {
            local_index++;
            *index = local_index;
            stmt->is_statement = true;
            stmt->statement_type = STATEMENT_EXPRESSION;
            return stmt;
        }
    } else {
        stmt->expression = NULL;
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

bool parse_all_statements(lex_token_list *list_of_tokens, int *index, function *func)
{
    int local_index = *index;
    statement *stmt = parse_statement(list_of_tokens, &local_index);
    if (stmt->is_statement)
    {
        func->statement = realloc(func->statement, sizeof(statement) * (func->statement_count + 1));
        func->statement[func->statement_count] = *stmt;
        func->statement[func->statement_count].is_statement = true;
        func->statement_count++;
        parse_all_statements(list_of_tokens, &local_index, func);
        *index = local_index;
        return true;
    }
    return false;
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
                        func->statement_count = 0;
                        bool has_statements = parse_all_statements(list_of_tokens, &local_index, func);
                        if (has_statements)
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

BinOp *copy_binop(BinOp *b)
{
    BinOp *new_binop = malloc(sizeof(BinOp));
    new_binop->operation = b->operation;
    if (b->lTerm != NULL)
        new_binop->lTerm = copy_term(b->lTerm);
    if (b->rTerm != NULL)
        new_binop->rTerm = copy_term(b->rTerm);
    if (b->lFactor != NULL)
        new_binop->lFactor = b->lFactor;
    if (b->rFactor != NULL)
        new_binop->rFactor = b->rFactor;
    return new_binop;
}

term *copy_term(term *t)
{
    term *new_term = malloc(sizeof(term));
    new_term->binop = NULL;
    new_term->factor = NULL;
    new_term->is_term = NULL;
    if (t != NULL && t->binop != NULL)
    {
        new_term->binop = copy_binop(t->binop);
    }
    else
    {
        new_term->factor = malloc(sizeof(factor));
        new_term->factor = t->factor;
    }
    new_term->is_term = t->is_term;
    return new_term;
}

factor *copy_factor(factor *f)
{
    factor *new_factor = malloc(sizeof(factor));
    new_factor->expression = NULL;
    new_factor->factor = NULL;
    new_factor->operation = NULL;
    new_factor->is_factor = false;
    if (f->expression != NULL)
    {
        new_factor->expression = malloc(sizeof(expression));
        new_factor->expression = copy_expression(f->expression);
    }
    else if (f->factor != NULL)
    {
        new_factor->factor = malloc(sizeof(factor));
        new_factor->factor = copy_factor(f->factor);
    }
    new_factor->value = f->value;
    new_factor->operation = f->operation;
    new_factor->is_factor = true;
    return new_factor;
}

expression *copy_expression(expression *e)
{
    expression *new_expression = malloc(sizeof(expression));
    new_expression->term = NULL;
    new_expression->binop = NULL;
    new_expression->is_expression = false;
    if (e->term != NULL)
    {
        new_expression->term = malloc(sizeof(term));
        new_expression->term = copy_term(e->term);
    }
    else if (e->binop != NULL)
    {
        new_expression->binop = malloc(sizeof(BinOp));
        new_expression->binop = copy_binop(e->binop);
    }
    new_expression->is_expression = e->is_expression;
    return new_expression;
}

#endif