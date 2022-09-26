#pragma once
#include <iostream>
#include <variant>
#include "node.hpp"
#include "logic.hpp"
#include "errorHandler.hpp"
#include "logic.hpp"

namespace snowlang::interpreter
{
    struct Context;

    struct Number
    {
        union
        {
            int intVal = 0;
            float floatVal;
        };
        bool isInt = true;
        bool valid = false;

        Number() = default;
        Number(int t_number)
            : intVal(t_number), isInt(true), valid(true) {}
        Number(float t_number)
            : floatVal(t_number), isInt(false), valid(true) {}
        inline void set(int t_number)
        {
            intVal = t_number;
            isInt = true;
            valid = true;
        }
        inline void set(float t_number)
        {
            floatVal = t_number;
            isInt = false;
            valid = true;
        }
        inline bool holdsInt()
        {
            return isInt;
        }
        inline bool holdsFloat()
        {
            return !isInt;
        }
        inline int getInt() { return intVal; }
        inline float getFloat() { return floatVal; }

        bool isZero();

        static Number addOp(Number left, Number right);
        static Number subOp(Number left, Number right);
        static Number multOp(Number left, Number right);
        static Number divOp(Number left, Number right);
        static Number andOp(Number left, Number right);
        static Number orOp(Number left, Number right);
        static Number gtOp(Number left, Number right);
        static Number geOp(Number left, Number right);
        static Number eqOp(Number left, Number right);
        static Number neqOp(Number left, Number right);
        static Number leOp(Number left, Number right);
        static Number ltOp(Number left, Number right);
    };

    struct ModuleDeclaration
    {
        std::unique_ptr<Node> body;

        ModuleDeclaration(std::unique_ptr<Node> t_body)
            : body(std::move(t_body)) {}
    };

    struct FunctionDeclaration
    {
        std::vector<std::string> args;
        std::unique_ptr<Node> bodyNode;
        Number execute(std::vector<Number> args);

        FunctionDeclaration(
            std::vector<std::string> t_args,
            std::unique_ptr<Node> t_bodyNode)
            : args(std::move(t_args)),
              bodyNode(std::move(t_bodyNode)) {}
    };

    using SymbolValueType = std::variant<
        std::monostate,
        Number,
        ModuleDeclaration,
        FunctionDeclaration>;

    class SymbolTable
    {
    public:
        void setSymbol(const std::string &name, SymbolValueType value);
        SymbolValueType &lookup(
            const std::string &name,
            int posStart,
            int posEnd);
        SymbolTable(SymbolTable *t_parent) : parent(t_parent) {}
        SymbolTable() = default;

    private:
        std::unordered_map<std::string, SymbolValueType>
            m_symbols;
        SymbolTable *parent{nullptr};
    };

    using NodeReturnType = std::variant<
        std::monostate,
        Number,
        ModuleDeclaration,
        FunctionDeclaration,
        LogicGate *,
        std::vector<LogicGate> *>;

    class Interpreter
    {
    public:
        Interpreter(std::unique_ptr<Node> t_ast)
            : m_ast(std::move(t_ast)) {}
        std::unique_ptr<Module> interpret();

    private:
        std::unique_ptr<Node> m_ast;

        NodeReturnType buildModule(
            Context &ctx, const std::string &identifier,
            int posStart, int posEnd);
        NodeReturnType visit(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitBinOp(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitUnOp(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitLeaf(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitItem(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitDefine(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitCon(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitFor(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitWhile(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitBreak(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitContinue(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitReturn(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitIf(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitBlock(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitFuncDecl(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitFuncCall(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitMod(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitVarAssign(
            std::unique_ptr<Node> &node, Context &ctx);
    };

    struct Context
    {
        SymbolTable &symbolTable;
        Module &logic;
        Context(SymbolTable &t_symbolTable, Module &t_logic)
            : symbolTable(t_symbolTable), logic(t_logic) {}
    };
}