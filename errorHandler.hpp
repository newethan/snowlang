#pragma once

#include <iostream>
#include "token.hpp"

namespace snowlang::err
{
    // Error messages
    const std::string NOERR = "";
    const std::string COULD_NOT_MAKE_TOKEN =
        "Could not make token. Character may be illegal.";
    const std::string EXPECTED_RBRACK = "Expected ']'.";
    const std::string EXPECTED_LPAREN = "Expected '('.";
    const std::string EXPECTED_RPAREN = "Expected ')'.";
    const std::string EXPECTED_LBRACE = "Expected '{'.";
    const std::string EXPECTED_RBRACE = "Expected '}'.";
    const std::string EXPECTED_COMMA = "Expected ','.";
    const std::string EXPECTED_SEMICOLON = "Expected ';'.";
    const std::string EXPECTED_ASSIGN = "Expected '='.";
    const std::string EXPECTED_FIRST_OF_ATOM =
        "Expected number, variable (identifier),"
        " function call, '+', '-' or '('.";
    const std::string EXPECTED_FIRST_OF_COND_FACTOR =
        "Expected 'true', 'false', 'not' or '('.";
    const std::string EXPECTED_FIRST_OF_INSTRUCTION =
        "Expected 'let', 'con', 'for', 'while', 'break',"
        " 'continue', 'if' or identifier.";
    const std::string EXPECTED_FIRST_OF_FUNCTION_INSTRUCTION =
        "Expected 'for', 'while', 'break',"
        " 'continue', 'return', 'if' or identifier.";
    const std::string EXPECTED_COMPARISON =
        "Expected '>', '>=', '==', '<=' or '<'.";
    const std::string EXPECTED_EOI =
        "Unexpected token, expected end of input here.";
    const std::string EXPECTED_INT = "Expected integer.";
    const std::string EXPECTED_IDEN = "Expected identifier.";
    const std::string EXPECTED_IN = "Expected 'in'.";
    const std::string EXPECTED_PUBLIC = "Expected 'public:'.";
    const std::string EXPECTED_PRIVATE = "Expected 'private:'.";

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