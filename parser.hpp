#pragma once

#include "snowlang.hpp"
#include "node.hpp"

namespace snowlang::parser
{
    class Parser
    {
    public:
        Parser(std::vector<Token> t_tokens);
        std::vector<Token> tokens;
        int pos = 0;
        void advance(int positions);
        static void printAst(std::shared_ptr<Node> ast,
                             int indent = 0);

        // private:
        std::shared_ptr<Node> expression();
        std::shared_ptr<Node> term();
        std::shared_ptr<Node> factor();
    };
}
