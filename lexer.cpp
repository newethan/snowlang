#include <regex>

#include "lexer.hpp"
#include "errorHandler.hpp"
using namespace std;

namespace snowlang::lexer
{

    namespace // Implementation functions
    {
        Token generateNextToken(LexState &state)
        {
            string slicedText =
                state.text.substr(
                    state.pos,
                    state.text.length() - state.pos);
            for (const auto &keyValuePair : tokenRegexMap)
            {
                const auto &tokenRegex = keyValuePair.first;
                const auto &tokenType = keyValuePair.second;
                smatch match;
                regex_search(slicedText, match, tokenRegex);
                if (match.size() == 0)
                    continue;
                int tokenStart = state.pos;
                state.advance(match[0].length());
                return Token(
                    tokenType,
                    match[0],
                    tokenStart,
                    state.pos - 1);
            }
            throw err::SnowlangException(
                state.pos, state.pos, err::COULD_NOT_MAKE_TOKEN);
        }
    } // End of anonymous namespace

    vector<Token> lex(const string &text)
    {
        vector<Token> tokens;
        LexState state{text};
        while (state.pos < (int)text.length())
        {
            auto nextToken = generateNextToken(state);
            if (nextToken.type == TT_WHITESPACE ||
                nextToken.type == TT_COMMENT)
                continue;
            tokens.push_back(nextToken);
        }
        tokens.push_back(Token(TT_EOF, "", state.pos, state.pos));
        return tokens;
    }
}