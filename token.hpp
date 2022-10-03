#pragma once

#include <iostream>
#include <regex>

namespace snowlang
{
    enum TokenType
    {
        TT_NULL,   // null token
        TT_EOF,    // end of file
        TT_LBRACE, // brackets
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
        TT_PRINT,
        TT_TICK,
        TT_HOLD,
        TT_LET,
        TT_CON,
        TT_ASSIGN,
        TT_IF,
        TT_ELIF,
        TT_ELSE,
        TT_FOR,
        TT_WHILE,
        TT_BREAK,
        TT_CONTINUE,
        TT_RETURN,
        TT_IN,
        TT_OR,
        TT_AND,
        TT_NOT,
        TT_GT,
        TT_GE,
        TT_EQ,
        TT_NEQ,
        TT_LE,
        TT_LT,
        TT_FLOAT, // tokens with variable value
        TT_INT,
        TT_IDEN,
        TT_STRLIT
    };

    const std::vector<std::pair<std::regex, TokenType>> tokenRegexMap{
        {std::regex("^\\{"), TT_LBRACE},
        {std::regex("^\\}"), TT_RBRACE},
        {std::regex("^\\["), TT_LBRACK},
        {std::regex("^\\]"), TT_RBRACK},
        {std::regex("^\\("), TT_LPAREN},
        {std::regex("^\\)"), TT_RPAREN},
        {std::regex("^;"), TT_SEMICOLON},
        {std::regex("^,"), TT_COMMA},
        {std::regex("^\\."), TT_PERIOD},
        {std::regex("^\\+"), TT_PLUS},
        {std::regex("^-"), TT_MINUS},
        {std::regex("^\\*\\*"), TT_POW},
        {std::regex("^\\*"), TT_MULT},
        {std::regex("^/"), TT_DIV},
        {std::regex("^%"), TT_REM},
        {std::regex("^mod\\b"), TT_MOD},
        {std::regex("^print\\b"), TT_PRINT},
        {std::regex("^tick\\b"), TT_TICK},
        {std::regex("^hold\\b"), TT_HOLD},
        {std::regex("^let\\b"), TT_LET},
        {std::regex("^con\\b"), TT_CON},
        {std::regex("^=(?!=)"), TT_ASSIGN},
        {std::regex("^if\\b"), TT_IF},
        {std::regex("^elif\\b"), TT_ELIF},
        {std::regex("^else\\b"), TT_ELSE},
        {std::regex("^for\\b"), TT_FOR},
        {std::regex("^while\\b"), TT_WHILE},
        {std::regex("^break\\b"), TT_BREAK},
        {std::regex("^continue\\b"), TT_CONTINUE},
        {std::regex("^return\\b"), TT_RETURN},
        {std::regex("^in\\b"), TT_IN},
        {std::regex("^\\|"), TT_OR},
        {std::regex("^&"), TT_AND},
        {std::regex("^!(?!=)"), TT_NOT},
        {std::regex("^>(?!=)"), TT_GT},
        {std::regex("^>="), TT_GE},
        {std::regex("^=="), TT_EQ},
        {std::regex("^!="), TT_NEQ},
        {std::regex("^<="), TT_LE},
        {std::regex("^<(?!=)"), TT_LT},
        {std::regex("^[0-9]+[.][0-9]+"), TT_FLOAT},
        {std::regex("^\\d+"), TT_INT},
        {std::regex("^\\w+\\b"), TT_IDEN},
        {std::regex("^\".*?\""), TT_STRLIT}};

    struct Token
    {
        enum TokenType type = TT_NULL;
        std::string value;
        int tokenStart = 0, tokenEnd = 0;

        Token() = default;

        Token(enum TokenType t_type,
              std::string t_value,
              int t_tokenStart, int t_tokenEnd)
            : type(t_type), value(t_value),
              tokenStart(t_tokenStart),
              tokenEnd(t_tokenEnd) {}
        Token(enum TokenType t_type,
              std::string t_value,
              int t_pos)
            : type(t_type), value(t_value),
              tokenStart(t_pos),
              tokenEnd(t_pos) {}

        Token(enum TokenType t_type,
              int t_tokenStart, int t_tokenEnd)
            : type(t_type),
              tokenStart(t_tokenStart),
              tokenEnd(t_tokenEnd) {}
        Token(enum TokenType t_type,
              int t_pos)
            : type(t_type),
              tokenStart(t_pos),
              tokenEnd(t_pos) {}
    };
}