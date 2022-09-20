#pragma once

#include "snowlang.hpp"
#include "token.hpp"

namespace snowlang::err
{
    // Error messages
    const std::string NOERR = "";
    const std::string INVALID_TOKEN = "Invalid token.";
    const std::string EXPECTED_RBRACK = "Expected ']'.";
    const std::string EXPECTED_LPAREN = "Expected '('.";
    const std::string EXPECTED_RPAREN = "Expected ')'.";
    const std::string EXPECTED_COMMA = "Expected ','.";
    const std::string EXPECTED_FIRST_OF_FACTOR =
        "Expected number, variable, '+', '-' or '('.";
    const std::string EXPECTED_COMPARISON =
        "Expected '>', '>=', '==', '<=' or '<'.";
    const std::string EXPECTED_EOI =
        "Unexpected token, expected end of input here.";
    const std::string EXPECTED_INT = "Expected number.";
    const std::string EXPECTED_IDEN = "Expected identifier.";

    // Custom exception class
    class SnowlangException : std::exception
    {
    public:
        int posStart;
        int posEnd;
        std::string message;

        SnowlangException(
            int t_posStart,
            int t_posEnd,
            std::string t_message)
            : posStart(t_posStart),
              posEnd(t_posEnd),
              message(t_message) {}

        SnowlangException(
            Token errorToken,
            std::string t_message)
            : posStart(errorToken.tokenStart),
              posEnd(errorToken.tokenEnd),
              message(t_message) {}
    };

    void fatalErrorAbort(
        const std::string &text,
        const std::string &filename,
        int posStart,
        int posEnd,
        const std::string &message);
}