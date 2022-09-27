#pragma once
#include <iostream>
#include <variant>
#include <vector>
#include <memory>
#include "node.hpp"
#include "logic.hpp"
#include "errorHandler.hpp"
#include "logic.hpp"
#include "symbol.hpp"

namespace snowlang::interpreter
{
    struct Context
    {
        SymbolTable &symbolTable;
        Module &logic;

        // context information
        bool inLoop{false};
        bool inFunction{false};
        bool shouldContinue{false};
        bool shouldBreak{false};
        bool shouldReturn{false};
        Number returnValue{Number(0)};
        bool inRuntime{false};

        // module context
        std::vector<std::string> buildStack;

        Context(SymbolTable &t_symbolTable, Module &t_logic)
            : symbolTable(t_symbolTable), logic(t_logic) {}

        inline void copyCtxInfo(Context &other)
        {
            inLoop = other.inLoop;
            inFunction = other.inFunction;
            inRuntime = other.inRuntime;
        }

        inline void copyModuleCtx(Context &other)
        {
            buildStack = other.buildStack;
        }

        inline void copyInfoToForward(Context &other)
        {
            shouldContinue = other.shouldContinue;
            shouldBreak = other.shouldBreak;
            shouldReturn = other.shouldReturn;
            if (other.shouldReturn)
                returnValue = other.returnValue;
        }
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
        void interpret();

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
        NodeReturnType visitPrint(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitTick(
            std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitHold(
            std::unique_ptr<Node> &node, Context &ctx);
    };
}