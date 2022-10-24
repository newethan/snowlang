#include <iostream>
#include "token.hpp"
#include "node.hpp"

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
    const std::string EXPECTED_COMMA = "Parsing error: Expected ','.";
    const std::string EXPECTED_SEMICOLON = "Parsing error: Expected ';'.";
    const std::string EXPECTED_FIRST_OF_ATOM =
        "Parsing error: Expected number, variable (identifier),"
        " function call, '+', '-' or '('.";
    const std::string EXPECTED_FIRST_OF_INSTRUCTION =
        "Parsing error: Expected 'let', 'con', 'for', 'while', 'break',"
        " 'continue', 'return' , 'if' or identifier.";
    const std::string EXPECTED_EOI =
        "Parsing error: Unexpected token.";
    const std::string EXPECTED_INT = "Parsing error: Expected integer.";
    const std::string EXPECTED_IDEN = "Parsing error: Expected identifier.";
    const std::string EXPECTED_IN = "Parsing error: Expected 'in'.";
    const std::string EXPECTED_STRLIT = "Parsing error: Expected string literal.";

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
        std::string buf;
        buf += "Runtime error: index out of bounds.";

        // both bounds are the same - only one index possible
        if (lower == upper)
        {
            buf += " (Expected ";
            buf += std::to_string(lower);
        }
        else // range for possible indeces
        {
            buf += " (Expected index between ";
            buf += std::to_string(lower);
            buf += " and ";
            buf += std::to_string(upper);
            buf += ", inclusive,";
        }
        buf += " but got ";
        buf += std::to_string(got);
        buf += ")";

        return buf;
    }
    const std::string RANGE_BOUND_MUST_BE_INTEGER =
        "Runtime error: Range bound must be integer.";
    const std::string EXPECTED_POS_INT =
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
        std::string buf;
        buf += "Runtime error: Incorrect number of arguments to call function.";
        buf += " (Expected ";
        buf += std::to_string(expected);
        buf += ", got ";
        buf += std::to_string(got);
        buf += ")";

        return buf;
    }
    inline const std::string MOD_WRONG_ARG_NUM(int expected, int got)
    {
        std::string buf;
        buf += "Runtime error: Incorrect number of arguments to construct module.";
        buf += " (Expected ";
        buf += std::to_string(expected);
        buf += ", got ";
        buf += std::to_string(got);
        buf += ")";

        return buf;
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
    const std::string CIRCULAR_IMPORT =
        "Runtime error: File cannot import itself.";
    inline const std::string ITEM_VALUE_WRONG_SIZE(
        int expected, int got)
    {
        std::string buf;
        buf += "Runtime error: Incorrect size for object value.";
        buf += " (Expected ";
        buf += std::to_string(expected);
        if (expected == 1)
            buf += " bit, got ";
        else
            buf += " bits, got ";
        buf.append(std::to_string(got));
        if (got == 1)
            buf += " bit)";
        else
            buf += " bits)";

        return buf;
    }
    const std::string OBJECT_VALUE_ONE_OR_ZERO =
        "Runtime error: Object value must be made up of '0' and '1'.";
    inline std::string FILE_NOT_FOUND(const std::string &filename)
    {
        std::string buf;
        buf += "Runtime error: File '";
        buf += filename;
        buf += "' not found.";
        return buf;
    }
    inline std::string REDECL(const std::string &name)
    {
        std::string buf;
        buf += "Runtime error: Redeclaration of module or function '";
        buf += name;
        buf += "'.";
        return buf;
    }
    const std::string EXPECTED_LVALUE =
        "Runtime error: Expected lvalue (identifier)";

    // Lexer and parser exception
    class LexerParserException : std::exception
    {
    public:
        Pos pos;
        std::string message;

        LexerParserException(
            Pos t_pos, const std::string &t_message)
            : pos(t_pos), message(t_message) {}
    };

    // Interpreter exception
    class InterpreterException : std::exception
    {
    public:
        Pos pos;
        std::string filename, text, message;

        InterpreterException(
            const std::string &t_filename, const std::string &t_text,
            Pos t_pos, const std::string &t_message)
            : pos(t_pos), filename(t_filename), text(t_text),
              message(t_message) {}
    };

    void fatalErrorAbort(
        Pos pos,
        const std::string &filename, const std::string &text,
        const std::string &message, bool shouldExit = true);
}