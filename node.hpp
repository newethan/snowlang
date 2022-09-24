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
        NT_WHILE,
        NT_BREAK,
        NT_CONTINUE,
        NT_RETURN,
        NT_IF,
        NT_BLOCK,
        NT_SCRIPT,
        NT_FUNCDECL,
        NT_FUNCCALL,
        NT_MOD,
        NT_DECLARATIONS,
        NT_VARASSIGN
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

    struct WhileValue
    {
        std::unique_ptr<Node> cond;
        std::unique_ptr<Node> block;

        WhileValue(
            std::unique_ptr<Node> t_cond,
            std::unique_ptr<Node> t_block)
            : cond(std::move(t_cond)),
              block(std::move(t_block)) {}
    };

    struct BreakValue
    {
        BreakValue() = default;
    };

    struct ContinueValue
    {
        ContinueValue() = default;
    };

    struct ReturnValue
    {
        std::unique_ptr<Node> expression;

        ReturnValue(std::unique_ptr<Node> t_expression)
            : expression(std::move(t_expression)) {}
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
            : instructions(std::move(t_instructions)) {}
    };

    struct FuncDeclValue
    {
        Token identifier;
        std::vector<Token> argNames;
        std::unique_ptr<Node> body;

        FuncDeclValue(
            Token &t_identifier,
            std::vector<Token> &t_argNames,
            std::unique_ptr<Node> t_body)
            : identifier(t_identifier),
              argNames(t_argNames),
              body(std::move(t_body)) {}
    };

    struct FuncCallValue
    {
        Token identifier;
        std::vector<std::unique_ptr<Node>> args;

        FuncCallValue(
            Token &t_identifier,
            std::vector<std::unique_ptr<Node>> t_args)
            : identifier(t_identifier),
              args(std::move(t_args)) {}
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
              declarations(std::move(t_declarations)),
              block(std::move(t_block)) {}
    };

    struct DeclarationsValue
    {
        std::vector<std::unique_ptr<Node>> types;
        std::vector<Token> identifiers;

        DeclarationsValue(
            std::vector<std::unique_ptr<Node>> t_types,
            std::vector<Token> t_identifiers)
            : types(std::move(t_types)), identifiers(t_identifiers) {}
    };

    struct ScriptValue
    {
        std::vector<std::unique_ptr<Node>> fields;

        ScriptValue(std::vector<std::unique_ptr<Node>> t_fields)
            : fields(std::move(t_fields)) {}
    };

    struct VarAssignValue
    {
        Token identifier;
        std::unique_ptr<Node> expression;

        VarAssignValue(
            Token t_identifier,
            std::unique_ptr<Node> t_expression)
            : identifier(t_identifier),
              expression(std::move(t_expression)) {}
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
            WhileValue,
            BreakValue,
            ContinueValue,
            ReturnValue,
            IfValue,
            BlockValue,
            FuncDeclValue,
            FuncCallValue,
            ModuleValue,
            DeclarationsValue,
            ScriptValue,
            VarAssignValue>;

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