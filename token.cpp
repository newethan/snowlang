#include "token.hpp"

using namespace std;

namespace snowlang
{

    Token::Token(enum TokenType t_type, string t_value, int t_tokenStart, int t_tokenEnd)
        : type(t_type), value(t_value), tokenStart(t_tokenStart), tokenEnd(t_tokenEnd) {}

    string Token::reprTokenType(enum TokenType tokenType)
    {
        const char *reprs[] = {
            "TT_NULL",
            "TT_EOF",
            "TT_WHITESPACE",
            "TT_COMMENT",
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
            "TT_POW",
            "TT_MULT",
            "TT_DIV",
            "TT_REM",
            "TT_MOD",
            "TT_PRINT",
            "TT_TICK",
            "TT_HOLD"
            "TT_LET",
            "TT_CON",
            "TT_ASSIGN",
            "TT_IF",
            "TT_ELIF",
            "TT_ELSE",
            "TT_FOR",
            "TT_WHILE",
            "TT_BREAK",
            "TT_CONTINUE",
            "TT_RETURN",
            "TT_IN",
            "TT_OR",
            "TT_AND",
            "TT_NOT",
            "TT_GT",
            "TT_GE",
            "TT_EQ",
            "TT_NEQ",
            "TT_LE",
            "TT_LT",
            "TT_FLOAT",
            "TT_INT",
            "TT_IDEN",
            "TT_STRLIT"};
        return reprs[tokenType];
    }

    string Token::repr()
    {
        return (reprTokenType(type) + ": '" + value + "'");
    }

}