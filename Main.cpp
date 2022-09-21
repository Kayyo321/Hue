#include <iostream>
#include <fstream>

#include "Parser/Parser.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Argument count cannot be lower than 2\n");
        return 1;
    }

    std::ifstream t (argv[1]);
    std::string file ((std::istreambuf_iterator<char>(t)),
                       std::istreambuf_iterator<char>());

    Parser parser {file.c_str()};
    Node *headNode {parser.GetAST()};

    parser.PrintTree(headNode);

    return 0;
}