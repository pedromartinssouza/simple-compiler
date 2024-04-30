#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>
#include <stdbool.h>

typedef enum Types {
    VARIABLE_INT,
    VARIABLE_CHAR,
    VARIABLE_QUBIT,
} Types;

typedef struct variable_definition {
    Types type;
    char *name;
    int size;
} variable_definition;

const variable_definition int_def = {VARIABLE_INT, "int", 32};
const variable_definition char_def = {VARIABLE_CHAR, "char", 8};
const variable_definition qubit_def = {VARIABLE_QUBIT, "qubit", 1};

typedef struct generic_variable_instance {
    char *name;
    int size;
    variable_definition var_def;
} generic_variable_instance;

typedef struct variable_cache {
    generic_variable_instance *variables;
    int size;
} variable_cache;

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
    TOKEN_ASSIGNMENT,
    TOKEN_GREATER,
    TOKEN_LESS,
    TOKEN_NEG,
    TOKEN_BITWISE_COMPLEMENT,
    TOKEN_NEGATION,
    TOKEN_PLUS,
    TOKEN_MULTIPLICATION,
    TOKEN_DIVISION,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS_THAN,
    TOKEN_LESS_THAN_EQUAL,
    TOKEN_GREATER_THAN,
    TOKEN_GREATER_THAN_EQUAL,
    TOKEN_ID,
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
    {TOKEN_ASSIGNMENT, "=", "="},
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
    {TOKEN_AND, "&&", "&&"},
    {TOKEN_OR, "\\|\\|", "||"},
    {TOKEN_EQUAL, "==", "=="},
    {TOKEN_NOT_EQUAL, "!=", "!="},
    {TOKEN_LESS_THAN, "<", "<"},
    {TOKEN_LESS_THAN_EQUAL, "<=", "<="},
    {TOKEN_GREATER_THAN, ">", ">"},
    {TOKEN_GREATER_THAN_EQUAL, ">=", ">="},
    {TOKEN_ID, "[a-zA-Z_][a-zA-Z_0-9]*", NULL},
};

enum STATEMENT_TYPE {
    STATEMENT_RETURN,
    STATEMENT_DECLARATION,
    STATEMENT_EXPRESSION,
};
enum FACTOR_TYPE {
    FACTOR_EXPRESSION,
    FACTOR_IDENTIFIER,
    FACTOR_NUMBER,
    FACTOR_UNOP,
};


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
    int factor_type;
    char *identifier;
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
    int statement_type;
    char *identifier;
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
    int statement_count;
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

bool is_token_regex_int(lex_token token)
{

    for (int i = 0; i < sizeof(token_regex_relation) / sizeof(lex_token); i++) {
        lex_token reference_token = token_regex_relation[i];
        if (reference_token.token_type != TOKEN_NUMBER)
            continue;

        regex_t regex;
        regcomp(&regex, reference_token.regex, REG_EXTENDED);
        int match = regexec(&regex, token.value, 0, NULL, 0);
        if (match == 0)
            return true;
    }
    return false;
}

#endif