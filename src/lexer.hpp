#pragma once

#include <iostream>
#include "token.hpp"

namespace snowlang::lexer
{
    // map to identify single character tokens
    const std::unordered_map<char, TokenType> charToToken{
        {'{', TT_LBRACE},
        {'}', TT_RBRACE},
        {'[', TT_LBRACK},
        {']', TT_RBRACK},
        {'(', TT_LPAREN},
        {')', TT_RPAREN},
        {';', TT_SEMICOLON},
        {',', TT_COMMA},
        {'.', TT_PERIOD},
        {'+', TT_PLUS},
        {'-', TT_MINUS},
        {'/', TT_DIV},
        {'%', TT_REM},
        {'|', TT_OR},
        {'&', TT_AND},
    };

    // map to identify single or double character tokens
    // for example if '=' is enountered then next character
    // has to be checked. if it is also '=' then we have
    // equality token (TT_EQ). If there's only one '='
    // then we have an assignment token (TT_ASSIGN).
    // the char pair represents the two characters.
    // in our example '=' twice. The TokenType pair
    // represents the two options for token types.
    // in our example TT_ASSIGN followed by TT_EQ.
    const std::vector<std::pair<
        std::pair<char, char>, std::pair<TokenType, TokenType>>>
        twoCharToToken{
            {{'*', '*'}, {TT_MULT, TT_POW}},
            {{'=', '='}, {TT_ASSIGN, TT_EQ}},
            {{'!', '='}, {TT_NOT, TT_NEQ}},
            {{'>', '='}, {TT_GT, TT_GE}},
            {{'<', '='}, {TT_LT, TT_LE}}};

    // map to identify keywords
    const std::unordered_map<std::string, TokenType> keywords{
        {"import", TT_IMPORT},
        {"mod", TT_MOD},
        {"print", TT_PRINT},
        {"tick", TT_TICK},
        {"hold", TT_HOLD},
        {"let", TT_LET},
        {"con", TT_CON},
        {"if", TT_IF},
        {"elif", TT_ELIF},
        {"else", TT_ELSE},
        {"for", TT_FOR},
        {"in", TT_IN},
        {"while", TT_WHILE},
        {"break", TT_BREAK},
        {"continue", TT_CONTINUE},
        {"return", TT_RETURN}};

    class Lexer
    {
    public:
        Lexer(const std::string &t_text, size_t t_fileIndex = 0)
            : text(t_text), fileIndex(t_fileIndex) {}
        std::vector<Token> lex();

    private:
        const std::string &text;
        const size_t fileIndex = 0;
        size_t pos = 0;
        std::vector<Token> tokens;
        void generateNextToken();
        bool advance(int positions = 1);
        bool accept(char c);
        char peek(int offset = 0);

        void generateNumber();
        void generateIden();
        void generateStrlit();
    };
}
