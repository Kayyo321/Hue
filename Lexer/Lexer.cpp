#include "Lexer.h"

void Token::PrintToken() {
    std::cout << "TOKEN: [\"" << this->data << "\", TYPE: "
        << tokenTypeStrings[this->type] << ", LN: " << this->lineNumber << "]";
}

Lexer::Lexer(char *_data) : data {_data}, curChar {data[0]} {}

char Lexer::Peek() {
    if (this->data[this->curPos + 1] != '\0') {
        return this->data[this->curPos + 1];
    }

    return '\0';
}

bool Lexer::Eat() {
    if (this->Peek() != '\0') {
        this->curChar = this->data[++this->curPos];
        return true;
    }

    this->curChar = '\0';

    return false;
}

Token *Lexer::NextToken() {
    std::unique_ptr<Token> curTok {std::make_unique<Token>()};

ReRun:
    if (isalpha(this->curChar) || this->curChar == '_') {
        curTok->type = IDENTIFIER;

        bool eatenLastChar = true;

        while (isalpha(this->curChar) || this->curChar == '_') {
            if (!eatenLastChar) {
                break;
            }

            curTok->data += this->curChar;

            eatenLastChar = Eat();
        }

        size_t index {};

        for (const std::string &symbol: symbols) {
            if (symbol == curTok->data) {
                curTok->type = TokenType(index);
            }

            ++index;
        }
    }
    else if (isdigit(this->curChar)) {
        if (curTok->type == MAYBE_DOUBLE) {
            curTok->type = DOUBLE_LITERAl;
        }
        else {
            curTok->type = INTEGER_LITERAL;
        }

        bool eatenLastChar = true;

        while (isdigit(this->curChar) || this->curChar == '.') {
            if (!eatenLastChar) {
                break;
            }

            curTok->data += this->curChar;

            if (this->curChar == '.') {
                if (curTok->type != DOUBLE_LITERAl) {
                    curTok->type = DOUBLE_LITERAl;
                }
            }

            eatenLastChar = Eat();
        }
    }
    else {
        if (curTok->type == MAYBE_DOUBLE) {
            curTok->type = PERIOD;
            return curTok.release();
        }

        switch (this->curChar) {
            case '\r':
            case '\t':
            case ' ':
                if (Eat()) {
                    goto ReRun;
                }
                break;

            case '\n':
                curTok->type = EOL;
                curTok->data = "\n";

                ++curLine;

                curTok->lineNumber = curLine;
                break;

            case '+':
                curTok->type = PLUS;
                curTok->data += this->curChar;

                if (Peek() == '+') {
                    curTok->type = PLUS_PLUS;
                    curTok->data += Peek();
                    Eat();
                }
                else if (Peek() == '=') {
                    curTok->type = PLUS_EQU;
                    curTok->data += Peek();
                    Eat();
                }
                break;

            case '-':
                curTok->type = MINUS;
                curTok->data += this->curChar;

                if (Peek() == '=') {
                    curTok->type = MINUS_EQU;
                    curTok->data += Peek();
                    Eat();
                }
                else if (Peek() == '-') {
                    curTok->type = MINUS_MINUS;
                    curTok->data += Peek();
                    Eat();
                }
                else if (Peek() == '>') {
                    curTok->type = R_ARROW;
                    curTok->data += Peek();
                    Eat();
                }
                break;

            case '*':
                curTok->type = ASTERISK;
                curTok->data += this->curChar;
                break;

            case '/':
                curTok->type = DIVIDE;

                if (Peek() == '/') {
                    while (this->curChar != '\n' && this->curChar != '\0') {
                        Eat();
                    }

                    goto ReRun;
                }
                else {
                    curTok->data += this->curChar;
                }
                break;

            case '%':
                curTok->type = MOD;
                curTok->data += this->curChar;
                break;

            case ';':
                curTok->type = SEMICOLON;
                curTok->data += this->curChar;
                break;

            case '"':
                curTok->type = STRING_LITERAL;

                Eat();

                while (this->curChar != '"' && this->curChar != '\0') {
                    curTok->data += this->curChar;
                    Eat();
                }

                break;

            case '\'':
                curTok->type = CHAR_LITERAL;

                Eat();

                curTok->data += this->curChar;

                Eat();

                if (this->curChar != '\'') {
                    curTok->type = NULL_VAL;
                    return curTok.release();
                }
                break;

            case '(':
                curTok->type = L_PARAN;
                curTok->data += this->curChar;
                break;

            case ')':
                curTok->type = R_PARAN;
                curTok->data += this->curChar;
                break;

            case '[':
                curTok->type = L_BRACKET;
                curTok->data += this->curChar;
                break;

            case ']':
                curTok->type = R_BRACKET;
                curTok->data += this->curChar;
                break;

            case '{':
                curTok->type = L_CURLY;
                curTok->data += this->curChar;
                break;

            case '}':
                curTok->type = R_CURLY;
                curTok->data += this->curChar;
                break;

            case '|':
                curTok->type = PIPE;
                curTok->data += this->curChar;

                if (Peek() == '|') {
                    curTok->type = OR;
                    curTok->data += Peek();
                    Eat();
                }
                break;

            case '&':
                curTok->type = REF;
                curTok->data += this->curChar;

                if (Peek() == '&') {
                    curTok->type = AND;
                    curTok->data += Peek();
                    Eat();
                }
                break;

            case ',':
                curTok->type = COMMA;
                curTok->data += this->curChar;
                break;

            case '.':
                curTok->type = MAYBE_DOUBLE;
                curTok->data += this->curChar;

                Eat();

                goto ReRun;

            case '=':
                curTok->type = EQU;
                curTok->data += this->curChar;

                if (Peek() == '=') {
                    curTok->type = EQU_EQU;
                    curTok->data += Peek();
                    Eat();
                }
                break;

            case '<':
                curTok->type = LT;
                curTok->data += this->curChar;

                if (Peek() == '=') {
                    curTok->type = LT_EQU;
                    curTok->data += Peek();
                    Eat();
                }
                else if (Peek() == '-') {
                    curTok->type = L_ARROW;
                    curTok->data += Peek();
                    Eat();
                }
                break;

            case '>':
                curTok->type = GT;
                curTok->data += this->curChar;

                if (Peek() == '=') {
                    curTok->type = GT_EQU;
                    curTok->data += Peek();
                    Eat();
                }
                break;

            case '!':
                curTok->type = NOT;
                curTok->data += this->curChar;

                if (Peek() == '=') {
                    curTok->type = NOT_EQU;
                    curTok->data += Peek();
                    Eat();
                }
                break;

            default: break;
        }

        Eat();

        return curTok.release();
    }

    return curTok.release();
}