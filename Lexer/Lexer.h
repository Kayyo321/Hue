#ifndef HUE_LEXER_H
#define HUE_LEXER_H

#include <cstring>
#include <iostream>
#include <cstdlib>
#include <cstdbool>
#include <cctype>
#include <vector>
#include <string>
#include <memory>

const std::vector<std::string> symbols {
    "int",
    "double",
    "float",
    "char",
    "str",
    "ptr",
    "const",
    "return",
    "inc",
    "sgnl"
};

enum TokenType {
    INT_SYMBOL,
    DOUBLE_SYMBOL,
    FLOAT_SYMBOL,
    CHAR_SYMBOL,
    STR_SYMBOL,
    PTR_SYMBOL,
    CONST_SYMBOL,
    RETURN_SYMBOL,
    INC_SYMBOL,
    SIGNAL_SYMBOL,
    IDENTIFIER,
    CHAR_LITERAL,
    STRING_LITERAL,
    INTEGER_LITERAL,
    MAYBE_DOUBLE,
    DOUBLE_LITERAl,
    PLUS,
    MINUS,
    ASTERISK,
    DIVIDE,
    MOD,
    L_PARAN,
    R_PARAN,
    L_BRACKET,
    R_BRACKET,
    L_CURLY,
    R_CURLY,
    PIPE,
    OR,
    REF,
    AND,
    COMMA,
    PERIOD,
    EQU,
    PLUS_PLUS,
    MINUS_MINUS,
    LT,
    GT,
    LT_EQU,
    GT_EQU,
    L_ARROW,
    R_ARROW,
    PLUS_EQU,
    MINUS_EQU,
    EQU_EQU,
    NOT,
    NOT_EQU,
    SEMICOLON,
    EOL,
    NULL_VAL
};

static const char *tokenTypeStrings[] = {
    "INT_SYMBOL",
    "DOUBLE_SYMBOL",
    "FLOAT_SYMBOL",
    "CHAR_SYMBOL",
    "STR_SYMBOL",
    "PTR_SYMBOL",
    "CONST_SYMBOL",
    "RETURN_SYMBOL",
    "INC_SYMBOL",
    "SIGNAL_SYMBOL",
    "IDENTIFIER",
    "CHARACTER_LITERAL",
    "STRING_LITERAL",
    "INTEGER_LITERAL",
    "MAYBE_DOUBLE",
    "DOUBLE_LITERAl",
    "PLUS",
    "MINUS",
    "ASTERISK",
    "DIVIDE",
    "MOD",
    "L_PARAN",
    "R_PARAN",
    "L_BRACKET",
    "R_BRACKET",
    "L_CURLY",
    "R_CURLY",
    "PIPE",
    "OR",
    "REF",
    "AND",
    "COMMA",
    "PERIOD",
    "EQU",
    "PLUS_PLUS",
    "MINUS_MINUS",
    "LT",
    "GT",
    "LT_EQU",
    "GT_EQU",
    "L_ARROW",
    "R_ARROW",
    "PLUS_EQU",
    "MINUS_EQU",
    "EQU_EQU",
    "NOT",
    "NOT_EQU",
    "SEMICOLON",
    "EOL",
    "NULL_VAL"
};

class Token {
public:
    void PrintToken();

    std::string data {};
    enum TokenType type {NULL_VAL};
    size_t lineNumber {};
};

class Lexer {
public:
    Lexer(char *_data);

    Token * NextToken();

private:
    char Peek();
    bool Eat();

    char *data {};
    char curChar {};
    size_t curPos {}, curLine {1};
};

#endif //HUE_LEXER_H