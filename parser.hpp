#pragma once

#include <unordered_set>
#include "node.hpp"
#include "errorHandler.hpp"

namespace snowlang::parser
{
    struct Parser
    {
        const std::vector<Token> &tokens;
        size_t pos = 0;

        Parser(const std::vector<Token> &t_tokens)
            : tokens(t_tokens) {}

        std::unique_ptr<Node> parse();

        inline Token current() { return tokens[pos]; }

        inline void advance(int positions = 1) { pos += positions; }

        inline bool accept(
            TokenType type,
            const std::string &errorMessage = err::NOERR);
        inline bool accept(
            std::unordered_set<TokenType> types,
            const std::string &errorMessage = err::NOERR);

        inline bool typeIs(TokenType type)
        {
            return current().type == type;
        }
        inline bool typeIs(std::unordered_set<TokenType> types)
        {
            return types.count(current().type) > 0;
        }

        inline Token accepted()
        {
            return m_acceptedToken;
        }

    private:
        Token m_acceptedToken;
        std::unique_ptr<Node> script();
        std::unique_ptr<Node> block();
        std::unique_ptr<Node> instruction();
        std::unique_ptr<Node> item();
        std::unique_ptr<Node> expr();
        std::unique_ptr<Node> orExpr();
        std::unique_ptr<Node> equalityExpr();
        std::unique_ptr<Node> inequalityExpr();
        std::unique_ptr<Node> arithExpr();
        std::unique_ptr<Node> term();
        std::unique_ptr<Node> factor();
        std::unique_ptr<Node> atom();

        std::unique_ptr<Node> parseBinOp(
            std::unordered_set<TokenType> types,
            std::function<std::unique_ptr<Node>()> funcLeft,
            std::function<std::unique_ptr<Node>()> funcRight = nullptr);
    };
    std::unique_ptr<Node> parse(const std::vector<Token> &tokens);
}
