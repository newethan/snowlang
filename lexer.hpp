#pragma once

#include "snowlang.hpp"
#include "token.hpp"

namespace snowlang::lexer
{
    class Lexer
    {
    public:
        Lexer(std::string t_text, std::string t_filename);
        std::vector<Token> tokens;

    private:
        std::string text;
        std::string filename;
        void generateTokens();
        Token generateNextToken(int &textPos);
    };
}
