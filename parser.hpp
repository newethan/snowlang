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

        ParseState(const std::vector<Token> &t_tokens)
            : tokens(t_tokens) {}

        inline void advance(int positions = 1) { pos += positions; }
        inline Token current() { return tokens[pos]; }

        inline bool accept(
            TokenType type,
            const std::string &errorMessage = err::NOERR);
        inline bool accept(
            std::unordered_set<TokenType> types,
            const std::string &errorMessage = err::NOERR);

        inline Token accepted()
        {
            return m_acceptedToken;
        }

    private:
        Token m_acceptedToken;
    };
    std::unique_ptr<Node> parse(const std::vector<Token> &tokens);
}
