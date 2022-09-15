#pragma once

#include "snowlang.hpp"
#include "lexer.hpp"

namespace snowlang
{
    enum NodeType
    {
        NT_BINOP,
        NT_INT
    };

    union NodeValue
    {
        struct BinOpValue binOpValue;
        struct NumberValue numberValue;
    };

    // VALUE STRUCTS
    struct BinOpValue
    {
        Node left;
        Node right;
        Token operationToken;
    };
    struct NumberValue
    {
        Token numberToken;
    };

    ///////////////

    class Node
    {
    public:
        enum NodeType type;
        union NodeValue value;
    };
}