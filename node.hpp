#pragma once

#include "snowlang.hpp"
#include "lexer.hpp"

namespace snowlang
{

    class Node;

    enum NodeType
    {
        NT_BINOP,
        NT_UNOP,
        NT_NUMBER
    };

    /////////////// value structs

    struct NumberValue
    {
        Token numberToken;

        NumberValue(Token t_numberToken)
            : numberToken(t_numberToken) {}
    };
    struct BinOpValue
    {
        std::unique_ptr<Node> left{nullptr};
        std::unique_ptr<Node> right{nullptr};
        Token operationToken;

        BinOpValue(
            std::unique_ptr<Node> t_left,
            std::unique_ptr<Node> t_right,
            Token t_operationToken)
            : left(std::move(t_left)),
              right(std::move(t_right)),
              operationToken(t_operationToken) {}
    };

    struct UnOpValue
    {
        std::unique_ptr<Node> node{nullptr};
        Token operationToken;

        UnOpValue(std::unique_ptr<Node> t_node, Token t_operationToken)
            : node(std::move(t_node)), operationToken(t_operationToken) {}
    };

    ///////////////

    class Node
    {
    public:
        enum NodeType type;

        // By default number value
        std::variant<NumberValue, BinOpValue, UnOpValue> value;

        Node(enum NodeType t_type,
             std::variant<
                 NumberValue,
                 BinOpValue,
                 UnOpValue>
                 t_value);

        static std::string reprNodeType(enum NodeType nodeType);
    };
}