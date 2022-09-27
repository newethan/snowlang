#include <iostream>
#include "token.hpp"

#pragma once

namespace snowlang::err
{
    // Error messages

    const std::string NOERR = "";

    // Lexing errors
    const std::string COULD_NOT_MAKE_TOKEN =
        "Lexing error: Could not make token. Character may be illegal.";

    // Parsing errors
    const std::string EXPECTED_RBRACK = "Parsing error: Expected ']'.";
    const std::string EXPECTED_LPAREN = "Parsing error: Expected '('.";
    const std::string EXPECTED_RPAREN = "Parsing error: Expected ')'.";
    const std::string EXPECTED_LBRACE = "Parsing error: Expected '{'.";
    const std::string EXPECTED_RBRACE = "Parsing error: Expected '}'.";
    const std::string EXPECTED_GT = "Parsing error: Expected '>'.";
    const std::string EXPECTED_COMMA = "Parsing error: Expected ','.";
    const std::string EXPECTED_SEMICOLON = "Parsing error: Expected ';'.";
    const std::string EXPECTED_ASSIGN = "Parsing error: Expected '='.";
    const std::string EXPECTED_FIRST_OF_ATOM =
        "Parsing error: Expected number, variable (identifier),"
        " function call, '+', '-' or '('.";
    const std::string EXPECTED_FIRST_OF_COND_FACTOR =
        "Parsing error: Expected 'true', 'false', 'not' or '('.";
    const std::string EXPECTED_FIRST_OF_INSTRUCTION =
        "Parsing error: Expected 'let', 'con', 'for', 'while', 'break',"
        " 'continue', 'return' , 'if' or identifier.";
    const std::string EXPECTED_COMPARISON =
        "Parsing error: Expected '>', '>=', '==', '<=' or '<'.";
    const std::string EXPECTED_EOI =
        "Parsing error: Unexpected token, expected end of input here.";
    const std::string EXPECTED_INT = "Parsing error: Expected integer.";
    const std::string EXPECTED_IDEN = "Parsing error: Expected identifier.";
    const std::string EXPECTED_IN = "Parsing error: Expected 'in'.";
    const std::string EXPECTED_PUBLIC = "Parsing error: Expected 'public:'.";
    const std::string EXPECTED_PRIVATE = "Parsing error: Expected 'private:'.";

    // Interpretation errors
    inline const std::string IDENTIFIER_UNDEFINED(
        const std::string &identifier)
    {
        return "Runtime error: Identifier '" + identifier + "' undefined.";
    }
    const std::string EXPECTED_NUMBER = "Runtime error: Expected number (variable).";
    const std::string MEMBER_ARRAY_UNDEFINED = "Runtime error: Member array undefined.";
    const std::string MEMBER_UNDEFINED = "Runtime error: Member undefined.";
    const std::string INDEX_MUST_BE_INTEGER = "Runtime error: Index must be integer.";
    inline std::string INDEX_OUT_OF_BOUNDS(int lower, int upper, int got)
    {
        std::string buffer;
        buffer.append("Runtime error: index out of bounds of array.");
        buffer.append(" (Expected index between ");
        buffer.append(std::to_string(lower));
        buffer.append(" and ");
        buffer.append(std::to_string(upper));
        buffer.append(", inclusive, but got ");
        buffer.append(std::to_string(got));
        buffer.append(")");

        return buffer;
    }
    const std::string RANGE_BOUND_MUST_BE_INTEGER =
        "Runtime error: Range bound must be integer.";
    const std::string EXPECTED_POS_INT_TICKS =
        "Runtime error: Expected a positive integer.";
    const std::string INT_ONLY_OP = "Runtime error: Operation defined for integers only.";
    const std::string LOWER_BOUND_GT_UPPER_BOUND =
        "Runtime error: Upper bound must be greater than or equal to lower bound.";
    const std::string NO_MEMBER_TO_ACCESS =
        "Runtime error: No member to access. (object is not a module)";
    const std::string OBJECT_TYPE_INCORRECT =
        "Runtime error: Object must be gate or array of gates";
    const std::string ALREADY_DEFINED =
        "Runtime error: Object already defined.\nNote: try using a different name.";
    const std::string DOES_NOT_NAME_MODULE_TYPE =
        "Runtime error: Expected a module type (declared using the 'mod' keyword)";
    const std::string DOES_NOT_NAME_FUNCTION =
        "Runtime error: Expected a function (declared using the 'let' keyword)";
    const std::string DIV_BY_ZERO = "Runtime error: Division by zero.";
    const std::string CONNECT_ARRAY_TO_NOT_ARRAY =
        "Runtime error: Cannot connect object that is not array to object that is array";
    const std::string CONNECT_ARRAY_TO_DIFF_SIZED_ARRAY =
        "Runtime error: Cannot connect differently sized arrays of gates.";
    inline const std::string WRONG_ARG_NUM(int expected, int got)
    {
        std::string buffer;
        buffer.append("Runtime error: Incorrect number of arguments to call function.");
        buffer.append(" (Expected ");
        buffer.append(std::to_string(expected));
        buffer.append(", got ");
        buffer.append(std::to_string(got));
        buffer.append(")");

        return buffer;
    }
    const std::string RETURN_OUTSIDE_FUNCTION =
        "Runtime error: 'return' statement outside of a function.";
    const std::string BREAK_CONTINUE_OUTSIDE_LOOP =
        "Runtime error: 'break' or 'continue' statement outisde of a loop.";
    const std::string LOGIC_INSIDE_FUNCTION =
        "Runtime error: Cannot modify modules inside a function.";
    const std::string TICK_HOLD_OUTSIDE_RUNTIME =
        "Runtime error: Cannot perform runtime logic operations"
        "outside 'runtime' function";
    const std::string CIRCULAR_CONSTRUCTION =
        "Runtime error: Cannot construct module using itself.";
    inline const std::string ITEM_VALUE_WRONG_SIZE(
        int expected, int got)
    {
        std::string buffer;
        buffer.append("Runtime error: Incorrect size for object value.");
        buffer.append(" (Expected ");
        buffer.append(std::to_string(expected));
        if (expected == 1)
            buffer.append(" bit, got ");
        else
            buffer.append(" bits, got ");
        buffer.append(std::to_string(got));
        if (got == 1)
            buffer.append(" bit)");
        else
            buffer.append(" bits)");

        return buffer;
    }
    const std::string OBJECT_VALUE_ONE_OR_ZERO =
        "Runtime error: Object value must be made up of '0' and '1'.";

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