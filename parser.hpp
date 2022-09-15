#include "snowlang.hpp"
#include "node.hpp"

namespace snowlang::parser
{
    class Parser
    {
    public:
        std::vector<Token> tokens;
        int pos;
        void advance(int positions);
        Token peek(int offset);
    };
}
