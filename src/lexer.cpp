#include <regex>

#include "lexer.hpp"
#include "errorHandler.hpp"
#include "pos.hpp"

namespace snowlang::lexer
{
    bool Lexer::advance(int positions)
    {
        pos += positions;
        return pos < text.length();
    }

    bool Lexer::accept(char c)
    {
        if (peek() == c)
        {
            advance();
            return true;
        }
        return false;
    }

    char Lexer::peek(int offset)
    {
        if (pos + offset >= text.length())
            return '\0';
        return text[pos + offset];
    }

    void Lexer::generateNextToken()
    {
        if (isspace(peek()))
        {
            while (isspace(peek()))
                advance();
            return;
        }
        else if (accept('#')) // comment
        {
            while (pos < text.length())
            {
                advance();
                if (peek(-1) == '\n')
                    break;
            }
            return;
        }
        for (auto &charTypePair : charToToken)
        {
            if (accept(charTypePair.first))
            {
                tokens.push_back(
                    Token(charTypePair.second,
                          Pos(pos - 1, fileIndex)));
                return;
            }
        }
        for (auto &charsTypesPair : twoCharToToken)
        {
            auto chars = charsTypesPair.first;
            auto types = charsTypesPair.second;
            if (accept(chars.first))
            {
                if (accept(chars.second))
                    tokens.push_back(Token(
                        types.second,
                        Pos(pos - 2, pos - 1, fileIndex)));
                else
                    tokens.push_back(Token(
                        types.first, Pos(pos - 1, fileIndex)));
                return;
            }
        }
        // else (returned if any token found by now)
        if (isdigit(peek()))
            generateNumber();
        else if (isalpha(peek()) || peek() == '_')
            generateIden();
        else if (peek() == '"')
            generateStrlit();
        else
            throw err::LexerParserException(
                Pos(pos, fileIndex), err::COULD_NOT_MAKE_TOKEN);
    }

    std::vector<Token> Lexer::lex()
    {
        while (pos < text.length())
            generateNextToken();
        tokens.push_back(Token(TT_EOF, Pos(pos, fileIndex)));
        return tokens;
    }

    void Lexer::generateNumber()
    {
        std::string num;
        bool isFloat = false;
        const int tokenStart = pos;
        while (isdigit(peek()))
        {
            num += peek();
            advance();
        }
        if (peek() == '.')
        {
            num += '.';
            isFloat = true;
            advance();
        }
        while (isdigit(peek()))
        {
            num += peek();
            advance();
        }
        const int tokenEnd = pos - 1;
        if (isFloat)
            tokens.push_back(Token(
                TT_FLOAT,
                Pos(tokenStart, tokenEnd, fileIndex),
                num));
        else
            tokens.push_back(Token(
                TT_INT,
                Pos(tokenStart, tokenEnd, fileIndex),
                num));
    }
    void Lexer::generateIden()
    {
        std::string iden;
        const int tokenStart = pos;
        while (isalpha(peek()) || peek() == '_' || isdigit(peek()))
        {
            // this function is only called if the character reached
            // is alpha or _. here digits are also allowed.
            // this means that identifiers cannot start with digits but can
            // contain them.

            iden += peek();
            advance();
        }
        const int tokenEnd = pos - 1;

        // check for keywords
        for (auto &keywordTypePair : keywords)
            if (iden == keywordTypePair.first) // iden is keyword
            {
                tokens.push_back(Token(
                    keywordTypePair.second, // the type specified
                    Pos(tokenStart, tokenEnd, fileIndex)));
                return;
            }

        // identifier is not keyword - add it as an iden token.
        tokens.push_back(Token(
            TT_IDEN,
            Pos(tokenStart, tokenEnd, fileIndex),
            iden));
    }
    void Lexer::generateStrlit()
    {
        // accept opening '""
        std::string strlit("\"");
        const int tokenStart = pos;
        accept('"');

        // until next '"" or newline
        while (peek() != '"')
        {
            if (peek() == '\n' || peek() == '\0')
                throw err::LexerParserException(
                    Pos(tokenStart, tokenStart, fileIndex),
                    err::COULD_NOT_MAKE_TOKEN);

            // escape \""
            if (peek() == '\\')
            {
                advance();
                strlit += "\\";
                strlit += peek();
            }
            else
                strlit += peek();
            advance();
        }

        // accept closing '"'
        advance();
        const int tokenEnd = pos - 1;
        strlit += '"';

        tokens.push_back(Token(
            TT_STRLIT, // string literal
            Pos(tokenStart, tokenEnd, fileIndex),
            strlit));
    }
}