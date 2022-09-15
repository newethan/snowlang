#include "lexer.hpp"
using namespace std;

namespace snowlang::lexer
{

    //
    // TOKEN METHODS
    //

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

    //
    // LEXER METHODS
    //

    Lexer::Lexer(string t_text, string t_filename)
        : text(t_text), filename(t_filename)
    {
        generateTokens();
    }

    void Lexer::generateTokens()
    {
        int textPos = 0;
        while (textPos < text.length())
        {
            auto nextToken = generateNextToken(textPos);
            if (nextToken.type == TT_WHITESPACE)
                continue;
            tokens.push_back(nextToken);
        }
    }

    Token Lexer::generateNextToken(int &textPos)
    {
        string slicedText = text.substr(textPos, text.length() - textPos);
        for (const auto &keyValuePair : tokenRegexMap)
        {
            const auto tokenRegex = keyValuePair.first;
            const auto tokenType = keyValuePair.second;
            smatch match;
            regex_search(slicedText, match, tokenRegex);
            if (match.size() == 0)
                continue;
            int tokenStart = textPos;
            textPos += match[0].length();
            return Token(tokenType, match[0], tokenStart, textPos - 1);
        }
        cout << "Error! Token not recognized." << endl;
        cout << slicedText << endl;
        exit(1);
    }
}