#pragma once

#include "snowlang.hpp"
#include "node.hpp"
#include "errorHandler.hpp"

namespace snowlang::parser
{
    struct ParseState
    {
        const std::vector<Token> &tokens;
        int pos = 0;

        void advance(int positions = 1) { pos += positions; }
        Token current() { return tokens[pos]; }

        inline bool typeIs(
            TokenType type,
            bool eat = false,
            const std::string &errorMessage = err::NOERR)
        {
            if (current().type == type)
            {
                if (eat)
                    advance();
                return true;
            }
            if (errorMessage == err::NOERR)
                return false;
            throw err::SnowlangException(
                current(),
                errorMessage);
        }

        inline bool typeIs(
            std::unordered_set<TokenType> types,
            bool eat = false,
            const std::string &errorMessage = err::NOERR)
        {
            if (types.count(current().type) > 0)
            {
                if (eat)
                    advance();
                return true;
            }
            if (errorMessage == err::NOERR)
                return false;
            throw err::SnowlangException(
                current(),
                errorMessage);
        }
    };
    void printAst(std::unique_ptr<Node> &ast,
                  int indent = 0);
    std::unique_ptr<Node> parse(const std::vector<Token> &tokens);
}
