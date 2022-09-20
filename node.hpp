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
        NT_LEAF,
        NT_RANGE,
        NT_TYPE,
        NT_ITEM
    };

    /////////////// value structs

    struct LeafValue
    {
        Token token;

        LeafValue(Token t_token)
            : token(t_token) {}
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

    struct RangeValue
    {
        std::unique_ptr<Node> from{nullptr};
        std::unique_ptr<Node> to{nullptr};

        RangeValue(std::unique_ptr<Node> t_from, std::unique_ptr<Node> t_to)
            : from(std::move(t_from)), to(std::move(t_to)) {}
    };

    struct TypeValue
    {
        Token identifier;
        int arraySize{-1}; // If value is -1, type is not array.

        TypeValue(Token t_identifier, int t_arraySize = -1)
            : identifier(t_identifier), arraySize(t_arraySize) {}
    };

    struct ItemValue
    {
        Token identifier;

        // If value is nullptr, identifier is not indexed
        std::unique_ptr<Node> index{nullptr};
        std::unique_ptr<Node> next{nullptr};

        ItemValue(Token t_identifier,
                  std::unique_ptr<Node> t_index,
                  std::unique_ptr<Node> t_next)
            : identifier(t_identifier),
              index(std::move(t_index)),
              next(std::move(t_next)) {}
    };

    ///////////////
    using NodeValueType =
        std::variant<
            LeafValue,
            BinOpValue,
            UnOpValue,
            RangeValue,
            TypeValue,
            ItemValue>;

    class Node
    {
    public:
        enum NodeType type;

        // By default LeafValue
        NodeValueType value;

        Node(enum NodeType t_type,
             NodeValueType t_value)
            : type(t_type), value(move(t_value)) {}

        static std::string reprNodeType(NodeType nodeType);
    };
}