#pragma once

#include "snowlang.hpp"
#include "token.hpp"

namespace snowlang::lexer
{
    struct LexState
    {
        const std::string &text;
        int pos = 0;
        void advance(int positions = 1) { pos += positions; }
    };
    std::vector<Token> lex(const std::string &text);
}
