#include "lexer.hpp"
using namespace std;

namespace snowlang::lexer
{
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