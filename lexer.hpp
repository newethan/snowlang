#pragma once

#include <iostream>
#include "token.hpp"

namespace snowlang::lexer
{
    struct LexState
    {
        const std::string &text;
        size_t pos = 0;
        inline void advance(int positions = 1) { pos += positions; }
    };
    std::vector<Token> lex(const std::string &text);
}
