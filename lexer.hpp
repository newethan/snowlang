#include "snowlang.hpp"
#include "token.hpp"

namespace snowlang::lexer
{
    class Lexer
    {
    public:
        Lexer(std::string t_text, std::string t_filename);

    private:
        std::string text;
        std::string filename;
        std::vector<Token> tokens;
        void generateTokens();
        Token generateNextToken(int &textPos);
    };
}
