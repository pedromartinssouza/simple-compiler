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
    TOKEN_GREATER,
    TOKEN_LESS,
    TOKEN_NEG,
    TOKEN_BITWISE_COMPLEMENT,
    TOKEN_NEGATION,
    TOKEN_PLUS,
    TOKEN_MULTIPLICATION,
    TOKEN_DIVISION,
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
    {TOKEN_NEG, "-", "-"},
    {TOKEN_GREATER, ">", ">"},
    {TOKEN_LESS, "<", "<"},
    {TOKEN_BITWISE_COMPLEMENT, "~", "~"},
    {TOKEN_NEGATION, "!", "!"},
    {TOKEN_MULTIPLICATION, "*", "*"},
    {TOKEN_DIVISION, "/", "/"},
};

lex_token token_regex_relation[] = {
    {TOKEN_INT, "int", "int"},
    {TOKEN_MAIN, "main", "main"},
    {TOKEN_RETURN, "return", "return"},
    {TOKEN_NUMBER, "[0-9]+", NULL},
};
// Backus-Naur Form (BNF) for the language
// <program> ::= <function>
//      <function> ::= <kind> <identifier> "(" ")" "{" <statement> "}"
//          <kind> ::= "int"
//          <identifier> ::= "main"
//          <statement> ::= "return" <expression> ";"
//              <expression> ::= <digit> { <digit> } | <UnOp> <expression>
//                  <digit> ::= "0" | "1" | ... | "9"

// Data structures for the language


typedef struct BinOp{
    lex_token *operation;
    struct term *lTerm;
    struct term *rTerm;
    struct factor *lFactor;
    struct factor *rFactor;
} BinOp;

typedef struct UnOp{
    lex_token *operation;
} UnOp;

typedef struct factor {
    char value;
    struct expression *expression;
    struct factor *factor;
    lex_token *operation;
    bool is_factor;
} factor;
typedef struct term {
    struct factor *factor;
    BinOp *binop;
    bool is_term;
} term;
typedef struct expression {
    term *term;
    BinOp *binop;
    bool is_expression;
} expression;
typedef struct statement {
    struct expression *expression;
    bool is_statement;
} statement;
typedef struct identifier {
    char *name;
    bool is_identifier;
} identifier;
typedef struct kind {
    char *name;
    bool is_kind;
} kind;
typedef struct function {
    struct kind *kind;
    struct identifier *identifier;
    struct statement *statement;
    bool is_function;
} function;
typedef struct program{
    struct function *function;
    bool is_program;
} program;

bool is_unary_operator(lex_token token)
{
    return token.token_type == TOKEN_NEG
        || token.token_type == TOKEN_BITWISE_COMPLEMENT
        || token.token_type == TOKEN_NEGATION;
}

#endif