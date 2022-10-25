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

        Context(SymbolTable &t_symbolTable, Module &t_logic)
            : symbolTable(t_symbolTable), logic(t_logic) {}

        inline void copyCtxInfo(Context &other)
        {
            inLoop = other.inLoop;
            inFunction = other.inFunction;
            inRuntime = other.inRuntime;
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
        std::vector<LogicGate> *,
        Module *,
        std::vector<std::unique_ptr<Module>> *>;

    class Interpreter
    {
    public:
        Interpreter(std::unique_ptr<Node> t_ast, const std::string &filename, const std::string &text)
            : m_ast(std::move(t_ast))
        {
            importedFiles.push_back(filename);
            files.push_back(text);
        }
        void interpret();

    private:
        std::unique_ptr<Node> m_ast;

        std::vector<std::string> buildStack;    // build call stack
        std::vector<std::string> importStack;   // import call stack
        std::vector<std::string> importedFiles; // filenames
        std::vector<std::string> files;         // file contents

        inline void error(Pos pos, const std::string &message)
        {
            throw err::InterpreterException(
                importedFiles[pos.fileIndex],
                files[pos.fileIndex],
                pos, message);
        }

        std::unique_ptr<Module> buildModule(
            Context &ctx,
            std::string typeName, Pos pos,
            const std::vector<std::unique_ptr<Node>> &args =
                std::vector<std::unique_ptr<Node>>());

        NodeReturnType visit(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitBinOp(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitUnOp(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitLeaf(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitItem(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitDefine(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitCon(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitFor(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitWhile(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitBreak(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitContinue(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitReturn(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitIf(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitBlock(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitFuncDecl(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitFuncCall(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitImport(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitMod(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitVarAssign(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitPrint(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitTick(
            const std::unique_ptr<Node> &node, Context &ctx);
        NodeReturnType visitHold(
            const std::unique_ptr<Node> &node, Context &ctx);

        // Assumes string's first and last characters are `"`.
        void printStrlit(
            Context &ctx, Token strlit,
            std::vector<std::unique_ptr<Node>> &expressions);
        void printObject(const NodeReturnType &object, size_t indent = 0);

        Args parseArgs(
            Context &ctx,
            const std::vector<std::unique_ptr<Node>> &args);

        void populateArgs(
            Context &ctx, SymbolTable &symbolTable,
            const Args &argsDecl,
            const std::vector<std::unique_ptr<Node>> &args,
            Pos errorPos);

        // Throws error if expression is not just an identifier
        // Otherwise returns the identifier as a Token
        Token getIdenFromExpr(const std::unique_ptr<Node> &expr);
    };
}