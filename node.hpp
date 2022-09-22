#pragma once

#include <iostream>
#include <variant>
#include "token.hpp"

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
        NT_ITEM,
        NT_DEF,
        NT_CONN,
        NT_LOOP,
        NT_IF,
        NT_BLOCK,
        NT_SCRIPT,
        NT_MOD,
        NT_WIRE,
        NT_WIRE_CONTENT
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
        Token arraySize; // If token is TT_NULL, type is not array.

        TypeValue(Token t_identifier, Token t_arraySize = Token())
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

    struct DefineValue
    {
        std::unique_ptr<Node> type{nullptr};
        Token identifier;

        DefineValue(std::unique_ptr<Node> t_type, Token t_identifier)
            : type(std::move(t_type)), identifier(t_identifier) {}
    };

    struct ConnectValue
    {
        std::unique_ptr<Node> left{nullptr};
        std::unique_ptr<Node> right{nullptr};

        ConnectValue(std::unique_ptr<Node> t_left,
                     std::unique_ptr<Node> t_right)
            : left(std::move(t_left)), right(std::move(t_right)) {}
    };

    struct LoopValue
    {
        Token var;
        std::unique_ptr<Node> range{nullptr};
        std::unique_ptr<Node> block{nullptr};

        LoopValue(
            Token t_var,
            std::unique_ptr<Node> t_range,
            std::unique_ptr<Node> t_block)
            : var(t_var),
              range(std::move(t_range)),
              block(std::move(t_block)) {}
    };

    struct IfValue
    {
        std::vector<std::unique_ptr<Node>> conds;
        std::vector<std::unique_ptr<Node>> ifBlocks;

        IfValue(std::vector<std::unique_ptr<Node>> t_conds,
                std::vector<std::unique_ptr<Node>> t_ifBlocks)
            : conds(std::move(t_conds)),
              ifBlocks(std::move(t_ifBlocks)) {}
    };

    struct BlockValue
    {
        std::vector<std::unique_ptr<Node>> instructions;

        BlockValue(std::vector<std::unique_ptr<Node>> t_instructions)
            : instructions(move(t_instructions)) {}
    };

    struct ModuleValue
    {
        Token identifier;
        std::unique_ptr<Node> input;
        std::unique_ptr<Node> output;
        std::unique_ptr<Node> block;

        ModuleValue(
            Token t_identifier,
            std::unique_ptr<Node> t_input,
            std::unique_ptr<Node> t_output,
            std::unique_ptr<Node> t_block)
            : identifier(t_identifier),
              input(move(t_input)),
              output(move(t_output)),
              block(move(t_block)) {}
    };

    struct WireValue
    {
        Token identifier;
        std::unique_ptr<Node> content;

        WireValue(
            Token t_identifier,
            std::unique_ptr<Node> t_content)
            : identifier(t_identifier), content(move(t_content)) {}
    };

    struct WireContentValue
    {
        std::vector<std::unique_ptr<Node>> types;
        std::vector<Token> identifiers;

        WireContentValue(
            std::vector<std::unique_ptr<Node>> t_types,
            std::vector<Token> t_identifiers)
            : types(move(t_types)), identifiers(t_identifiers) {}
    };

    struct ScriptValue
    {
        std::vector<std::unique_ptr<Node>> fields;

        ScriptValue(std::vector<std::unique_ptr<Node>> t_fields)
            : fields(move(t_fields)) {}
    };

    ///////////////
    using NodeValueType =
        std::variant<
            LeafValue,
            BinOpValue,
            UnOpValue,
            RangeValue,
            TypeValue,
            ItemValue,
            DefineValue,
            ConnectValue,
            LoopValue,
            IfValue,
            BlockValue,
            ModuleValue,
            WireValue,
            WireContentValue,
            ScriptValue>;

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
    void printAst(const std::unique_ptr<Node> &ast,
                  int indent = 0);
}