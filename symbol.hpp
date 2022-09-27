#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include "node.hpp"
#include "errorHandler.hpp"

namespace snowlang
{
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
        static Number powOp(Number left, Number right);
        static Number remOp(Number left, Number right);
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
        SymbolTable(SymbolTable *t_parent) : parent(t_parent) {}
        SymbolTable() = default;
        void setSymbol(const std::string &name, SymbolValueType value);
        SymbolValueType &lookup(
            const std::string &name,
            int posStart,
            int posEnd);
        SymbolTable *firstAncestor();

    private:
        std::unordered_map<std::string, SymbolValueType>
            m_symbols;
        SymbolTable *parent{nullptr};
    };
}