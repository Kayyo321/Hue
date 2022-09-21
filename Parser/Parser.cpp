#include "Parser.h"

Parser::Parser(const char *data) : data {data} {
    Lexer *lexer {new Lexer((char*)data)};
    size_t tokIndex {};

    headNode->nodeType = OTHER;

    curTok = lexer->NextToken();

    while (curTok->type != NULL_VAL) {
        tokens.push_back(std::move(*curTok));

        delete curTok;

        curTok = lexer->NextToken();

        ++tokIndex;
    }

    delete curTok;

    tokens.emplace_back(); // NULL_VAL token added to back.

    delete lexer;
    curTok = &tokens.at(0);
}

Node *Parser::GetAST() {
    headNode->nodeData = "Start";
    curNode = headNode;

    Parse(curNode, NULL_VAL);

    return headNode;
}

void Parser::Parse(Node *node, enum TokenType until) {
    Node &tmp {*curNode};
    curNode = node;

    while (!done && curTok->type != until) {
        if (IsSymbol()) {
            if (IsTypeSymbol()) {
                ParseDef();
            }
            else {
                switch (curTok->type) {
                    case INC_SYMBOL:
                        ParseInc();
                        break;

                    case SIGNAL_SYMBOL:
                        ParseSignal();
                        break;

                    case RETURN_SYMBOL:
                        ParseRet();
                        break;

                    default: break;
                }
            }
        }
        else if (IsOp()) {

        }
        else if (IsCon()) {

        }
        else {
            switch (curTok->type) {
                case IDENTIFIER:
                    ParseCall();
                    break;

                case STRING_LITERAL:
                case INTEGER_LITERAL:
                case DOUBLE_LITERAl:
                case CHAR_LITERAL: {
                    Node *lit {curNode->NewChild()};

                    lit->nodeToken = *curTok;
                    lit->nodeData = curTok->data;
                    lit->nodeType = LITERAL;

                    EatTok();
                }
                    break;

                case NULL_VAL:
                    done = true;
                    break;

                case EOL:
                case SEMICOLON:
                    EatTok();
                    break;

                case L_CURLY:
                    ParseBody();
                    break;

                default:
                    if (curTok->type == COMMA) { // Ignore commas, just trust me.
                        EatTok();
                        break;
                    }

                    Node *child {curNode->NewChild()};
                    child->nodeType = OTHER;
                    child->nodeData = curTok->data;
                    child->nodeToken = *curTok;

                    EatTok();
                    break;
            }
        }
    }

    curNode = &tmp;
}

void Parser::ParseDef() {
    Node type {};
    type.nodeData = curTok->data;
    type.nodeType = TYPE;
    type.nodeToken = *curTok;

    EatTok();

    while (curTok->type != IDENTIFIER || curTok->type == CONST_SYMBOL || curTok->type == PTR_SYMBOL) {
        *curNode->NewChild() = Node {*curTok};
        EatTok();
    }

    const std::string name = curTok->data;

    EatTok();

    if (curTok->type == L_PARAN) {
        ParseFuncDef(type, name);
    }
    else {
        ParseVarDef(type, name);
    }
}

void Parser::ParseFuncDef(Node type, std::string name) {
    Node *functionNode {curNode->NewChild()};

    functionNode->nodeType = FUNC_DEF;
    functionNode->nodeData = name;

    functionNode->children.push_back(type);

    curNode = functionNode->NewChild();
    curNode->nodeData = "params";
    curNode->nodeType = PARAM;

    ParseParam();

    curNode = functionNode;

    ParseBody();
}

void Parser::ParseParam() {
    EatTok(L_PARAN); // Skip '('.

    size_t paramCount {};

    while (curTok->type != R_PARAN) {
        Node *param {curNode->NewChild()};

        if (IsTypeSymbol()) {
            param->nodeData = "paramdef" + std::to_string(paramCount);
            param->nodeType = VAR_DEF;

            *param->NewChild() = Node {*curTok};
            param->children.at(0).nodeType = TYPE;
            param->children.at(0).nodeToken = *curTok;

            EatTok();

            if (curTok->type == IDENTIFIER) {
                Node *name {param->NewChild()};
                name->nodeType = ID;
                name->nodeData = curTok->data;
                name->nodeToken = *curTok;

                EatTok();

                while (curTok->type != R_PARAN && curTok->type != COMMA) {
                    Node *child {name->NewChild()};
                    child->nodeType = OTHER;
                    child->nodeData = curTok->data;
                    child->nodeToken = *curTok;

                    if (PeekTok().type != R_PARAN && PeekTok().type != COMMA) {
                        EatTok();
                    }
                    else {
                        break;
                    }
                }
            }
            else {
                param->nodeData = "badparam";
                param->nodeType = BAD_NODE;
                break;
            }
        }
        else {
            param->nodeData = "badparam";
            param->nodeType = BAD_NODE;
            break;
        }

        if (curTok->type != R_PARAN) {
            EatTok();
        }

        if (curTok->type == COMMA) {
            ++paramCount;
            EatTok();
        }
    }

    EatTok(R_PARAN); // Skip ')'.

    Node *paramCountNode {curNode->NewChild()};
    paramCountNode->nodeType = PARAM_COUNT;
    paramCountNode->nodeData = std::to_string(
        (paramCount == 0)
            ? paramCount
            : paramCount + 1
    );
}

void Parser::ParseBody() {
    Node *body {curNode->NewChild()};
    body->nodeType = BODY;
    body->nodeData = "body";

    EatTok(L_CURLY); // Skip '{'.

    Parse(body, R_CURLY);

    EatTok(R_CURLY);
}

void Parser::ParseVarDef(Node type, std::string name) {
    Node *varNode {curNode->NewChild()};
    varNode->nodeData = name;
    varNode->nodeType = VAR_DEF;

    while (curTok->type != EQU) {
        Node *tmp {varNode->NewChild()};
        tmp->nodeType = OTHER;
        tmp->nodeData = curTok->data;
        tmp->nodeToken = *curTok;

        EatTok();
    }

    EatTok();

    Node *valueNode {varNode->NewChild()};
    valueNode->nodeType = OTHER;
    valueNode->nodeData = "value";

    Parse(valueNode, SEMICOLON);
}

void Parser::ParseCall() {
    std::string name {curTok->data};

    EatTok();

    if (curTok->type == L_PARAN) {
        ParseFuncCall(name);
    }
    else {
        ParseVarCall(name);
    }
}

void Parser::ParseFuncCall(std::string name) {
    Node *functionNode {curNode->NewChild()};
    functionNode->nodeType = FUNC_CALL;
    functionNode->nodeData = name;

    Parse(functionNode, L_PARAN);

    EatTok();

    Node *args {functionNode->NewChild()};
    args->nodeData = "args";
    args->nodeType = ARG;

    Parse(args, R_PARAN);

    Node *argCountNode {args->NewChild()};
    argCountNode->nodeType = ARG_COUNT;
    argCountNode->nodeData = std::to_string(args->children.size());

    EatTok();
}

void Parser::ParseVarCall(std::string name) {
    Node *varNode {curNode->NewChild()};
    varNode->nodeType = VAR_CALL;
    varNode->nodeData = name;

    Parse(varNode, SEMICOLON);
}

void Parser::ParseSignal() {
    Node *sgnlNode {curNode->NewChild()};
    sgnlNode->nodeType = ID;
    sgnlNode->nodeData = "sgnl";
    sgnlNode->nodeToken = *curTok;

    EatTok();

    while (curTok->type != EOL) {
        Node tmp {};
        tmp.nodeData = curTok->data;
        tmp.nodeType = OTHER;
        tmp.nodeToken = *curTok;

        sgnlNode->children.push_back(tmp);

        EatTok();
    }

    EatTok();
}

void Parser::ParseInc() {
    Node *incNode {curNode->NewChild()};
    incNode->nodeType = ID;
    incNode->nodeData = "inc";
    incNode->nodeToken = *curTok;

    EatTok();

    while (curTok->type != EOL) {
        Node tmp {};
        tmp.nodeData = curTok->data;
        tmp.nodeType = OTHER;
        tmp.nodeToken = *curTok;

        incNode->children.push_back(tmp);

        EatTok();
    }

    EatTok();
}

void Parser::ParseRet() {
    Node *retNode {curNode->NewChild()};
    retNode->nodeType = ID;
    retNode->nodeData = "ret";
    retNode->nodeToken = *curTok;

    EatTok();

    Parse(retNode, SEMICOLON);

    EatTok();
}

// int x = 1 + 2 * 3;
//           ^
//TODO: Create ParseOperation function.
void Parser::ParseOperation() {

}

// int b = x == y && a == c;
//           ^^
//TODO: Create ParseCondition function.
void Parser::ParseCondition() {

}

void Parser::PrintTree(Node *headNode) {
    for (size_t i {0}; i < treeIndexAmount; ++i) {
        printf("  ");
    }

    std::cout << "(\"" << headNode->nodeData << "\", "
        << nodeTypeStrings[headNode->nodeType] << ", ";

    headNode->nodeToken.PrintToken();

    printf(")");

    if (headNode->children.size() > 0) {
        printf(" {\n");

        ++treeIndexAmount;

        for (Node &n: headNode->children) {
            PrintTree(&n);
        }

        --treeIndexAmount;

        for (size_t i{0}; i < treeIndexAmount; ++i) {
            printf("  ");
        }

        printf("}");
    }

    printf("\n");
}