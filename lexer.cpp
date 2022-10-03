#include <regex>

#include "lexer.hpp"
#include "errorHandler.hpp"

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
        return text[pos + offset];
    }

    void Lexer::generateNextToken()
    {
        if (isspace(peek()))
        {
            while (isspace(peek()))
                advance();
        }
        else if (accept('#')) // comment
        {
            while (pos < text.length())
            {
                advance();
                if (peek(-1) == '\n')
                    break;
            }
        }
        else if (accept('{'))
            tokens.push_back(Token(TT_LBRACE, pos - 1));
        else if (accept('}'))
            tokens.push_back(Token(TT_RBRACE, pos - 1));
        else if (accept('['))
            tokens.push_back(Token(TT_LBRACK, pos - 1));
        else if (accept(']'))
            tokens.push_back(Token(TT_RBRACK, pos - 1));
        else if (accept('('))
            tokens.push_back(Token(TT_LPAREN, pos - 1));
        else if (accept(')'))
            tokens.push_back(Token(TT_RPAREN, pos - 1));
        else if (accept(';'))
            tokens.push_back(Token(TT_SEMICOLON, pos - 1));
        else if (accept(','))
            tokens.push_back(Token(TT_COMMA, pos - 1));
        else if (accept('.'))
            tokens.push_back(Token(TT_PERIOD, pos - 1));
        else if (accept('+'))
            tokens.push_back(Token(TT_PLUS, pos - 1));
        else if (accept('-'))
            tokens.push_back(Token(TT_MINUS, pos - 1));
        else if (accept('*'))
        {
            if (accept('*'))
                tokens.push_back(Token(TT_POW, pos - 2, pos - 1));
            else
                tokens.push_back(Token(TT_MULT, pos - 1));
        }
        else if (accept('/'))
            tokens.push_back(Token(TT_DIV, pos - 1));
        else if (accept('%'))
            tokens.push_back(Token(TT_REM, pos - 1));
        else if (accept('|'))
            tokens.push_back(Token(TT_OR, pos - 1));
        else if (accept('&'))
            tokens.push_back(Token(TT_AND, pos - 1));
        else if (accept('='))
        {
            if (accept('='))
                tokens.push_back(Token(TT_EQ, pos - 2, pos - 1));
            else
                tokens.push_back(Token(TT_ASSIGN, pos - 1));
        }
        else if (accept('!'))
        {
            if (accept('='))
                tokens.push_back(Token(TT_NEQ, pos - 2, pos - 1));
            else
                tokens.push_back(Token(TT_NOT, pos - 1));
        }
        else if (accept('>'))
        {
            if (accept('='))
                tokens.push_back(Token(TT_GE, pos - 2, pos - 1));
            else
                tokens.push_back(Token(TT_GT, pos - 1));
        }
        else if (accept('<'))
        {
            if (accept('='))
                tokens.push_back(Token(TT_LE, pos - 2, pos - 1));
            else
                tokens.push_back(Token(TT_LT, pos - 1));
        }
        else if (isdigit(peek()))
            generateNumber();
        else if (isalpha(peek()) || peek() == '_')
            generateIden();
        else if (peek() == '"')
            generateStrlit();
        else
            throw err::SnowlangException(
                pos, pos, err::COULD_NOT_MAKE_TOKEN);
    }

    std::vector<Token> Lexer::lex()
    {
        while (pos < text.length())
            generateNextToken();
        tokens.push_back(Token(TT_EOF, "", pos, pos));
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
        }
        while (isdigit(peek()))
        {
            num += peek();
            advance();
        }
        const int tokenEnd = pos - 1;
        if (isFloat)
            tokens.push_back(Token(TT_FLOAT, num, tokenStart, tokenEnd));
        else
            tokens.push_back(Token(TT_INT, num, tokenStart, tokenEnd));
    }
    void Lexer::generateIden()
    {
        std::string iden;
        const int tokenStart = pos;
        while (isalpha(peek()) || peek() == '_')
        {
            iden += peek();
            advance();
        }
        const int tokenEnd = pos - 1;

        // check for keywords
        if (iden == "mod")
            tokens.push_back(Token(TT_MOD, tokenStart, tokenEnd));
        else if (iden == "print")
            tokens.push_back(Token(TT_PRINT, tokenStart, tokenEnd));
        else if (iden == "tick")
            tokens.push_back(Token(TT_TICK, tokenStart, tokenEnd));
        else if (iden == "hold")
            tokens.push_back(Token(TT_HOLD, tokenStart, tokenEnd));
        else if (iden == "let")
            tokens.push_back(Token(TT_LET, tokenStart, tokenEnd));
        else if (iden == "con")
            tokens.push_back(Token(TT_CON, tokenStart, tokenEnd));
        else if (iden == "if")
            tokens.push_back(Token(TT_IF, tokenStart, tokenEnd));
        else if (iden == "elif")
            tokens.push_back(Token(TT_ELIF, tokenStart, tokenEnd));
        else if (iden == "else")
            tokens.push_back(Token(TT_ELSE, tokenStart, tokenEnd));
        else if (iden == "for")
            tokens.push_back(Token(TT_FOR, tokenStart, tokenEnd));
        else if (iden == "in")
            tokens.push_back(Token(TT_IN, tokenStart, tokenEnd));
        else if (iden == "while")
            tokens.push_back(Token(TT_WHILE, tokenStart, tokenEnd));
        else if (iden == "break")
            tokens.push_back(Token(TT_BREAK, tokenStart, tokenEnd));
        else if (iden == "continue")
            tokens.push_back(Token(TT_CONTINUE, tokenStart, tokenEnd));
        else if (iden == "return")
            tokens.push_back(Token(TT_RETURN, tokenStart, tokenEnd));
        else
            tokens.push_back(Token(TT_IDEN, iden, tokenStart, tokenEnd));
    }
    void Lexer::generateStrlit()
    {
        std::string strlit("\"");
        const int tokenStart = pos;
        accept('"');
        while (peek() != '"')
        {
            if (peek() == '\n')
                throw err::SnowlangException(
                    tokenStart, tokenStart, err::COULD_NOT_MAKE_TOKEN);
            strlit += peek();
            advance();
        }
        advance();
        const int tokenEnd = pos - 1;
        strlit += '"';
        tokens.push_back(Token(TT_STRLIT, strlit, tokenStart, tokenEnd));
    }
}