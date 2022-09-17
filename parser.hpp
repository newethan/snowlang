#pragma once

#include "snowlang.hpp"
#include "node.hpp"

namespace snowlang::parser
{
    struct ParseState
    {
        const std::vector<Token> &tokens;
        int pos = 0;
        void advance(int positions = 1) { pos += positions; }
    };
    void printAst(std::unique_ptr<Node> &ast,
                  int indent = 0);
    std::unique_ptr<Node> parse(const std::vector<Token> &tokens);
}
