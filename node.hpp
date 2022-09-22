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
        NT_LET,
        NT_CON,
        NT_FOR,
        NT_IF,
        NT_BLOCK,
        NT_SCRIPT,
        NT_MOD,
        NT_DECLARATIONS
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

    struct LetValue
    {
        std::unique_ptr<Node> type{nullptr};
        Token identifier;

        LetValue(std::unique_ptr<Node> t_type, Token t_identifier)
            : type(std::move(t_type)), identifier(t_identifier) {}
    };

    struct ConValue
    {
        std::unique_ptr<Node> left{nullptr};
        std::unique_ptr<Node> right{nullptr};

        ConValue(std::unique_ptr<Node> t_left,
                 std::unique_ptr<Node> t_right)
            : left(std::move(t_left)), right(std::move(t_right)) {}
    };

    struct ForValue
    {
        Token var;
        std::unique_ptr<Node> range{nullptr};
        std::unique_ptr<Node> block{nullptr};

        ForValue(
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
        std::unique_ptr<Node> declarations;
        std::unique_ptr<Node> block;

        ModuleValue(
            Token t_identifier,
            std::unique_ptr<Node> t_declarations,
            std::unique_ptr<Node> t_block)
            : identifier(t_identifier),
              declarations(move(t_declarations)),
              block(move(t_block)) {}
    };

    struct DeclarationsValue
    {
        std::vector<std::unique_ptr<Node>> types;
        std::vector<Token> identifiers;

        DeclarationsValue(
            std::vector<std::unique_ptr<Node>> t_types,
            std::vector<Token> t_identifiers)
            : types(move(t_types)), identifiers(t_identifiers) {}
    };

    struct ScriptValue
    {
        std::vector<std::unique_ptr<Node>> modules;

        ScriptValue(std::vector<std::unique_ptr<Node>> t_modules)
            : modules(move(t_modules)) {}
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
            LetValue,
            ConValue,
            ForValue,
            IfValue,
            BlockValue,
            ModuleValue,
            DeclarationsValue,
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