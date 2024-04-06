#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>
#include <stdbool.h>

enum Tokens {
    INVALID_TOKEN = -1,
    TOKEN_INT,
    TOKEN_MAIN,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_RETURN,
    TOKEN_NUMBER,
    TOKEN_SEMICOLON,
    TOKEN_EQUAL,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_GREATER,
    TOKEN_LESS,
};


typedef struct {
    int token_type;
    const char *regex;
    char *value;
} lex_token;

typedef struct {
    lex_token *token_list;
    int size;
} lex_token_list;

lex_token invalid_token = {INVALID_TOKEN, ""};

lex_token single_char_tokens[] = {
    {TOKEN_LPAREN, "(", "("},
    {TOKEN_RPAREN, ")", ")"},
    {TOKEN_LBRACE, "{", "{"},
    {TOKEN_RBRACE, "}", "}"},
    {TOKEN_SEMICOLON, ";", ";"},
    {TOKEN_EQUAL, "=", "="},
    {TOKEN_PLUS, "+", "+"},
    {TOKEN_MINUS, "-", "-"},
    {TOKEN_GREATER, ">", ">"},
    {TOKEN_LESS, "<", "<"},
};

lex_token token_regex_relation[] = {
    {TOKEN_INT, "int", "int"},
    {TOKEN_MAIN, "main", "main"},
    {TOKEN_RETURN, "return", "return"},
    {TOKEN_NUMBER, "[0-9]+", NULL},
};

#endif