#pragma once

#include <iostream>
#include "token.hpp"

namespace snowlang::lexer
{
    class Lexer
    {
    public:
        Lexer(const std::string &t_text) : text(t_text) {}
        std::vector<Token> lex();

    private:
        const std::string &text;
        size_t pos = 0;
        std::vector<Token> tokens;
        void generateNextToken();
        bool advance(int positions = 1);
        bool accept(char c);
        char peek(int offset = 0);

        void generateNumber();
        void generateIden();
        void generateStrlit();
    };
}
