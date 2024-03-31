#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h> 

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
    TOKEN_SEMICOLON
};

typedef struct {
    int token_type;
    const char *regex;
} lex_token;

typedef struct {
    lex_token *token_list;
    int size;
} lex_token_list;

lex_token invalid_token = {INVALID_TOKEN, ""};

lex_token single_char_tokens[] = {
    {TOKEN_LPAREN, "("},
    {TOKEN_RPAREN, ")"},
    {TOKEN_LBRACE, "{"},
    {TOKEN_RBRACE, "}"},
    {TOKEN_SEMICOLON, ";"}
};

lex_token token_regex_relation[] = {
    {TOKEN_INT, "int"},
    {TOKEN_MAIN, "main"},
    {TOKEN_RETURN, "return"},
    {TOKEN_NUMBER, "[0-9]+"},
};

lex_token is_single_char_token(char c) {
    for (int i = 0; i < sizeof(single_char_tokens) / sizeof(lex_token); i++) {
        lex_token token = single_char_tokens[i];
        if (c == token.regex[0]) {
            return token;
        }
    }
    return invalid_token;
}

lex_token is_token(char *word, int word_size) {

    lex_token single_char_token = is_single_char_token(word[0]);
    if (single_char_token.token_type != INVALID_TOKEN) {
        return single_char_token;
    }

    char *temp_word = NULL;
    temp_word = malloc(word_size + 1); 
    strcpy(temp_word, word);

    for (int i = 0; i < sizeof(token_regex_relation) / sizeof(lex_token); i++) {
        lex_token token = token_regex_relation[i];
        regex_t regex;
        regcomp(&regex, token.regex, REG_EXTENDED);
        int match = regexec(&regex, temp_word, 0, NULL, 0);
        if (match == 0) {
            return token;
        }
    }

    printf("ERROR: Could not match token for word: %s\n", temp_word);
    exit(1);
}

lex_token_list* iterate_line(char *line, int line_size) {
    lex_token_list *list_of_tokens = malloc(sizeof(lex_token_list));
    list_of_tokens->size = 0;
    list_of_tokens->token_list = NULL;

    char *word = NULL;
    int word_size = 0;
    for (int i = 0; i < line_size; i++) {
        if (line[0] == '/' && line[1] == '/') {
            continue;
        }

        if (line[i] == ' ' || line[i] == '\n' || line[i] == '\r') {
            if (word_size != 0) {
                lex_token token = is_token(word, word_size);
                list_of_tokens->size++;
                list_of_tokens->token_list = realloc(list_of_tokens->token_list, sizeof(lex_token) * list_of_tokens->size);
                list_of_tokens->token_list[list_of_tokens->size - 1] = token;
                word = NULL;
                word_size = 0;
            }
            continue;
        }
        
        if (is_single_char_token(line[i]).token_type != INVALID_TOKEN && word_size != 0) {
            lex_token token = is_token(word, word_size);
            list_of_tokens->size++;
            list_of_tokens->token_list = realloc(list_of_tokens->token_list, sizeof(lex_token) * list_of_tokens->size);
            list_of_tokens->token_list[list_of_tokens->size - 1] = token;
            word = NULL;
            word_size = 0;
        }

        char c = line[i];
        word = realloc(word, sizeof(char) * (word_size + 1));
        word[word_size] = c;
        word_size++;
    }
    return list_of_tokens;
}

lex_token_list *get_tokens_in_file(char *file_name) {
    lex_token_list *tokens_used = malloc(sizeof(lex_token_list));
    tokens_used->size = 0;
    tokens_used->token_list = NULL;

    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        printf("ERROR: Could not open file %s\n", file_name);
        exit(1);
    }

    char c;
    char *line = NULL;
    int line_size = 0;
    while ((c = fgetc(file)) != EOF) {
        line_size++;
        line = realloc(line, sizeof(char) *  line_size);
        line[line_size - 1] = c;

        if (c == '\n') {
            lex_token_list *tokens_in_line;
            tokens_in_line = iterate_line(line, line_size);
            tokens_used->size += tokens_in_line->size;
            tokens_used->token_list = realloc(tokens_used->token_list, sizeof(lex_token) * tokens_used->size);
            for (int i = 0; i < tokens_in_line->size; i++) {
                tokens_used->token_list[tokens_used->size - tokens_in_line->size + i] = tokens_in_line->token_list[i];
            }
            line = NULL;
            line_size = 0;
        }
    }

    fclose(file);
    return tokens_used;
}

int main(void) {
    lex_token_list *tokens_used = get_tokens_in_file("return_2.c");
    printf("RESULT\n[");
    for (int i = 0; i < tokens_used->size; i++) {
        printf("%d", tokens_used->token_list[i].token_type);
        if (i != tokens_used->size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
    return 0;
}