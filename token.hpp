#pragma once

#include <iostream>
#include <regex>

namespace snowlang
{
    enum TokenType
    {
        TT_NULL,       // null token
        TT_EOF,        // end of file
        TT_WHITESPACE, // whitespace
        TT_COMMENT,    // comment
        TT_LBRACE,     // brackets
        TT_RBRACE,
        TT_LBRACK,
        TT_RBRACK,
        TT_LPAREN,
        TT_RPAREN,
        TT_SEMICOLON, // punctuation and operations
        TT_COMMA,
        TT_PERIOD,
        TT_PLUS,
        TT_MINUS,
        TT_POW,
        TT_MULT,
        TT_DIV,
        TT_REM,
        TT_MOD, // keywords
        TT_WIRE,
        TT_INPUT,
        TT_OUTPUT,
        TT_CONSTRUCT,
        TT_DEFINE,
        TT_CONNECT,
        TT_IF,
        TT_ELIF,
        TT_ELSE,
        TT_IS,
        TT_LOOP,
        TT_IN,
        TT_OR,
        TT_AND,
        TT_NOT,
        TT_TRUE,
        TT_FALSE,
        TT_GT,
        TT_GE,
        TT_EQ,
        TT_LE,
        TT_LT,
        TT_VAR, // tokens with variable value
        TT_IDEN,
        TT_INT
    };

    const std::vector<std::pair<std::regex, TokenType>> tokenRegexMap{
        {std::regex("^\\s+"), TT_WHITESPACE},  // whitespace
        {std::regex("^#.*\n"), TT_WHITESPACE}, // comment
        {std::regex("^\\{"), TT_LBRACE},       // brackets
        {std::regex("^\\}"), TT_RBRACE},
        {std::regex("^\\["), TT_LBRACK},
        {std::regex("^\\]"), TT_RBRACK},
        {std::regex("^\\("), TT_LPAREN},
        {std::regex("^\\)"), TT_RPAREN},
        {std::regex("^;"), TT_SEMICOLON}, // punctuation
        {std::regex("^,"), TT_COMMA},
        {std::regex("^\\."), TT_PERIOD},
        {std::regex("^\\+"), TT_PLUS}, // operations
        {std::regex("^-"), TT_MINUS},
        {std::regex("^\\*\\*"), TT_POW},
        {std::regex("^\\*"), TT_MULT},
        {std::regex("^/"), TT_DIV},
        {std::regex("^%"), TT_REM},
        {std::regex("^>="), TT_GE},
        {std::regex("^>"), TT_GT},
        {std::regex("^=="), TT_EQ},
        {std::regex("^<="), TT_LE},
        {std::regex("^<"), TT_LT},
        {std::regex("^\\|"), TT_OR},
        {std::regex("^&"), TT_AND},
        {std::regex("^!"), TT_NOT},
        {std::regex("^mod\\b"), TT_MOD}, // keywords
        {std::regex("^wire\\b"), TT_WIRE},
        {std::regex("^in:"), TT_INPUT},
        {std::regex("^out:"), TT_OUTPUT},
        {std::regex("^cons:"), TT_CONSTRUCT},
        {std::regex("^@"), TT_DEFINE},
        {std::regex("^="), TT_CONNECT},
        {std::regex("^if\\b"), TT_IF},
        {std::regex("^elif\\b"), TT_ELIF},
        {std::regex("^else\\b"), TT_ELSE},
        {std::regex("^is\\b"), TT_IS},
        {std::regex("^loop\\b"), TT_LOOP},
        {std::regex("^in\\b"), TT_IN},
        {std::regex("^true\\b"), TT_TRUE},
        {std::regex("^false\\b"), TT_FALSE},
        {std::regex("^\\$\\w+"), TT_VAR}, // var values
        {std::regex("^\\d+"), TT_INT},
        {std::regex("^\\w+"), TT_IDEN}};

    class Token
    {
    public:
        enum TokenType type = TT_NULL;
        std::string value;
        int tokenStart = 0, tokenEnd = 0;

        Token() = default;
        Token(enum TokenType t_type,
              std::string t_value,
              int t_tokenStart,
              int t_tokenEnd);

        static std::string reprTokenType(enum TokenType);
        std::string repr();
    };
}