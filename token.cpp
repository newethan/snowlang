#include "snowlang.hpp"
#include "token.hpp"

using namespace std;

namespace snowlang
{

    Token::Token(enum TokenType t_type, string t_value, int t_tokenStart, int t_tokenEnd)
        : type(t_type), value(t_value), tokenStart(t_tokenStart), tokenEnd(t_tokenEnd) {}

    string Token::reprTokenType(enum TokenType tokenType)
    {
        return (const char *[]){
            "TT_NULL",
            "TT_WHITESPACE",
            "TT_LBRACE",
            "TT_RBRACE",
            "TT_LBRACK",
            "TT_RBRACK",
            "TT_LPAREN",
            "TT_RPAREN",
            "TT_SEMICOLON",
            "TT_COMMA",
            "TT_PERIOD",
            "TT_PLUS",
            "TT_MINUS",
            "TT_MULT",
            "TT_DIV",
            "TT_MOD",
            "TT_WIRE",
            "TT_INPUT",
            "TT_OUTPUT",
            "TT_DEFINE",
            "TT_CONNECT",
            "TT_LOOP",
            "TT_VAR",
            "TT_IDEN",
            "TT_INT"}[tokenType];
    }

    string Token::repr()
    {
        return ("Token('" + value + "' : " + reprTokenType(type) + ")");
    }

}