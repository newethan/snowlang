#include <iostream>
#include "token.hpp"

#pragma once

namespace snowlang::err
{
    // Error messages

    const std::string NOERR = "";

    // Lexing errors
    const std::string COULD_NOT_MAKE_TOKEN =
        "Could not make token. Character may be illegal.";

    // Parsing errors
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

    // Interpretation errors
    inline const std::string IDENTIFIER_UNDEFINED(
        const std::string &identifier)
    {
        return "Identifier '" + identifier + "' undefined.";
    }
    const std::string EXPECTED_NUMBER = "Expected number (variable).";
    const std::string MEMBER_ARRAY_UNDEFINED = "Member array undefined.";
    const std::string MEMBER_UNDEFINED = "Member undefined.";
    const std::string INDEX_MUST_BE_INTEGER = "Index must be integer.";
    const std::string NO_MEMBER_TO_ACCESS =
        "No member to access. (object is not a module)";
    const std::string OBJECT_TYPE_INCORRECT =
        "Object must be gate or array of gates";
    const std::string ALREADY_DEFINED =
        "Object already defined.\nNote: try using a different name.";
    const std::string DOES_NOT_NAME_MODULE_TYPE =
        "Expected a module type (declared using the 'mod' keyword)";
    const std::string DIV_BY_ZERO = "Division by zero.";
    const std::string CONNECT_ARRAY_TO_NOT_ARRAY =
        "Cannot connect object that is not array to object that is array";
    const std::string CONNECT_ARRAY_TO_DIFF_SIZED_ARRAY =
        "Cannot connect differently sized arrays of gates.";

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