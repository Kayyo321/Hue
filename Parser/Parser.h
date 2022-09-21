#ifndef HUE_PARSER_H
#define HUE_PARSER_H

#include "../Lexer/Lexer.h"
#include <iostream>

enum NodeType {
    FUNC_DEF,
    FUNC_CALL,
    VAR_DEF,
    VAR_CALL,
    CONDITION,
    BODY,
    PARAM,
    PARAM_COUNT,
    ARG,
    TYPE,
    ID,
    OTHER,
    LITERAL,
    BAD_NODE
};

static const char *nodeTypeStrings[] {
    "FUNC_DEF",
    "FUNC_CALL",
    "VAR_DEF",
    "VAR_CALL",
    "CONDITION",
    "BODY",
    "PARAM",
    "PARAM_COUNT",
    "ARG",
    "TYPE",
    "ID",
    "OTHER",
    "LITERAL",
    "BAD_NODE"
};

class Node {
public:
    Node() {}
    Node(Token token) : nodeToken {token} {nodeData = token.data;}

    Node &operator = (Token *token) {
        nodeData = token->data;
        nodeToken = *token;

        return *this;
    }

    Node *NewChild() {
        children.emplace_back();

        return &children.at(children.size() - 1);
    }

    std::string nodeData {};
    enum NodeType nodeType {OTHER};
    Token nodeToken {};
    std::vector<Node> children {};
};

class Parser {
public:
    explicit Parser(const char *_path);

    ~Parser() {
        delete headNode;
    }

    Node *GetAST();

    void PrintTree(Node *headNode);

private:
    void Parse(Node *node, enum TokenType until);

    void ParseDef();
    void ParseFuncDef(Node type, std::string name);
    void ParseParam();
    void ParseBody();
    void ParseVarDef(Node type, std::string name);

    void ParseCall();
    void ParseFuncCall(std::string name);
    void ParseVarCall(std::string name);

    void ParseSignal();

    void ParseInc();

    void ParseRet();

    void ParseCondition();

    bool IsSymbol() {
        return curTok->type <= 9;
    }

    bool IsTypeSymbol() {
        return curTok->type <= 4;
    }

    bool IsOp() {
        return curTok->type >= 34 || curTok->type <= 48;
    }

    void EatTok() {
        ++curPos;
        curTok = &tokens.at(curPos);
    }

    void EatTok(enum TokenType _type) {
        if (curTok->type != _type) {
            std::string msg {"Expected: "}; msg += tokenTypeStrings[_type];
            msg += ". Found: "; msg += tokenTypeStrings[curTok->type]; msg += ".";

            throw std::runtime_error(msg.c_str());
        }

        ++curPos;
        curTok = &tokens.at(curPos);
    }

    bool done {false};

    const char *data {};

    Token *curTok {};
    std::vector<Token> tokens {};
    size_t curPos {};
    Node *headNode {new Node {}}, *curNode {headNode};

    size_t treeIndexAmount {0};
};

#endif //HUE_PARSER_H