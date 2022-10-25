#pragma once

#include <unordered_set>
#include "node.hpp"
#include "errorHandler.hpp"

namespace snowlang::parser
{
    const std::unordered_set<TokenType> FIRST_OF_EXPR =
        {TT_INT, TT_FLOAT, TT_IDEN, TT_PLUS, TT_MINUS, TT_LPAREN};

    struct Parser
    {
        const std::vector<Token> &tokens;
        size_t pos = 0;

        Parser(const std::vector<Token> &t_tokens, size_t t_fileIndex = 0)
            : tokens(t_tokens), fileIndex(t_fileIndex) {}

        std::unique_ptr<Node> parse();
        std::unique_ptr<Node> parseInstruction();

    private:
        size_t fileIndex;

        Token m_acceptedToken;

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
        std::unique_ptr<Node> script();
        std::unique_ptr<Node> block();
        std::unique_ptr<Node> instruction();
        std::unique_ptr<Node> construct();
        std::unique_ptr<Node> connect();
        std::unique_ptr<Node> for_loop();
        std::unique_ptr<Node> while_loop();
        std::unique_ptr<Node> if_statement();
        std::unique_ptr<Node> print();
        std::unique_ptr<Node> tick();
        std::unique_ptr<Node> hold();
        std::unique_ptr<Node> assign();
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
